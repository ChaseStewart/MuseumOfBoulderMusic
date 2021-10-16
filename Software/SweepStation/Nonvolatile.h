/******************************************************* 
 *  File: Nonvolatile.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __NONVOLATILE_H__
#define __NONVOLATILE_H__

/**
 * 
 */
enum stationType_t 
{
  STATION_TYPE_PERCUSSION = 0,
  STATION_TYPE_MELODIC,
  STATION_TYPE_SWEEP,
  STATION_TYPE_TEST
};

/**
 * EEPROM layout
 */
#define EEPROM_ADDR_MIN 0x00
#define EEPROM_ADDR_STATION_ID   0x10
#define EEPROM_ADDR_STATION_TYPE 0x11
#define EEPROM_ADDR_MIDI_CHANNEL 0x12
#define EEPROM_ADDR_MAX 0x7F // this is the limit for the TEENSY_LC

/**
 * Print out the state of the nonvolatile bytes
 */
void printNonvolConfig(void);

#endif //__NONVOLATILE_H__
