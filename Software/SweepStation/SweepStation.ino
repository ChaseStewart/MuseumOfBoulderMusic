/******************************************************* 
 *  File: SweepStation.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *  NOTE: Teensyduino and USB Type: "Serial + MIDI" are REQUIRED for this firmware
 *  Tested with Teensy 4.1 
 *******************************************************/
#include <EEPROM.h>
#include <NewPing.h>

#include "Preferences.h"
#include "SweepStationBSP.h"
#include "MIDIConstants.h"
#include "Nonvolatile.h"
#include "Ultrasonic.h"
#include "ArcadeButton.h"
#include "NeoPixel.h"

/* Memory arrays required by NeoPixel library */
byte NeoStickLeft_drawingMemory[NeoStick_count*3];         //  3 bytes per LED
DMAMEM byte NeoStickLeft_displayMemory[NeoStick_count*12]; // 12 bytes per LED

byte NeoStickRight_drawingMemory[NeoStick_count*3];         //  3 bytes per LED
DMAMEM byte NeoStickRight_displayMemory[NeoStick_count*12]; // 12 bytes per LED

/* Config mapping of MIDI channels/ CCs */
config_t in_config = {0};

/* Pitch bend variables */
unsigned long left_ping_time;
unsigned long left_range_in_us;
unsigned long left_range_in_cm;
unsigned long right_ping_time;
unsigned long right_range_in_us;
unsigned long right_range_in_cm;
int left_curr_bend_val = 1;
int left_prev_bend_val = 0;
int right_curr_bend_val = 1;
int right_prev_bend_val = 0;

/* PIR variables */
bool pir_state = LOW;
bool prev_pir_state = HIGH;

/* Presence detection variables */
unsigned long ramp_start_millis;
unsigned long last_activity;
bool is_ramped_up = false;
bool is_ramped_down = false;
uint8_t prev_increment = 255;
bool ramp_is_increasing = false;
bool ramp_is_active = false;
bool current_presence = false;
bool prev_presence = false;

/* Static Prototypes */
static void initPins(void); // Just init the pins as input/output/input_pullup
static void pingCheck(void); // Ultrasonic callback function
static void printBanner(void); // Print a serial welcome banner
static void myControlChange(byte channel, byte control, byte value); // callback handler for reading a ControlChange from Max/MSP
static void rampUp(bool *outBool, bool ramp_is_increasing, unsigned long start_millis, bool *is_ramped_up, bool *is_ramped_down);
static void rampDown(bool *outBool, bool ramp_is_increasing, unsigned long start_millis, bool *is_ramped_down, bool *is_ramped_up);

/* Global class instances */
ArcadeButton ArcadeButton0(SWEEP_STATION_BUTTON_0, SWEEP_STATION_LED_0, BUTTON_0);                      
ArcadeButton ArcadeButton1(SWEEP_STATION_BUTTON_1, SWEEP_STATION_LED_1, BUTTON_1);                    

NewPing ultrasonicLeft( SWEEP_STATION_LEFT_ULTRA_TRIG, // Trigger pin
                   SWEEP_STATION_LEFT_ULTRA_SENS, // Sense pin
                   PREFS_ULTRA_MAX_CM); // Max distance limit

NewPing ultrasonicRight(SWEEP_STATION_RIGHT_ULTRA_TRIG, // Trigger pin
                   SWEEP_STATION_RIGHT_ULTRA_SENS, // Sense pin
                   PREFS_ULTRA_MAX_CM); // Max distance limit

WS2812Serial NeoStickLeft(NeoStick_count, 
                      NeoStickLeft_displayMemory, 
                      NeoStickLeft_drawingMemory, 
                      SWEEP_STATION_NEO_STRIP_LEFT, 
                      WS2812_GRB);

WS2812Serial NeoStickRight(NeoStick_count, 
                      NeoStickRight_displayMemory, 
                      NeoStickRight_drawingMemory, 
                      SWEEP_STATION_NEO_STRIP_RIGHT, 
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

  NeoStickLeft.begin();
  NeoStickRight.begin();

  // Arduino does not seem to support designated initializers
  in_config.button0_cc     = MIDI_GEN_PURPOSE_1;
  in_config.button1_cc     = MIDI_GEN_PURPOSE_2;
  in_config.pbend_left_cc  = MIDI_GEN_PURPOSE_3;
  in_config.pbend_right_cc = MIDI_GEN_PURPOSE_4;
  in_config.presence_cc    = MIDI_GEN_PURPOSE_5;
  in_config.MIDI_Channel   = EEPROM.read(EEPROM_ADDR_MIDI_CHANNEL);

  ArcadeButton0.SetMIDIParams(in_config.MIDI_Channel, in_config.button0_cc);
  ArcadeButton1.SetMIDIParams(in_config.MIDI_Channel, in_config.button1_cc);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG 

  printBanner();
  printNonvolConfig();

  NeoStickLeft.clear();
  NeoStickRight.clear();  
  NeoStickLeft.show();
  NeoStickRight.show();
  digitalWrite(TEENSY_LED_PIN, LOW);
}

