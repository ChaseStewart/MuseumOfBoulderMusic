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

#define ULTRA_MAX_CM 30

#define ULTRA_PING_PERIOD (unsigned long) (3)

#define MAX_PITCH_BEND_DELTA 1700

#define BUTTON_NOTE_DEBOUNCE_DELAY 10

#define JOYSTICK_MSEC_PER_NOTE 300

#define LIN_POT_MIN_READING 20
/**
 * Enumeration of the various 
 */
enum scale_t
{
  MOD_MAJOR,  
  MOD_MINOR,
  MOD_MIXOLYDIAN,
  MOD_DORIAN,
  MOD_CHROMATIC,
  MOD_LIMIT
};

/**
 * Struct that holds configuration for Station1 
 */
typedef struct 
{
  uint8_t root_note;
  scale_t scale;
  uint8_t button1_offset;
  uint8_t button2_offset;
  uint8_t button3_offset;
  uint8_t button4_offset;
  uint8_t octave;
  uint8_t control_code;
  uint8_t MIDI_Channel;
} config_t;

#endif //__PREFERENCES_H__
