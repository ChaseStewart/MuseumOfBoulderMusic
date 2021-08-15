/******************************************************* 
 *  File: TeensyBSP.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __TEST_FW_BSP_H__
#define __TEST_FW_BSP_H__

#include "Arduino.h"

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

//#define ULTRASONIC_PING // uncomment if using Parallax Ping ultrasonic Sensor
#define INVERT_ROT_POT // uncomment if the rotary encoder turns the wrong way

/* Teensy layout constants */
#define TEST_FW_JOYSTICK_RIGHT 0
#define TEST_FW_JOYSTICK_LEFT 1
#define TEST_FW_JOYSTICK_UP 2
#define TEST_FW_JOYSTICK_DOWN 3
#define TEST_FW_BUTTON_1 4
#define TEST_FW_BUTTON_2 5

#endif /* __TEST_FW_BSP_H__ */
