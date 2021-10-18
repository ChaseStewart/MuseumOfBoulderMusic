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

#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location for Teensy

/* Teensy pinout constants */
#define PERCUSSION_STATION_NEO_STRIP 24
#define PERCUSSION_STATION_BUTTON_0 23
#define PERCUSSION_STATION_BUTTON_1 22
#define PERCUSSION_STATION_BUTTON_2 21
#define PERCUSSION_STATION_BUTTON_3 20
#define PERCUSSION_STATION_BUTTON_4 19
#define PERCUSSION_STATION_BUTTON_5 18
#define PERCUSSION_STATION_BUTTON_6 17
#define PERCUSSION_STATION_BUTTON_7 16

#define PERCUSSION_STATION_LED_0 1
#define PERCUSSION_STATION_LED_1 2
#define PERCUSSION_STATION_LED_2 3
#define PERCUSSION_STATION_LED_3 4
#define PERCUSSION_STATION_LED_4 5
#define PERCUSSION_STATION_LED_5 6
#define PERCUSSION_STATION_LED_6 7
#define PERCUSSION_STATION_LED_7 8

#define TEENSY_LED_PIN 13
#define PERCUSSION_STATION_ULTRA_TRIG 12
#define PERCUSSION_STATION_ULTRA_SENS 11
#define PERCUSSION_STATION_PIR_SENS 0


#endif /* __PERCUSSION_STATION_BSP_H__ */
