/******************************************************* 
 *  File: PercussionStation.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *  NOTE: Teensyduino and USB Type: "Serial + MIDI" are REQUIRED for this firmware
 *  Tested with Teensy-LC 
 *******************************************************/
#include <EEPROM.h>
#include <NewPing.h>
#include "USBHost_t36.h"

#include "Preferences.h"
#include "PercussionStationBSP.h"
#include "MIDIConstants.h"
#include "Nonvolatile.h"
#include "Ultrasonic.h"
#include "ArcadeButton.h"
#include "NeoPixel.h"
#include "USBJoystick.h"


byte NeoStick_drawingMemory[NeoStick_count*3];         //  3 bytes per LED
DMAMEM byte NeoStick_displayMemory[NeoStick_count*12]; // 12 bytes per LED

byte NeoButtons_drawingMemory[NeoButtons_count*3];         //  3 bytes per LED
DMAMEM byte NeoButtons_displayMemory[NeoButtons_count*12]; // 12 bytes per LED

/* Globals */
unsigned long ping_time;
unsigned long range_in_us;
unsigned long range_in_cm;
unsigned long ramp_end_millis;

uint8_t prev_increment = 255;
bool ramp_is_increasing = false;
bool ramp_is_active = false;
bool trigger_state = LOW;
bool prev_trigger_state = HIGH;
bool thumb_state = LOW;
bool prev_thumb_state = HIGH;

uint8_t prev_roll = 0;
uint8_t prev_pitch = 0;
uint8_t prev_yaw = 0;


bool pir_state = LOW;
bool prev_pir_state = HIGH;
int curr_bend_val = 1;
int prev_bend_val = 0;
config_t in_config = {0};

/* Static Prototypes */
static void initPins(void); // Just init the pins as input/output/input_pullup
static void pingCheck(void); // Ultrasonic callback function
static void printBanner(void); // Print a serial welcome banner
static void myControlChange(byte channel, byte control, byte value); // callback handler for reading a ControlChange from Max/MSP
static void updateRamp(bool *outBool, bool ramp_is_increasing, unsigned long end_millis);

/* Global class instances */
ArcadeButton ArcadeButton0(PERCUSSION_STATION_BUTTON_0, PERCUSSION_STATION_LED_0, BUTTON_0);                      
ArcadeButton ArcadeButton1(PERCUSSION_STATION_BUTTON_1, PERCUSSION_STATION_LED_1, BUTTON_1);                    
ArcadeButton ArcadeButton2(PERCUSSION_STATION_BUTTON_2, PERCUSSION_STATION_LED_2, BUTTON_2);
ArcadeButton ArcadeButton3(PERCUSSION_STATION_BUTTON_3, PERCUSSION_STATION_LED_3, BUTTON_3);
ArcadeButton ArcadeButton4(PERCUSSION_STATION_BUTTON_4, PERCUSSION_STATION_LED_4, BUTTON_4);
ArcadeButton ArcadeButton5(PERCUSSION_STATION_BUTTON_5, PERCUSSION_STATION_LED_5, BUTTON_5); 

NewPing ultrasonic(PERCUSSION_STATION_ULTRA_TRIG, // Trigger pin
                   PERCUSSION_STATION_ULTRA_SENS, // Sense pin
                   PREFS_ULTRA_MAX_CM); // Max distance limit

WS2812Serial NeoStick(NeoStick_count, 
                      NeoStick_displayMemory, 
                      NeoStick_drawingMemory, 
                      PERCUSSION_STATION_NEO_STRIP, 
                      WS2812_GRB);
                      
WS2812Serial NeoButtons(NeoButtons_count,
                        NeoButtons_displayMemory, 
                        NeoButtons_drawingMemory, 
                        PERCUSSION_STATION_NEO_BUTTONS, 
                        WS2812_GRB);

/**
 * Setup pinouts and serial 
 */
