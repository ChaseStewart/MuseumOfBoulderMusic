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

#include "Preferences.h"
#include "USBHost_t36.h"


#define JOYSTICK_AXIS_ROLL  0
#define JOYSTICK_AXIS_PITCH 1

/* Class vars */
extern USBHost myusb;
extern USBHub hub1;
extern USBHIDParser hid1;
extern JoystickController joysticks;

extern USBDriver *drivers[];
extern bool driver_active[];

extern USBHIDInput *hiddrivers[];
extern const char * hid_driver_names[];
extern bool hid_driver_active[];

extern uint64_t joystick_full_notify_mask;
extern int psAxis[64];
extern int user_axis[64];


void PrintDeviceListChanges(void); // Helper function from Paul Stoffregen to print added or removed devices 
void UpdateJoystick(config_t in_config);
bool JoystickIsPressed(void);

#endif // __USB_JOYSTICK_H__
