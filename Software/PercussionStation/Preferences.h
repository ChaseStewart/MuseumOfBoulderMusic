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

#include "MIDIConstants.h"

#define DEBUG

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