void setup() 
{
  /* Setup */
  initPins();
  
  /* We will read speciific MIDI CC messages to set the countdown light */
  usbMIDI.setHandleControlChange(myControlChange);
    
  NeoStick.begin();
  NeoButtons.begin();
  myusb.begin();

  // Arduino does not seem to support designated initializers
  in_config.button0_cc = MIDI_GEN_PURPOSE_1;
  in_config.button1_cc = MIDI_GEN_PURPOSE_2;
  in_config.button2_cc = MIDI_GEN_PURPOSE_3;
  in_config.button3_cc = MIDI_GEN_PURPOSE_4;
  in_config.button4_cc = MIDI_GEN_PURPOSE_5;
  in_config.button5_cc = MIDI_GEN_PURPOSE_6;
  in_config.pbend_cc   = MIDI_GEN_PURPOSE_7;
  in_config.roll_cc    = MIDI_EFFECT_1_DEPTH;
  in_config.pitch_cc   = MIDI_EFFECT_2_DEPTH;
  in_config.yaw_cc     = MIDI_EFFECT_3_DEPTH;
  in_config.trigger_cc = MIDI_EFFECT_4_DEPTH;
  in_config.thumb_cc   = MIDI_EFFECT_5_DEPTH;
  in_config.MIDI_Channel = EEPROM.read(EEPROM_ADDR_MIDI_CHANNEL);

  ArcadeButton0.SetMIDIParams(in_config.MIDI_Channel, in_config.button0_cc);
  ArcadeButton1.SetMIDIParams(in_config.MIDI_Channel, in_config.button1_cc);
  ArcadeButton2.SetMIDIParams(in_config.MIDI_Channel, in_config.button2_cc);
  ArcadeButton3.SetMIDIParams(in_config.MIDI_Channel, in_config.button3_cc);
  ArcadeButton4.SetMIDIParams(in_config.MIDI_Channel, in_config.button4_cc);
  ArcadeButton5.SetMIDIParams(in_config.MIDI_Channel, in_config.button5_cc);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG 

  printBanner();
  printNonvolConfig();

  NeoButtons.clear();
  NeoStick.clear();
  
  NeoButtons.show();
  NeoStick.show();
}

/**
 * Just print all sensor outputs to serial to ensure proper functionality
 */
void loop() 
{
  /* get updates from usbHost */
  myusb.Task();
  PrintDeviceListChanges();

  /* TODO move all of this into USBJoystick.x */
  if (joysticks.available()) 
  {
    /* Get joystick button state */
    uint32_t buttons = joysticks.getButtons();
    thumb_state = (buttons & BUTTON_MASK_THUMB);
    trigger_state = (buttons & BUTTON_MASK_TRIGGER);

    /* Get joystick axis state */
    uint32_t raw_pitch = joysticks.getAxis(JOYSTICK_AXIS_PITCH) / 8;
    uint8_t pitch = (uint8_t) constrain(raw_pitch, 0, 127);
    uint32_t raw_roll = joysticks.getAxis(JOYSTICK_AXIS_ROLL) / 8;
    uint8_t roll = (uint8_t) constrain(raw_roll, 0, 127);
    uint32_t raw_yaw = joysticks.getAxis(JOYSTICK_AXIS_YAW) / 2;
    uint8_t yaw = (uint8_t) constrain(raw_yaw, 0, 127);
    
    if (thumb_state != prev_thumb_state)
    {
      usbMIDI.sendControlChange(in_config.thumb_cc, (thumb_state) ? PREFS_BUTTON_CC_LOW_VAL: PREFS_BUTTON_CC_HIGH_VAL, in_config.MIDI_Channel);   
    }
    if (trigger_state != prev_trigger_state)
    {
      usbMIDI.sendControlChange(in_config.trigger_cc, (trigger_state) ? PREFS_BUTTON_CC_LOW_VAL: PREFS_BUTTON_CC_HIGH_VAL, in_config.MIDI_Channel);   
    }
    if (roll != prev_roll)
    {
      usbMIDI.sendControlChange(in_config.roll_cc,  roll,  in_config.MIDI_Channel);
    }
    if (pitch != prev_pitch)
    {
      usbMIDI.sendControlChange(in_config.pitch_cc, pitch, in_config.MIDI_Channel);
    }
    if (yaw != prev_yaw)
    {
      usbMIDI.sendControlChange(in_config.yaw_cc, yaw,     in_config.MIDI_Channel);
    }
   
    joysticks.joystickDataClear();

    prev_thumb_state = thumb_state;
    prev_trigger_state = trigger_state;
    prev_roll = roll;
    prev_yaw = yaw;
    prev_pitch = pitch;
  }
  

  
  /* Get Ultrasonic Distance sensor reading */
  if (micros() >= ping_time)
  {
    /* NOTE: due to using newPing timer, this has to indirectly set range_in_us */
    ultrasonic.ping_timer(pingCheck);
    range_in_cm = range_in_us / US_ROUNDTRIP_CM;
    ping_time += PREFS_ULTRA_PING_PERIOD;
  }

  /* constrain range_in_cm, but sufficiently low values are treated as high ones */
  if ((range_in_cm < P_BEND_MIN_CM) || (range_in_cm > P_BEND_MAX_CM))
  {
    range_in_cm = P_BEND_MAX_CM;
  }

  /* convert ultrasonic range to value for MIDI CC and send it */
  curr_bend_val = P_BEND_ONEBYTE_VALUE(range_in_cm);
  if ((curr_bend_val != prev_bend_val) && (abs(curr_bend_val- prev_bend_val) < PREFS_P_BEND_ONEBYTE_MAX_DELTA))
  {
    usbMIDI.sendControlChange(in_config.pbend_cc, curr_bend_val, in_config.MIDI_Channel);
  }
  prev_bend_val = curr_bend_val;

  /* Read buttons and update averaging arrays */
  ArcadeButton0.Update();
  ArcadeButton1.Update();
  ArcadeButton2.Update();
  ArcadeButton3.Update();
  ArcadeButton4.Update();
  ArcadeButton5.Update();
      
  /* Handle PIR sensor */
  pir_state = digitalRead(PERCUSSION_STATION_PIR_SENS);

  /* Only catch state transitions */
  if (pir_state != prev_pir_state)
  {
    ramp_is_active = true;
    ramp_end_millis = millis() + 1000;
    prev_increment = 255;
    
    if (0 != pir_state)
    {
      ramp_is_increasing = true;
      digitalWrite(TEENSY_LED_PIN, HIGH);
    }
    else
    {
      ramp_is_increasing = false;
      digitalWrite(TEENSY_LED_PIN, LOW);
    }
  }
  prev_pir_state = pir_state;
  updateRamp(&ramp_is_active, &prev_increment, ramp_is_increasing, ramp_end_millis);

  /* Flush any queued messages */
  usbMIDI.send_now();

  /* This will trigger the myControlChange callback as needed */
  while (usbMIDI.read(PREFS_MIDI_INPUT_CHANNEL))
  {
    // no-op, handled by callback(s)  
  }
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
  pinMode(PERCUSSION_STATION_BUTTON_0, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_1, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_2, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_3, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_4, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_5, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_PIR_SENS, INPUT);
  pinMode(PERCUSSION_STATION_LED_0, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_1, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_2, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_3, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_4, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_5, OUTPUT);

  pinMode(TEENSY_LED_PIN, OUTPUT);
  digitalWrite(TEENSY_LED_PIN, LOW);
}


