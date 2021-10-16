/******************************************************* 
 *  File: PercussionStationBSP.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __SWEEP_STATION_BSP_H__
#define __SWEEP_STATION_BSP_H__

#include "Arduino.h"
#include "Preferences.h"

/* Just a documentation macro for analog vs digital pins */
#define ANALOG_READ(x) (x)

#define SCB_AIRCR (*(volatile uint32_t *)0xE000ED0C) // Application Interrupt and Reset Control location for Teensy

/* Teensy pinout constants */
#define SWEEP_STATION_NEO_STRIP_LEFT 29
#define SWEEP_STATION_NEO_STRIP_RIGHT 24
#define SWEEP_STATION_BUTTON_0 23
#define SWEEP_STATION_BUTTON_1 22
#define SWEEP_STATION_LED_0 1
#define SWEEP_STATION_LED_1 2
#define TEENSY_LED_PIN 13
#define SWEEP_STATION_LEFT_ULTRA_TRIG 12
#define SWEEP_STATION_LEFT_ULTRA_SENS 11
#define SWEEP_STATION_RIGHT_ULTRA_TRIG 10
#define SWEEP_STATION_RIGHT_ULTRA_SENS 9
#define SWEEP_STATION_PIR_SENS 0


#endif /* __SWEEP_STATION_BSP_H__ */
