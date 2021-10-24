/******************************************************* 
 *  File: PercussionStation.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *  NOTE: Teensyduino and USB Type: "Serial + MIDI" are REQUIRED for this firmware
 *  REQUIRES: Teensy 4.1 (possibly Teensy 3.6 acceptable, but untested)  
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

/* Memory arrays required by NeoPixel library */
byte NeoStick_drawingMemory[NeoStick_count*3];         //  3 bytes per LED
DMAMEM byte NeoStick_displayMemory[NeoStick_count*12]; // 12 bytes per LED

/* Config mapping of MIDI channels/ CCs */
config_t in_config = {0};

/* Pitch bend variables */
unsigned long ping_time;
unsigned long range_in_us;
unsigned long range_in_cm;
unsigned long ping_stop_time;
int curr_bend_val = 1;
int prev_bend_val = 0;

/* PIR variables */
bool pir_state = LOW;
bool prev_pir_state = HIGH;

/* Presence detection variables */
unsigned long ramp_start_millis;
unsigned long last_activity;
uint8_t prev_increment = 255;
bool ramp_is_increasing = false;
bool ramp_is_active = false;
bool current_presence = false;
bool prev_presence = false;
bool is_ramped_up = false;
bool is_ramped_down = false;

/* Static Prototypes */
static void initPins(void); // Just init the pins as input/output/input_pullup
static void pingCheck(void); // Ultrasonic callback function
static void printBanner(void); // Print a serial welcome banner
static void myControlChange(byte channel, byte control, byte value); // callback handler for reading a ControlChange from Max/MSP
static void rampUp(bool *outBool, uint8_t *prevIncrement, unsigned long start_millis, bool *is_ramped_up, bool *is_ramped_down);
static void rampDown(bool *outBool, uint8_t *prevIncrement, unsigned long start_millis, bool *is_ramped_down, bool *is_ramped_up);
static void ClearCCs(config_t in_config);
static void softRestart(void); // just restart the teensy


/* Global class instances */
ArcadeButton ArcadeButton0(PERCUSSION_STATION_BUTTON_0, PERCUSSION_STATION_LED_0, BUTTON_0);                      
ArcadeButton ArcadeButton1(PERCUSSION_STATION_BUTTON_1, PERCUSSION_STATION_LED_1, BUTTON_1);                    
ArcadeButton ArcadeButton2(PERCUSSION_STATION_BUTTON_2, PERCUSSION_STATION_LED_2, BUTTON_2);
ArcadeButton ArcadeButton3(PERCUSSION_STATION_BUTTON_3, PERCUSSION_STATION_LED_3, BUTTON_3);
ArcadeButton ArcadeButton4(PERCUSSION_STATION_BUTTON_4, PERCUSSION_STATION_LED_4, BUTTON_4);
ArcadeButton ArcadeButton5(PERCUSSION_STATION_BUTTON_5, PERCUSSION_STATION_LED_5, BUTTON_5); 
ArcadeButton ArcadeButton6(PERCUSSION_STATION_BUTTON_6, PERCUSSION_STATION_LED_6, BUTTON_6); 
ArcadeButton ArcadeButton7(PERCUSSION_STATION_BUTTON_7, PERCUSSION_STATION_LED_7, BUTTON_7); 

NewPing ultrasonic(PERCUSSION_STATION_ULTRA_TRIG, // Trigger pin
                   PERCUSSION_STATION_ULTRA_SENS, // Sense pin
                   PREFS_ULTRA_MAX_CM); // Max distance limit

WS2812Serial NeoStick(NeoStick_count, 
                      NeoStick_displayMemory, 
                      NeoStick_drawingMemory, 
                      PERCUSSION_STATION_NEO_STRIP, 
                      WS2812_GRB);

/**
 * Uncomment one of these to set the duration for the Teensy to automatically reboot
 * only 1 of these can be selected or else you will get a compilation error.
 * 
 * NOTE: CRITICAL NOTE HERE- the teensy DOES NOT sync to NTP server or anything for time of day, 
 * This will simply be a day/week/whenever from when you booted it up. To 
 */
//const unsigned long reboot_time = REBOOT_MIN_IN_MSEC;
//const unsigned long reboot_time = REBOOT_DAY_IN_MSEC;
const unsigned long reboot_time = REBOOT_WEEK_IN_MSEC;
//const unsigned long reboot_time = REBOOT_NEVER;
   


/**
 * Setup pinouts and serial 
 */
