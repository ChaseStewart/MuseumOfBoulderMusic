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

enum stationType_t 
{
  STATION_TYPE_PERCUSSION = 0,
  STATION_TYPE_MELODIC,
  STATION_TYPE_SWEEP,
  STATION_TYPE_TEST
};

/**
 * Interpret whether to invert the NeoStrip by using value or (127 - value) in the xStation functions that set the neostrip index 
 */
enum neoStripInvertState_t
{
  NEOSTRIP_INVERT_YES     = 0x00,
  NEOSTRIP_INVERT_NO      = 0x01,
  NEOSTRIP_INVERT_DEFAULT = 0xFF  // NOTE that unset EEPROM regions empirically default to 0xFF on Teensy, this will be logically equivalent to a NEOSTRIP_INVERT_NO
};

/**
 * EEPROM layout
 */
#define EEPROM_ADDR_MIN               0x00
#define EEPROM_ADDR_STATION_ID        0x10
#define EEPROM_ADDR_STATION_TYPE      0x11
#define EEPROM_ADDR_MIDI_CHANNEL      0x12
#define EEPROM_ADDR_INVERT_NEOSTRIP_1 0x13
#define EEPROM_ADDR_INVERT_NEOSTRIP_2 0x14
#define EEPROM_ADDR_MAX               0x7F // this is the limit for the TEENSY_LC

void printNonvolConfig(void);

#endif //__NONVOLATILE_H__
