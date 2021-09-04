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

/**
 * Enumeration of the various supported modes
 */
enum midi_mode_t
{
  MODE_MAJOR,  
  MODE_MINOR,
  MODE_MIXOLYDIAN,
  MODE_DORIAN,
  MODE_CHROMATIC,
  MODE_LIMIT
};

/**
 * Struct that holds configuration for Station1 
 */
typedef struct 
{
  uint8_t root_note;
  midi_mode_t  mode;
  uint8_t button1_offset;
  uint8_t button2_offset;
  uint8_t button3_offset;
  uint8_t button4_offset;
  uint8_t octave;
  uint8_t control_code;
  uint8_t MIDI_Channel;
} config_t;

#endif //__PREFERENCES_H__
