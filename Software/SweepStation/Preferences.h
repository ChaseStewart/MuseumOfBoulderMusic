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
#define PREFS_ULTRA_MAX_CM 30
#define PREFS_ULTRA_PING_PERIOD (unsigned long) (3)
#define PREFS_P_BEND_MAX_DELTA 1700
#define PREFS_P_BEND_ONEBYTE_MAX_DELTA 10

/* Button preferences */
#define PREFS_BUTTON_DEBOUNCE_MSEC 10
#define PREFS_BUTTON_CC_LOW_VAL 0 // what CC corresponds to button OFF
#define PREFS_BUTTON_CC_HIGH_VAL 127 // what CC val corresponds to button ON
#define PREFS_ARCADE_BUTTON_PWM_HIGH 200
#define PREFS_ARCADE_BUTTON_PWM_LOW_ABSENCE 0
#define PREFS_ARCADE_BUTTON_PWM_LOW_PRESENCE 30

/* MIDI preferences */
#define PREFS_MIDI_INPUT_CHANNEL   MIDI_CHANNEL_15
#define PREFS_MIDI_INPUT_CC        MIDI_GEN_PURPOSE_8   

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
  uint8_t pbend_left_cc;
  uint8_t pbend_right_cc;
  uint8_t presence_cc;
  uint8_t MIDI_Channel;
} config_t;

#endif //__PREFERENCES_H__
