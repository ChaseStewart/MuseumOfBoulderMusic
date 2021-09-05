/******************************************************* 
 *  File: ConfigEEPROM.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __CONFIG_EEPROM_H__
#define __CONFIG_EEPROM_H__

/* EEPROM */
#define EEPROM_ADDR_MIN 0x00
#define EEPROM_ADDR_STATION_ID   0x10
#define EEPROM_ADDR_MIDI_CHANNEL 0x11 
#define EEPROM_ADDR_MAX 0x7F // this is the limit for the TEENSY_LC


#endif // __CONFIG_EEPROM_H__
