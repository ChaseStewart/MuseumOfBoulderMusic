/******************************************************* 
 *  File: Station1.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *  NOTE: Teensyduino and USB Type: "Serial + MIDI" are REQUIRED for this firmware
 *  Tested with Teensy-LC 
 *******************************************************/
#include <NewPing.h>
#include <EEPROM.h>

#include "Preferences.h"
#include "Station1BSP.h"
#include "MIDIConstants.h"
#include "Nonvolatile.h"
#include "Ultrasonic.h"
#include "ButtonNote.h"
#include "DiscreteJoystick.h"

/* Globals */
unsigned long ping_time;
unsigned long range_in_us;
unsigned long range_in_cm;
int curr_bend_val = 1;
int prev_bend_val = 0;
int lin_pot_reading = 0;
int lin_pot_cc_val = 0;
config_t in_config = {0};

/* Static Prototypes */
static void initPins(void); // Just init the pins as input/output/input_pullup
static void pingCheck(void); // Ultrasonic callback function
static void printBanner(void); // Print a serial welcome banner

/* Global class instances */
ButtonNote ButtonNote0(STATION1_BUTTON_3, // button pin
                       MIDI_GEN_PURPOSE_1, // MIDI CC
                       BUTTON_0, // button ID
                       BUTTON_MODE_TOGGLE); // toggle or momentary button
                       
ButtonNote ButtonNote1(STATION1_BUTTON_2, // button pin
                       MIDI_GEN_PURPOSE_2, // MIDI CC
                       BUTTON_1, // button ID
                       BUTTON_MODE_MOMENTARY); // toggle or momentary button
                       
ButtonNote ButtonNote2(STATION1_BUTTON_1, // button pin
                       MIDI_GEN_PURPOSE_3, // MIDI CC 
                       BUTTON_2, // button ID
                       BUTTON_MODE_TOGGLE); // toggle or momentary button

NewPing ultrasonic(STATION1_ULTRA_TRIG, // Trigger pin
                   STATION1_ULTRA_SENS, // Sense pin
                   PREFS_ULTRA_MAX_CM); // Max distance limit

DiscreteJoystick joystick(STATION1_JOYSTICK_UP, // joystick up pin
                          STATION1_JOYSTICK_DOWN, // joystick down pin
                          STATION1_JOYSTICK_LEFT, // joystick left pin
                          STATION1_JOYSTICK_RIGHT, // joystick right pin
                          MIDI_GEN_PURPOSE_7, // MIDI CC for horizontal axis 
                          MIDI_GEN_PURPOSE_8); // MIDI CC for vertical axis

/**
 * Setup pinouts and serial 
 */
void setup() 
{
  initPins();
  digitalWrite(TEENSY_LED_PIN, LOW);

  // Arduino does not seem to support designated initializers
  in_config.octave = 48;
  in_config.root_note = 0;
  in_config.mode = MODE_MINOR;
  in_config.button1_offset = 5;
  in_config.button2_offset = 7;
  in_config.button3_offset = 12;
  in_config.button4_offset = 24;
  in_config.control_code = MIDI_GEN_PURPOSE_1;
  in_config.MIDI_Channel = EEPROM.read(EEPROM_MIDI_CHANNEL_ADDR);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG 
 
  printBanner();
}

/**
 * Just print all sensor outputs to serial to ensure proper functionality
 */
