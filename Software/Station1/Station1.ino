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
int curr_note_ofst = 0;
int analog_volume = 100;
int lin_pot_reading = 0;
int lin_pot_cc_val = 0;

ButtonNote ButtonNote0(STATION1_BUTTON_3, BUTTON_0);
ButtonNote ButtonNote1(STATION1_BUTTON_2, BUTTON_1);
ButtonNote ButtonNote2(STATION1_BUTTON_1, BUTTON_2);
config_t in_config;

NewPing ultrasonic(STATION1_ULTRA_TRIG, STATION1_ULTRA_SENS, ULTRA_MAX_CM);
DiscreteJoystick joystick(STATION1_JOYSTICK_UP, STATION1_JOYSTICK_DOWN, STATION1_JOYSTICK_LEFT, STATION1_JOYSTICK_RIGHT);

/* Prototypes */
static void pingCheck(void);

/**
 * Setup pinouts and serial 
 */
void setup() 
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
  digitalWrite(TEENSY_LED_PIN, LOW);

  // lack of designated initializers is unfortunate
  in_config.octave = 48;
  in_config.root_note = 0;
  in_config.scale = MOD_MINOR;
  in_config.button1_offset = 5;
  in_config.button2_offset = 7;
  in_config.button3_offset = 12;
  in_config.button4_offset = 24;
  in_config.control_code = 16;
  in_config.MIDI_Channel = EEPROM.read(EEPROM_MIDI_CHANNEL_ADDR);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG  
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
    ping_time += ULTRA_PING_PERIOD;
  }

  /* constrain range_in_cm, but sufficiently low values are treated as high ones */
  if (range_in_cm < PITCH_BEND_MIN_CM || range_in_cm > PITCH_BEND_MAX_CM)
  {
    range_in_cm = PITCH_BEND_MAX_CM;
  }
  
  curr_bend_val = SCALED_PITCH_BEND(range_in_cm);
  if (curr_bend_val!= prev_bend_val && abs(curr_bend_val- prev_bend_val) < MAX_PITCH_BEND_DELTA)
  {
    usbMIDI.sendPitchBend(curr_bend_val, in_config.MIDI_Channel);
    prev_bend_val = curr_bend_val;
  }

  joystick.UpdateNote(&curr_note_ofst);
  joystick.UpdateVolume(&analog_volume);

  /* Send note on debounced rising edge of TEENSY_CAP_TOUCH1_PIN */
  ButtonNote0.Update();
  ButtonNote1.Update();
  ButtonNote2.Update();
  
  /* send notes if needed */
  if (ButtonNote0.ShouldSendNote(curr_note_ofst, analog_volume)) 
    ButtonNote0.SendNote(curr_note_ofst, analog_volume, in_config);
  if (ButtonNote1.ShouldSendNote(curr_note_ofst, analog_volume)) 
    ButtonNote1.SendNote(curr_note_ofst, analog_volume, in_config);
  if (ButtonNote2.ShouldSendNote(curr_note_ofst, analog_volume))
    ButtonNote2.SendNote(curr_note_ofst, analog_volume, in_config);
  
  /* Consider CapTouch sensors as triggered if any of last CAP_TOUCH_ARRAY_LEN samples were high */
  ButtonNote0.CheckMIDINeedsUpdate();
  ButtonNote1.CheckMIDINeedsUpdate();
  ButtonNote2.CheckMIDINeedsUpdate();

  lin_pot_reading = analogRead(STATION1_LIN_POT);
  lin_pot_reading = constrain(lin_pot_reading, 0, 1024); 

  lin_pot_cc_val  = floor((1024 - lin_pot_reading) * 128.0/1024.0);

  if (lin_pot_reading > LIN_POT_MIN_READING)
  {
    usbMIDI.sendControlChange(in_config.control_code, lin_pot_cc_val, in_config.MIDI_Channel);
  }


  /* Flush any queued messages */
  usbMIDI.send_now();

  /* Ignore all MIDI input */
  while (usbMIDI.read());
}

/**
 * Callback function to check whether ultrasonic sonar has returned data
 */
static void pingCheck(void)
{
  range_in_us = (ultrasonic.check_timer()) ? ultrasonic.ping_result : range_in_us +2;
}