/**
 * Process sensor inputs to generate proper MIDI messages and LED feedback
 */
void loop() 
{
  /* Get Right Ultrasonic Distance sensor reading */
  if (micros() >= left_ping_time)
  {
    /* NOTE: due to using newPing timer, this has to indirectly set range_in_us */
    ultrasonicLeft.ping_timer(pingCheckLeft);
    left_range_in_cm = left_range_in_us / US_ROUNDTRIP_CM;
    left_ping_time += PREFS_ULTRA_PING_PERIOD;
  }

  /* constrain range_in_cm, but sufficiently low values are treated as high ones */
  if ((left_range_in_cm < P_BEND_MIN_CM) || (left_range_in_cm > P_BEND_MAX_CM))
  {
    left_range_in_cm = P_BEND_MAX_CM;
  }

  /* convert Right ultrasonic range to value for MIDI CC and send it */
  left_curr_bend_val = P_BEND_ONEBYTE_VALUE(left_range_in_cm);
  if ((left_curr_bend_val != left_prev_bend_val) && (abs(left_curr_bend_val - left_prev_bend_val) < PREFS_P_BEND_ONEBYTE_MAX_DELTA))
  {
    usbMIDI.sendControlChange(in_config.pbend_left_cc, left_curr_bend_val, in_config.MIDI_Channel);
    updateNeoPixelStick(NeoStickLeft, left_curr_bend_val);
  }

  /* Get Left Ultrasonic Distance sensor reading */
  if (micros() >= right_ping_time)
  {
    /* NOTE: due to using newPing timer, this has to indirectly set range_in_us */
    ultrasonicRight.ping_timer(pingCheckRight);
    right_range_in_cm = right_range_in_us / US_ROUNDTRIP_CM;
    right_ping_time += PREFS_ULTRA_PING_PERIOD;
  }

  /* constrain range_in_cm, but sufficiently low values are treated as high ones */
  if ((right_range_in_cm < P_BEND_MIN_CM) || (right_range_in_cm > P_BEND_MAX_CM))
  {
    right_range_in_cm = P_BEND_MAX_CM;
  }

  /* convert Left ultrasonic range to value for MIDI CC and send it */
  right_curr_bend_val = P_BEND_ONEBYTE_VALUE(right_range_in_cm);
  if ((right_curr_bend_val != right_prev_bend_val) && (abs(right_curr_bend_val - right_prev_bend_val) < PREFS_P_BEND_ONEBYTE_MAX_DELTA))
  {
    usbMIDI.sendControlChange(in_config.pbend_right_cc, right_curr_bend_val, in_config.MIDI_Channel);
    updateNeoPixelStick(NeoStickRight, right_curr_bend_val);
  }
  left_prev_bend_val = left_curr_bend_val;
  right_prev_bend_val = right_curr_bend_val;


  /* Read buttons and update averaging arrays */
  ArcadeButton0.Update();
  ArcadeButton1.Update();
      
  /* Handle PIR sensor */
  pir_state = digitalRead(SWEEP_STATION_PIR_SENS);

  /**
   * Presence is a momentary state defined as:
   * PIR presence detected, OR
   * Arcade buttons pressed, OR
   * either Ultrasonic Sensor is changing values
   */
  current_presence = (pir_state | 
                      !ArcadeButton0.GetReading() |
                      !ArcadeButton1.GetReading() |
                      ultrasonicLeft.check_timer() | 
                      ultrasonicRight.check_timer()
                      );

  /* Keep pushing the timeout time forward if presence is still detected */
  if (current_presence)
  {
      last_activity = millis();
  }
  
  /* Debug for PIR state- we can turn this off if it becomes distracting */
  digitalWrite(TEENSY_LED_PIN, pir_state);  

  /**
   * Start rampUp() if we meet the following criteria: 
   * 1. We just had presence this loop, 
   * 2. ramp isn't running, 
   * 3. we previously ramped down 
   */
  if (!ramp_is_active && !ramp_is_increasing && current_presence && !prev_presence)
  {
    ramp_is_active = true;
    ramp_start_millis = millis();
    prev_increment = 255;
    ramp_is_increasing = true;
  }

  /**
   * Start rampDown() if we meet the following criteria: 
   * 1. We have had absence for PREFS_ACTIVITY_TIMEOUT, 
   * 2. ramp isn't running, 
   * 3. we previously ramped up 
   */
  if (!ramp_is_active && ramp_is_increasing && millis() - last_activity > PREFS_ACTIVITY_TIMEOUT)
  {
    ramp_is_active = true;
    ramp_start_millis = millis();
    prev_increment = 255;
    ramp_is_increasing = false;
  }

  /* Stop the ramp from running PREFS_RAMP_PERIOD millis() after the ramp */ 
  if (ramp_is_active && (millis() - ramp_start_millis > PREFS_RAMP_PERIOD))
  {
    ramp_is_active = false;  
  }

  /* Call rampUp() or rampDown() if appropriate */
  if (ramp_is_active && ramp_is_increasing)
  {
    rampUp(&ramp_is_active, &prev_increment, ramp_start_millis, &is_ramped_up, &is_ramped_down);
  }
  else if (ramp_is_active && !ramp_is_increasing)
  {
    rampDown(&ramp_is_active, &prev_increment, ramp_start_millis, &is_ramped_down, &is_ramped_up);  
  }
  prev_presence = current_presence;
  prev_pir_state = pir_state;

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
 * 
 * NOTE: Identical to pingCheckRight- this is good enough for now
 */
static void pingCheckLeft(void)
{
  left_range_in_us = (ultrasonicLeft.check_timer()) ? ultrasonicLeft.ping_result : left_range_in_us +2;
}

/**
 * Callback function to check whether ultrasonic sonar has returned data-
 * Provides a "spring constant" to the rangefinder reading, pushing it back to a detune of zero
 * when the user's hand is away from the rangefinder beam
 * 
 * NOTE: Identical to pingCheckLeft- this is good enough for now
 */
static void pingCheckRight(void)
{
  right_range_in_us = (ultrasonicRight.check_timer()) ? ultrasonicRight.ping_result : right_range_in_us +2;
}

/**
 * Just init the pins for this project- this is to de-clutter setup()
 */
static void initPins(void)
{
  pinMode(SWEEP_STATION_BUTTON_0, INPUT_PULLUP);
  pinMode(SWEEP_STATION_BUTTON_1, INPUT_PULLUP);
  pinMode(SWEEP_STATION_PIR_SENS, INPUT);
  pinMode(SWEEP_STATION_LED_0, OUTPUT);
  pinMode(SWEEP_STATION_LED_1, OUTPUT);
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
  DEBUG_PRINTLN("*                Sweep Station Firmware          *");
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
  // this is a no-op for the sweep stations
}

/**
 * Create a non-blocking ramp up from absence to presence. 
 */
static void rampUp(bool *outBool, uint8_t *prevIncrement, unsigned long start_millis, bool *is_ramped_up, bool *is_ramped_down)
{
  if (true == *is_ramped_up) return;
  if (false == *outBool) return;
  
  unsigned long currentMillis = millis();
  if (currentMillis - start_millis > PREFS_RAMP_PERIOD) 
  {
    *outBool = false;
    *is_ramped_down = false;
    *is_ramped_up = true;
    return;
  }
  
  uint8_t increment = (PREFS_RAMP_PERIOD - ((start_millis + PREFS_RAMP_PERIOD) - currentMillis)) / (PREFS_RAMP_PERIOD / PREFS_RAMP_INCREMENTS);
  if (increment != *prevIncrement && increment < PREFS_RAMP_INCREMENTS)
  {
    ArcadeButton0.SetLowValue(5*increment);
    ArcadeButton1.SetLowValue(5*increment);
    usbMIDI.sendControlChange(in_config.presence_cc, 73 + 6 * increment, in_config.MIDI_Channel);
    *prevIncrement = increment;
  }
}

/**
 * Create a non-blocking ramp down from presence to absence. 
 */
static void rampDown(bool *outBool, uint8_t *prevIncrement, unsigned long start_millis, bool *is_ramped_down, bool *is_ramped_up)
{
  if (true == *is_ramped_down) return;
  if (false == *outBool) return;
  
  unsigned long currentMillis = millis();
  if (currentMillis - start_millis > PREFS_RAMP_PERIOD) 
  {
    *outBool = false;
    *is_ramped_down = true;
    *is_ramped_up = false; 
    return;
  }
  
  uint8_t increment = (PREFS_RAMP_PERIOD - ((start_millis + PREFS_RAMP_PERIOD) - currentMillis)) / (PREFS_RAMP_PERIOD / PREFS_RAMP_INCREMENTS);
  if (increment != *prevIncrement && increment < PREFS_RAMP_INCREMENTS)
  {
    ArcadeButton0.SetLowValue(50 - 5*increment);
    ArcadeButton1.SetLowValue(50 - 5*increment);
    usbMIDI.sendControlChange(in_config.presence_cc, 127 - 6 * increment, in_config.MIDI_Channel);
    *prevIncrement = increment;
  }
}