void loop() 
{
  /* Get Ultrasonic Distance sensor reading */
  if (micros() >= ping_time)
  {
    /* NOTE: due to using newPing timer, this has to indirectly set range_in_us */
    ultrasonic.ping_timer(pingCheck);
    range_in_cm = range_in_us / US_ROUNDTRIP_CM;
    ping_time += PREFS_ULTRA_PING_PERIOD;
  }

  /* constrain range_in_cm, but sufficiently low values are treated as high ones */
  if (range_in_cm < P_BEND_MIN_CM || range_in_cm > P_BEND_MAX_CM)
  {
    range_in_cm = P_BEND_MAX_CM;
  }

  /* convert ultrasonic range to value for MIDI CC and send it */
  curr_bend_val = P_BEND_ONEBYTE_VALUE(range_in_cm);
  if (curr_bend_val!= prev_bend_val && abs(curr_bend_val- prev_bend_val) < PREFS_P_BEND_ONEBYTE_MAX_DELTA)
  {
    usbMIDI.sendControlChange(MIDI_GEN_PURPOSE_5, curr_bend_val, in_config.MIDI_Channel);
  }
  prev_bend_val = curr_bend_val;

  /* handle joystick inputs and send MIDI as needed */
  joystick.UpdateXAxis(in_config);
  joystick.UpdateYAxis(in_config);

  /* Read buttons and update averaging arrays */
  ButtonNote0.Update(in_config);
  ButtonNote1.Update(in_config);
  ButtonNote2.Update(in_config);
  
  /* send MIDI CC messages if needed */
  if (ButtonNote0.ShouldSendNote()) 
    ButtonNote0.SendControlCode(in_config);
  if (ButtonNote1.ShouldSendNote()) 
    ButtonNote1.SendControlCode(in_config);
  if (ButtonNote2.ShouldSendNote())
    ButtonNote2.SendControlCode(in_config);
  
  /* Consider CapTouch sensors as triggered if any of last CAP_TOUCH_ARRAY_LEN samples were high */
  ButtonNote0.CheckMIDINeedsUpdate();
  ButtonNote1.CheckMIDINeedsUpdate();
  ButtonNote2.CheckMIDINeedsUpdate();

  /* Read in linear potentiometer values and convert to CC val */
  lin_pot_reading = analogRead(STATION1_LIN_POT);
  lin_pot_reading = constrain(lin_pot_reading, 0, 1024); 
  lin_pot_cc_val  = floor((1024 - lin_pot_reading) * 128.0/1024.0);

  /**
   * TODO FIXME currently the potentiometer cannot distinguish between a press with a low value and no press
   * There is a plan to fix this that involves adding a pressure sensor under this one.
   */
  if (lin_pot_reading > PREFS_LIN_POT_MIN_READING)
  {
    usbMIDI.sendControlChange(MIDI_GEN_PURPOSE_4, lin_pot_cc_val, in_config.MIDI_Channel);
  }

  /* Flush any queued messages */
  usbMIDI.send_now();

  /* Ignore all MIDI input */
  while (usbMIDI.read());
}

/**
 * Callback function to check whether ultrasonic sonar has returned data-
 * Provides a "spring constant" to the rangefinder reading, pushing it back to a detune of zero
 * when the user's hand is away from the rangefinder beam
 */
static void pingCheck(void)
{
  range_in_us = (ultrasonic.check_timer()) ? ultrasonic.ping_result : range_in_us +2;
}


/**
 * Just init the pins for this project- this is to de-clutter setup()
 */
static void initPins(void)
{
  pinMode(STATION1_JOYSTICK_RIGHT, INPUT_PULLUP);
  pinMode(STATION1_JOYSTICK_LEFT, INPUT_PULLUP);
  pinMode(STATION1_JOYSTICK_UP, INPUT_PULLUP);
  pinMode(STATION1_JOYSTICK_DOWN, INPUT_PULLUP);
  pinMode(STATION1_BUTTON_1, INPUT_PULLUP);
  pinMode(STATION1_BUTTON_2, INPUT_PULLUP);
  pinMode(STATION1_BUTTON_3, INPUT_PULLUP);
  pinMode(STATION1_LIN_POT, INPUT);
  pinMode(TEENSY_LED_PIN, OUTPUT);
}

/**
 * Just print a quick serial banner- this is to de-clutter setup()
 */
void printBanner(void)
{
  DEBUG_PRINTLN();
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("***********************************************");
  DEBUG_PRINTLN("*             Station 1 Firmware              *");
  DEBUG_PRINTLN("*                                             *");
  DEBUG_PRINTLN("* By Chase E. Stewart for Hidden Layer Design *");
  DEBUG_PRINTLN("***********************************************");
  DEBUG_PRINTLN();
  // TODO FIXME return info about MIDI channel and station ID
}