/**
 * Just print a quick serial banner- this is to de-clutter setup()
 */
void printBanner(void)
{
  DEBUG_PRINTLN();
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("**************************************************");
  DEBUG_PRINTLN("*             Percussion Station Firmware        *");
  DEBUG_PRINTLN("*                                                *");
  DEBUG_PRINTLN("* By Chase E. Stewart for Hidden Layer Design    *");
  DEBUG_PRINTLN("*                                                *");

#if !defined(CORE_TEENSY)
  DEBUG_PRINTLN("* Running on non-Teensy!                         *");
#elif defined(__IMXRT1062__)
  DEBUG_PRINTLN("* Running on a Teensy 4.x                        *");
#elif !defined(__arm__)
  DEBUG_PRINTLN("* Running on a non-arm Teensy like the 2.0/2.1++ *");
#elif defined(__MKL26Z64__)
  DEBUG_PRINTLN("* Running on a Teensy LC                         *");
#elif defined(__MK20DX256__)
  DEBUG_PRINTLN("* Running on a Teensy 3.1                        *");
#elif defined(__MK20DX128__)
  DEBUG_PRINTLN("* Running on a Teensy 3.0                        *");
#else
  DEBUG_PRINTLN("* Running on an unknown Teensy processor         *");
#endif
  DEBUG_PRINTLN("**************************************************");
  DEBUG_PRINTLN();
}


/**
 * Callback function for receiving MIDI control change back from Max/MSP
 */
static void myControlChange(byte channel, byte control, byte value)
{
  if ((channel != PREFS_MIDI_INPUT_CHANNEL) || (control != PREFS_MIDI_INPUT_CC))
  {
    return;
  }
  updateNeoPixelStick(NeoStick, value);
}


static void updateRamp(bool *outBool, uint8_t *prevIncrement, bool ramp_is_increasing, unsigned long end_millis)
{
  if (false == *outBool) return;
  
  unsigned long currentMillis = millis();
  if (currentMillis > end_millis) 
  {
    *outBool = false;
    return;
  }
  
  uint8_t increment = (1000 - (end_millis - currentMillis)) / 100;
  
  if (increment != *prevIncrement)
  {
    Serial.printf("Increment=%d, currMillis=%d, endMillis=%d\r\n", increment, currentMillis, end_millis);
    if (ramp_is_increasing)
    {
        ArcadeButton0.SetLowValue(5*increment);
        ArcadeButton1.SetLowValue(5*increment);
        ArcadeButton2.SetLowValue(5*increment);
        ArcadeButton3.SetLowValue(5*increment);
        ArcadeButton4.SetLowValue(5*increment);
        ArcadeButton5.SetLowValue(5*increment);
        NeoButtons.setPixel(0, 5*increment,5*increment,5*increment);
        NeoButtons.show();
    }
    else
    {
        ArcadeButton0.SetLowValue(50 - 5*increment);
        ArcadeButton1.SetLowValue(50 - 5*increment);
        ArcadeButton2.SetLowValue(50 - 5*increment);
        ArcadeButton3.SetLowValue(50 - 5*increment);
        ArcadeButton4.SetLowValue(50 - 5*increment);
        ArcadeButton5.SetLowValue(50 - 5*increment);
        NeoButtons.setPixel(0, 50-5*increment,50-5*increment,50-5*increment);
        NeoButtons.show();  
    }
    *prevIncrement = increment;
  }
}
