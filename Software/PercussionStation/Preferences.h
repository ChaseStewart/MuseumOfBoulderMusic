/******************************************************* 
 *  File: Preferences.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __PREFERENCES_H__
#define __PREFERENCES_H__

#include "Arduino.h"
#include "MIDIConstants.h"
#include "Nonvolatile.h" // for stationType_t

/* Constants for automatic reboot */
const unsigned int REBOOT_MIN_IN_MSEC  = 60000;
const unsigned int REBOOT_DAY_IN_MSEC  = 86400000;
const unsigned int REBOOT_WEEK_IN_MSEC = 604800000;
const unsigned int REBOOT_NEVER        = 0;

#define DEBUG

/**
 * Silence all printouts if not in DEBUG
 * 
 * NOTE: Set DEBUG definition in Preferences.h
 */
#ifdef DEBUG
#define DEBUG_PRINTLN(x)  Serial.println(x)
#define DEBUG_PRINT(x)  Serial.print(x)
#define DEBUG_PRINTLN_HEX(x) Serial.println (x, HEX)
#define DEBUG_PRINT_HEX(x) Serial.print (x, HEX)
#define DEBUG_PRINT_DEC4(x) Serial.print((float) x, 4)
#define DEBUG_PRINT_DEC3(x) Serial.print((float )x, 3)
#else
#define DEBUG_PRINTLN(x)
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN_HEX(x) 
#define DEBUG_PRINT_HEX(x)
#define DEBUG_PRINT_DEC4(x) 
#define DEBUG_PRINT_DEC3(x)
#endif /* DEBUG */


/* Ultrasonic / pitch bend preferences */
#define PREFS_ULTRA_MAX_CM 30 // change this to change the far range of what the ultrasonic can "see" - in centimeters
#define PREFS_ULTRA_PING_PERIOD (unsigned long) (33)
#define PREFS_ULTRA_MAX_DELTA 1700
#define PREFS_ULTRA_ONEBYTE_MAX_DELTA 8
#define PREFS_ULTRA_SPRINGBACK_VAL 5 // amount of "spring constant" for when ultrasonic is not being controlled by a user. Units are 0-127 CC value

/* Button preferences */
#define PREFS_BUTTON_DEBOUNCE_MSEC 10
#define PREFS_BUTTON_CC_LOW_VAL 0 // what CC corresponds to button OFF
#define PREFS_BUTTON_CC_HIGH_VAL 127 // what CC val corresponds to button ON
#define PREFS_ARCADE_BUTTON_PWM_HIGH 255 // Button brightness from 0-255 that determines how lit up the arcade buttons are when pressed
#define PREFS_ARCADE_BUTTON_PWM_LOW_ABSENCE 0 // Button brightness when completely off/ presence is not detected
#define PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE 10 // Button brightnes when presence is detected but the button is not currently being pressed Top of ramp value

/* Joystick preferences */
#define PREFS_JOYSTICK_DEBOUNCE_MSEC 300
#define PREFS_JOYSTICK_DEADZONE_LOW 124
#define PREFS_JOYSTICK_DEADZONE_HIGH 131

/* Linear potentiometer preferences */
#define PREFS_LIN_POT_MIN_READING 10

/* MIDI preferences */
#define PREFS_MIDI_INPUT_CHANNEL   MIDI_CHANNEL_15    // currently unused
#define PREFS_MIDI_INPUT_CC        MIDI_GEN_PURPOSE_8 // currently unused  

#define PREFS_RAMP_PERIOD 500
#define PREFS_RAMP_INCREMENTS 10
#define PREFS_ACTIVITY_TIMEOUT (300 * 1000) // 5 minutes 

/**
 * Struct that holds configuration for PercussionStation 
 */
typedef struct 
{
  uint8_t button0_cc;
  uint8_t button1_cc;
  uint8_t button2_cc;
  uint8_t button3_cc;
  uint8_t button4_cc;
  uint8_t button5_cc;
  uint8_t button6_cc;
  uint8_t button7_cc;
  uint8_t roll_pos_cc;
  uint8_t roll_neg_cc;
  uint8_t pitch_pos_cc;
  uint8_t pitch_neg_cc;
  uint8_t yaw_pos_cc;
  uint8_t yaw_neg_cc;
  uint8_t trigger_cc;
  uint8_t thumb_cc;
  uint8_t pbend_cc;
  uint8_t presence_cc;
  uint8_t MIDI_Channel;
  stationType_t HW_Type;
  neoStripInvertState_t invert;
} config_t;

#endif //__PREFERENCES_H__
