/******************************************************* 
 *  File: PercussionStationBSP.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __PERCUSSION_STATION_BSP_H__
#define __PERCUSSION_STATION_BSP_H__

#include "Arduino.h"
#include "Preferences.h"

/* Just a documentation macro for analog vs digital pins */
#define ANALOG_READ(x) (x)

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

#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location for Teensy

/* Teensy pinout constants */
#define PERCUSSION_STATION_BUTTON_0 23
#define PERCUSSION_STATION_BUTTON_1 22
#define PERCUSSION_STATION_BUTTON_2 21
#define PERCUSSION_STATION_BUTTON_3 20
#define PERCUSSION_STATION_BUTTON_4 19
#define PERCUSSION_STATION_BUTTON_5 18
#define TEENSY_LED_PIN 13
#define PERCUSSION_STATION_ULTRA_TRIG 12
#define PERCUSSION_STATION_ULTRA_SENS 11
#define PERCUSSION_STATION_PIR_SENS 0


#endif /* __PERCUSSION_STATION_BSP_H__ */
