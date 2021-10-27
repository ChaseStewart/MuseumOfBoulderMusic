/*******************************************************
 *  File: USBJoystick.cpp
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
#include "Arduino.h"
#include "USBJoystick.h"

/* usbhost classes */
USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);
JoystickController joysticks(myusb);

USBDriver *drivers[] = {&hub1, &joysticks, &hid1};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "joystick[0D]", "HID1"};
bool driver_active[CNT_DEVICES] = {false};

USBHIDInput *hiddrivers[] = {&joysticks};
#define CNT_HIDDEVICES (sizeof(hiddrivers)/sizeof(hiddrivers[0]))
const char * hid_driver_names[CNT_HIDDEVICES] = {"joystick[0H]"};
bool hid_driver_active[CNT_HIDDEVICES] = {false};

int user_axis[64];
int psAxis[64];


uint8_t prev_roll = 0;
uint8_t prev_pitch = 0;


//=============================================================================
// Show when devices are added or removed
//=============================================================================
void PrintDeviceListChanges() 
{
  for (uint8_t i = 0; i < CNT_DEVICES; i++) 
  {
    if (*drivers[i] != driver_active[i]) 
    {
      if (driver_active[i]) 
      {
        Serial.printf("*** Device %s - disconnected ***\r\n", driver_names[i]);
        driver_active[i] = false;
      } 
      else 
      {
        Serial.printf("*** Device %s %x:%x - connected ***\r\n", driver_names[i], drivers[i]->idVendor(), drivers[i]->idProduct());
        driver_active[i] = true;

        const uint8_t *psz = drivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\r\n", psz);
        psz = drivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\r\n", psz);
        psz = drivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\r\n", psz);
      }
    }
  }
  
  for (uint8_t i = 0; i < CNT_HIDDEVICES; i++) 
  {
    if (*hiddrivers[i] != hid_driver_active[i]) 
    {
      if (hid_driver_active[i]) 
      {
        Serial.printf("*** HID Device %s - disconnected ***\r\n", hid_driver_names[i]);
        hid_driver_active[i] = false;
      } 
      else 
      {
        Serial.printf("*** HID Device %s %x:%x - connected ***\r\n", hid_driver_names[i], hiddrivers[i]->idVendor(), hiddrivers[i]->idProduct());
        hid_driver_active[i] = true;

        const uint8_t *psz = hiddrivers[i]->manufacturer();
        if (psz && *psz) Serial.printf("  manufacturer: %s\r\n", psz);
        psz = hiddrivers[i]->product();
        if (psz && *psz) Serial.printf("  product: %s\r\n", psz);
        psz = hiddrivers[i]->serialNumber();
        if (psz && *psz) Serial.printf("  Serial: %s\r\n", psz);
      }
    }
  }
}


void UpdateJoystick(config_t in_config)
{
  if (joysticks.available()) 
  {
    /* Get joystick axis state */
    uint32_t raw_pitch = joysticks.getAxis(JOYSTICK_AXIS_PITCH) / 4;
    uint8_t pitch = (uint8_t) constrain(raw_pitch, 0, 254);
    uint32_t raw_roll = joysticks.getAxis(JOYSTICK_AXIS_ROLL) / 4;
    uint8_t roll = (uint8_t) constrain(raw_roll, 0, 254);
    
    if (roll != prev_roll && (roll < PREFS_JOYSTICK_DEADZONE_LOW || roll > PREFS_JOYSTICK_DEADZONE_HIGH))
    {
      if (roll >= 127)
      {
        usbMIDI.sendControlChange(in_config.roll_pos_cc,  roll - 127,  in_config.MIDI_Channel);        
      }
      else
      {
        usbMIDI.sendControlChange(in_config.roll_neg_cc,  127 - roll,  in_config.MIDI_Channel);
      }
    }
    if (pitch != prev_pitch && (pitch < PREFS_JOYSTICK_DEADZONE_LOW || pitch > PREFS_JOYSTICK_DEADZONE_HIGH))
    {
      if (pitch >= 127)
      {
        usbMIDI.sendControlChange(in_config.pitch_pos_cc, pitch - 127, in_config.MIDI_Channel);
      }
      else
      {
        usbMIDI.sendControlChange(in_config.pitch_neg_cc, 127 - pitch, in_config.MIDI_Channel);
      }
    }
    joysticks.joystickDataClear();

    prev_roll = roll;
    prev_pitch = pitch;
  }
}


bool JoystickIsPressed()
{
  if (joysticks.available()) 
  {    
    /* Get joystick axis state */
    uint32_t raw_pitch = joysticks.getAxis(JOYSTICK_AXIS_PITCH) / 8;
    uint8_t pitch = (uint8_t) constrain(raw_pitch, 0, 127);
    
    uint32_t raw_roll = joysticks.getAxis(JOYSTICK_AXIS_ROLL) / 8;
    uint8_t roll = (uint8_t) constrain(raw_roll, 0, 127);
        
    return (pitch != prev_pitch | roll != prev_roll);
  }
  return false; // if joystick not available
}
