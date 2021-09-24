/*******************************************************
 *  File: Nonvolatile.cpp
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#include <EEPROM.h>

#include "Nonvolatile.h"
#include "Preferences.h"


static const char *StationTypeToStr(stationType_t stationType)
{
  switch (stationType)
  {
    case STATION_TYPE_PERCUSSION:
      return "Percussion";      
    case STATION_TYPE_MELODIC:
      return "Melodic";
    case STATION_TYPE_TEST:
      return "Test";
    default:
      return "Unknown";
  }
}


void printNonvolConfig(void)
{
  DEBUG_PRINTLN("*** NONVOL CONFIG STATS ***");
  Serial.printf("  Device ID: %02d\r\n", EEPROM.read(EEPROM_ADDR_STATION_ID));
  Serial.printf("  Station Type: %s\r\n", StationTypeToStr(EEPROM.read(EEPROM_ADDR_STATION_TYPE)));
  Serial.printf("  MIDI Channel: 0x%02x\r\n", EEPROM.read(EEPROM_ADDR_MIDI_CHANNEL));
  DEBUG_PRINTLN("*** DONE ***");
  DEBUG_PRINTLN();
}

 
