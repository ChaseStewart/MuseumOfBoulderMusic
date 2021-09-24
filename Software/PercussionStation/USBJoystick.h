/******************************************************* 
 *  File: USBJoystick.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  NOTE: PrintDeviceListChanges and general starting point
 *  from Paul Stoffregen's usbHost_t36 library and examples
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __USB_JOYSTICK_H__
#define __USB_JOYSTICK_H__

#include "USBHost_t36.h"


#define JOYSTICK_AXIS_ROLL  0
#define JOYSTICK_AXIS_PITCH 1
#define JOYSTICK_AXIS_YAW   5

/* Class vars */
extern USBHost myusb;
extern USBHub hub1;
extern USBHIDParser hid1;
extern JoystickController joysticks;

extern int user_axis[64];

extern USBDriver *drivers[];
extern bool driver_active[];

extern USBHIDInput *hiddrivers[];
extern const char * hid_driver_names[];
extern bool hid_driver_active[];

extern uint64_t joystick_full_notify_mask;

extern int psAxis[64];

const uint32_t BUTTON_MASK_TRIGGER = 0x1;
const uint32_t BUTTON_MASK_THUMB   = 0x2;


void PrintDeviceListChanges(void); // Helper function from Paul Stoffregen to print added or removed devices 

#endif // __USB_JOYSTICK_H__