void setup() 
{
  /* Setup */
  initPins();
  digitalWrite(TEENSY_LED_PIN, HIGH);
    
  /* We will read speciific MIDI CC messages to set the countdown light */
  usbMIDI.setHandleControlChange(myControlChange);
    
  NeoStick.begin();
  myusb.begin();

  // Arduino does not seem to support designated initializers
  in_config.button0_cc   = MIDI_GEN_PURPOSE_1;
  in_config.button1_cc   = MIDI_GEN_PURPOSE_2;
  in_config.button2_cc   = MIDI_GEN_PURPOSE_3;
  in_config.button3_cc   = MIDI_GEN_PURPOSE_4;
  in_config.button4_cc   = MIDI_GEN_PURPOSE_5;
  in_config.button5_cc   = MIDI_GEN_PURPOSE_6;
  in_config.button6_cc   = MIDI_UNDEFINED_4;
  in_config.button7_cc   = MIDI_UNDEFINED_5;
  in_config.pbend_cc     = MIDI_GEN_PURPOSE_7;
  in_config.roll_pos_cc  = MIDI_EFFECT_1_DEPTH;
  in_config.roll_neg_cc  = MIDI_EFFECT_2_DEPTH;
  in_config.pitch_pos_cc = MIDI_EFFECT_3_DEPTH;
  in_config.pitch_neg_cc = MIDI_EFFECT_4_DEPTH;
  in_config.yaw_pos_cc   = MIDI_EFFECT_5_DEPTH;
  in_config.yaw_neg_cc   = MIDI_UNDEFINED_0;
  in_config.trigger_cc   = MIDI_UNDEFINED_1;
  in_config.thumb_cc     = MIDI_UNDEFINED_2;
  in_config.presence_cc  = MIDI_UNDEFINED_3;
  in_config.MIDI_Channel = EEPROM.read(EEPROM_ADDR_MIDI_CHANNEL);
  in_config.HW_Type = (stationType_t) EEPROM.read(EEPROM_ADDR_STATION_TYPE);

  ArcadeButton0.SetMIDIParams(in_config.MIDI_Channel, in_config.button0_cc);
  ArcadeButton1.SetMIDIParams(in_config.MIDI_Channel, in_config.button1_cc);
  ArcadeButton2.SetMIDIParams(in_config.MIDI_Channel, in_config.button2_cc);
  ArcadeButton3.SetMIDIParams(in_config.MIDI_Channel, in_config.button3_cc);
  ArcadeButton4.SetMIDIParams(in_config.MIDI_Channel, in_config.button4_cc);
  ArcadeButton5.SetMIDIParams(in_config.MIDI_Channel, in_config.button5_cc);
  ArcadeButton6.SetMIDIParams(in_config.MIDI_Channel, in_config.button6_cc);
  ArcadeButton7.SetMIDIParams(in_config.MIDI_Channel, in_config.button7_cc);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG 


  NeoStick.clear();
  NeoStick.show();

  ClearCCs(in_config);
  delay(10000);
  printBanner();
  printNonvolConfig();
  usbMIDI.sendControlChange(in_config.presence_cc, 73, in_config.MIDI_Channel);
  digitalWrite(TEENSY_LED_PIN, LOW); 
  ping_time = 0;
}

/**
 * Process sensor inputs to generate proper MIDI messages and LED feedback
 */
