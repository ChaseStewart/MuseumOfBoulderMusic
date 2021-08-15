/******************************************************* 
 *  File: TestFirmware.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *  NOTE: Teensyduino and USB Type: "Serial + MIDI" are REQUIRED for this firmware
 *  
 *******************************************************/
#include "TestFirmwareBSP.h"
#include "MIDI.h"
#include "MIDIConstants.h"

void setup() {
  pinMode(TEST_FW_JOYSTICK_RIGHT, INPUT_PULLUP);
  pinMode(TEST_FW_JOYSTICK_LEFT, INPUT_PULLUP);
  pinMode(TEST_FW_JOYSTICK_UP, INPUT_PULLUP);
  pinMode(TEST_FW_JOYSTICK_DOWN, INPUT_PULLUP);
  pinMode(TEST_FW_BUTTON_1, INPUT_PULLUP);
  pinMode(TEST_FW_BUTTON_2, INPUT_PULLUP);

#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG
  
}

void loop() 
{
  if (!digitalRead(TEST_FW_JOYSTICK_RIGHT))
    DEBUG_PRINT("Right ") 

  if (!digitalRead(TEST_FW_JOYSTICK_LEFT))
    DEBUG_PRINT("Left ");  
    
  if (!digitalRead(TEST_FW_JOYSTICK_UP))
    DEBUG_PRINT("Up ");  
  
  if (!digitalRead(TEST_FW_JOYSTICK_DOWN))
    DEBUG_PRINT("Down ");  
    
  if (digitalRead(TEST_FW_BUTTON_1))
    DEBUG_PRINT("Button1 ");  

  if (digitalRead(TEST_FW_BUTTON_2))
    DEBUG_PRINT("Button2 ");  
  
  DEBUG_PRINTLN();
  delay(1000);  

  /* TODO parse inputs to generate MIDI messages */
  //usbMIDI.send_now();

  /* Ignore all MIDI input */
  while (usbMIDI.read()); 
}
