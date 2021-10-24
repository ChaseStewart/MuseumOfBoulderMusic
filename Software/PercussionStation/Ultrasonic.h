/******************************************************* 
 *  File: Ultrasonic.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#ifndef __ULTRASONIC_H__
#define __ULTRASONIC_H__

#include "Preferences.h" // for PREFS_ULTRA_MAX_CM


/**
 * MIDI Pitch bend message accepts a 14-bit twos compliment value, 
 * but we only want to detune like a whammy bar, so we use 12 bits and subtract 2^12 
 * such that max val is 0 and whammy goes to half of the full negative (detune) range
 * 
 * NOTE: use pow(2,13) instead for full range!
 */
#define ULTRA_SCALED_VALUE(x) (int) (pow(2,12) * x  / PREFS_ULTRA_MAX_CM) - pow(2,12)  // Do not adjust!

/**
 * If using pitch bend for another MIDI control code, the range is only 0-127
 */
#define ULTRA_ONEBYTE_VALUE(x) (int) (pow(2,7) * (PREFS_ULTRA_MAX_CM - x)  / PREFS_ULTRA_MAX_CM)  // Do not adjust!


#endif // __ULTRASONIC_H__
