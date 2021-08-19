/******************************************************* 
 *  File: TestFirmware.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *  NOTE: Teensyduino and USB Type: "Serial + MIDI" are REQUIRED for this firmware
 *  Tested with Teensy-LC 
 *******************************************************/
#include <NewPing.h>

#include "Preferences.h"
#include "TestFirmwareBSP.h"
#include "MIDIConstants.h"


/* Globals */
unsigned long ping_time;
unsigned long range_in_us;
unsigned long range_in_cm;
NewPing ultrasonic(TEST_FW_ULTRA_TRIG, TEST_FW_ULTRA_SENS, ULTRA_MAX_CM);

/* Prototypes */
static void pingCheck(void);

/**
 * Setup pinouts and serial 
 */
void setup() {
  pinMode(TEST_FW_JOYSTICK_RIGHT, INPUT_PULLUP);
  pinMode(TEST_FW_JOYSTICK_LEFT, INPUT_PULLUP);
  pinMode(TEST_FW_JOYSTICK_UP, INPUT_PULLUP);
  pinMode(TEST_FW_JOYSTICK_DOWN, INPUT_PULLUP);
  pinMode(TEST_FW_BUTTON_1, INPUT_PULLUP);
  pinMode(TEST_FW_BUTTON_2, INPUT_PULLUP);
  pinMode(TEST_FW_BUTTON_3, INPUT_PULLUP);
  pinMode(TEST_FW_LIN_POT, INPUT);

#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG  
}


/**
 * Just print all sensor outputs to serial to ensure proper functionality
 */
void loop() 
{
  if (!digitalRead(TEST_FW_JOYSTICK_RIGHT)) 
    DEBUG_PRINT("Right ");
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
  if (!digitalRead(TEST_FW_BUTTON_3))
    DEBUG_PRINT("Button3 ");

  DEBUG_PRINT("LinPot Reading: ");
  DEBUG_PRINT(analogRead(TEST_FW_LIN_POT));

  /* Get Ultrasonic Distance sensor reading */
  if (micros() >= ping_time)
  {
    /* NOTE: due to using newPing timer, this has to indirectly set range_in_us */
    ultrasonic.ping_timer(pingCheck);
    range_in_cm = range_in_us / US_ROUNDTRIP_CM;
    ping_time += ULTRA_PING_PERIOD;
  }

  DEBUG_PRINT("Ultrasonic Reading: ");
  DEBUG_PRINT(range_in_cm); 
  
  DEBUG_PRINTLN();
  delay(1000);  

  /* TODO parse inputs to generate MIDI messages */
  //usbMIDI.send_now();

  /* Ignore all MIDI input */
  while (usbMIDI.read()); 
}

/**
 * Callback function to check whether ultrasonic sonar has returned data
 */
static void pingCheck(void)
{
  range_in_us = (ultrasonic.check_timer()) ? ultrasonic.ping_result : range_in_us +2;
}