void loop() 
{
  /**
   * Reboot this teensy after the alloted amt of time
   * If reboot_time is set to REBOOT_NEVER, then never reboot the teensy
   * NOTE: Please only select one of these!
   */
   if ((millis() > reboot_time) && (REBOOT_NEVER != reboot_time))
   {
      Serial.printf("NOTE: Reboot time reached; Going for reboot in 3 seconds...\r\n");
      delay(3000);
      softRestart();
   }
  
  /* get updates from usbHost */
  myusb.Task();
  PrintDeviceListChanges();
  UpdateJoystick(in_config);

  unsigned long loopMillis = millis(); 
  /* Get Ultrasonic Distance sensor reading */
  if (loopMillis >= ping_time)
  {
    /* TODO: this is working best with a blocking call right now- boo! */
    pingCheck();
    ping_time += PREFS_ULTRA_PING_PERIOD;
  }
  prev_bend_val = curr_bend_val;

  /* Read buttons and update averaging arrays */
  ArcadeButton0.Update();
  ArcadeButton1.Update();
  ArcadeButton2.Update();
  ArcadeButton3.Update();
  ArcadeButton4.Update();
  ArcadeButton5.Update();
  ArcadeButton6.Update();
  ArcadeButton7.Update();
      
  /* Handle PIR sensor */
  pir_state = digitalRead(PERCUSSION_STATION_PIR_SENS);

  /**
   * Presence is a momentary state defined as:
   * PIR presence detected, OR
   * Arcade buttons pressed, OR
   * either Ultrasonic Sensor is changing values
   */
  current_presence = (pir_state | 
                      !ArcadeButton0.GetReading() |
                      !ArcadeButton1.GetReading() |
                      !ArcadeButton2.GetReading() |
                      !ArcadeButton3.GetReading() |
                      !ArcadeButton4.GetReading() |
                      !ArcadeButton5.GetReading() |
                      !ArcadeButton6.GetReading() |
                      !ArcadeButton7.GetReading() |
                      JoystickIsPressed() |
                      ultrasonic.check_timer()
                      );
  
  /* Keep pushing the timeout time forward if presence is still detected */
  if (current_presence)
  {
      last_activity = millis();
  } 

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
  if (millis() - ramp_start_millis > PREFS_RAMP_PERIOD)
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
  digitalWrite(TEENSY_LED_PIN, pir_state);

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
  range_in_us = ultrasonic.ping();
  range_in_cm = range_in_us / US_ROUNDTRIP_CM;
  

  if (range_in_us == 0)
  {
    curr_bend_val = (curr_bend_val >= PREFS_ULTRA_SPRINGBACK_VAL) ? curr_bend_val - PREFS_ULTRA_SPRINGBACK_VAL : 0;
  }
  else
  {
    if (range_in_cm > 0 )
    {
      range_in_cm -= 1;
    }
    
    /* convert ultrasonic range to value for MIDI CC and send it */
    curr_bend_val = ULTRA_ONEBYTE_VALUE(range_in_cm);
    curr_bend_val = constrain(curr_bend_val, 0, 127);
  }

  if(curr_bend_val != prev_bend_val && abs(curr_bend_val - prev_bend_val) < PREFS_ULTRA_ONEBYTE_MAX_DELTA)
  {
    usbMIDI.sendControlChange(in_config.pbend_cc, curr_bend_val, in_config.MIDI_Channel);
    updateNeoPixelStick(NeoStick, curr_bend_val, in_config.HW_Type);
  }
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
  pinMode(PERCUSSION_STATION_BUTTON_6, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_7, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_PIR_SENS, INPUT);
  pinMode(PERCUSSION_STATION_LED_0, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_1, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_2, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_3, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_4, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_5, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_6, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_7, OUTPUT);
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

  /**
   * TODO FIXME currently unused
   */
  //  if ((channel != PREFS_MIDI_INPUT_CHANNEL) || (control != PREFS_MIDI_INPUT_CC))
  //  {
  //    return;
  //  }
  //  updateNeoPixelStick(NeoStick, value);
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
    ArcadeButton0.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
    ArcadeButton1.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
    ArcadeButton2.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
    ArcadeButton3.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
    ArcadeButton4.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
    ArcadeButton5.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
    ArcadeButton6.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
    ArcadeButton7.SetLowValue((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS) * increment);
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
    ArcadeButton0.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    ArcadeButton1.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    ArcadeButton2.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    ArcadeButton3.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    ArcadeButton4.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    ArcadeButton5.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    ArcadeButton6.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    ArcadeButton7.SetLowValue(PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE - ((PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE/PREFS_RAMP_INCREMENTS)*increment));
    usbMIDI.sendControlChange(in_config.presence_cc, 127 - 6 * increment, in_config.MIDI_Channel);
    *prevIncrement = increment;
  }
}

static void ClearCCs(config_t in_config)
{
  usbMIDI.sendControlChange(in_config.button0_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.button1_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.button2_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.button3_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.button4_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.button5_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.button6_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.button7_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.pbend_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.trigger_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.thumb_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.roll_pos_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.roll_neg_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.pitch_pos_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.pitch_neg_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.yaw_pos_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.yaw_neg_cc, 0, in_config.MIDI_Channel);
  usbMIDI.sendControlChange(in_config.presence_cc, 73, in_config.MIDI_Channel);
}

/**
 * Reset the Teensy in software
 */
static void softRestart() 
{
  Serial.end();  //clears the serial monitor  if used
  SCB_AIRCR = 0x05FA0004;  //write value for restart
}
