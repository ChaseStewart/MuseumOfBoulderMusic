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
#define PREFS_BUTTON_CC_HI_VAL 127 // what CC val corresponds to button ON

/* Joystick preferences */
#define PREFS_JOYSTICK_DEBOUNCE_MSEC 300

/* Linear potentiometer preferences */
#define PREFS_LIN_POT_MIN_READING 10

/* MIDI preferences */
#define PREFS_MIDI_INPUT_CHANNEL   MIDI_CHANNEL_15
#define PREFS_MIDI_INPUT_CC        MIDI_GEN_PURPOSE_8   

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
  uint8_t pbend_cc;
  uint8_t MIDI_Channel;
} config_t;

#endif //__PREFERENCES_H__
