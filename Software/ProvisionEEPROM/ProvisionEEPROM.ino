/******************************************************* 
 *  File: ProvisionEEPROM.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#include <EEPROM.h>

#include "ConfigEEPROM.h"

#define DEBUG_PRINTLN(x)  Serial.println(x)
#define DEBUG_PRINT(x)  Serial.print(x)
#define DEBUG_PRINTLN_HEX(x) Serial.println (x, HEX)
#define DEBUG_PRINT_HEX(x) Serial.print (x, HEX)
#define DEBUG_PRINT_DEC4(x) Serial.print((float) x, 4)
#define DEBUG_PRINT_DEC3(x) Serial.print((float )x, 3)
#define TEENSY_LED_PIN 13

/**
 * START EEPROM preferences to be written
 */
#define THIS_STATION_ID 0x1
#define THIS_MIDI_CHANNEL 0x1
/**
 * END EEPROM preferences to be written
 */

/* Static function prototypes */

/**
 * 
 */
static void EEPROMCheckAndSet(uint8_t address, uint8_t value);

/**
 * Just print a welcome banner
 */
static void printBanner(void); // Print a serial welcome banner

/**
 * Add additional EEPROM config values here
 */
static void updateEEPROM(uint8_t stationID, uint8_t midiChannel);

/**
 * print out all the values from the EEPROM
 */
static void printEEPROM(void);


void setup() 
{
  pinMode(TEENSY_LED_PIN, OUTPUT);
  digitalWrite(TEENSY_LED_PIN, LOW);  

  Serial.begin(9600);
  while(!Serial);
  
  printBanner();
  DEBUG_PRINTLN("* Updating EEPROM *");
  updateEEPROM(THIS_STATION_ID, THIS_MIDI_CHANNEL);
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("* Printing EEPROM *");
  printEEPROM();
  DEBUG_PRINTLN("* DONE *");
}

void loop() 
{
  digitalWrite(TEENSY_LED_PIN, HIGH);  
  delay(1000);
  
}

/**
 * Check and set
 */
static void updateEEPROM(uint8_t stationID, uint8_t midiChannel)
{
  EEPROMCheckAndSet(EEPROM_ADDR_STATION_ID, stationID);
  EEPROMCheckAndSet(EEPROM_ADDR_MIDI_CHANNEL, midiChannel);  
}

static void printEEPROM(void)
{
  uint8_t value;
  for (int i=0; i <= EEPROM_ADDR_MAX; i += 0x10 )
  {
    DEBUG_PRINT("0x");
    DEBUG_PRINT_HEX(i);
    if (i<0x10)
    {
      DEBUG_PRINT("0");
    }
    DEBUG_PRINT(": ");
    for (int j=0; j < 0x10; j++)
    {
      value = EEPROM.read(i+j);
      DEBUG_PRINT(" ");
      if (value < 0x10)
      {
        DEBUG_PRINT("0");
      }
      DEBUG_PRINT_HEX(value);      
    }
    DEBUG_PRINTLN("");
  }
  DEBUG_PRINTLN("");
}

/**
 * Read EEPROM at `address` and set it to `value` if it is not already
 */
static void EEPROMCheckAndSet(uint8_t address, uint8_t value)
{
  if (EEPROM.read(address) != value)
  {
    EEPROM.write(address, value);
    DEBUG_PRINT("Updated address 0x");
    DEBUG_PRINT_HEX(address);
    DEBUG_PRINT(" = ");
    DEBUG_PRINTLN(value);   
  }
}

/**
 * Just print a quick serial banner- this is to de-clutter setup()
 */
void printBanner(void)
{
  DEBUG_PRINTLN();
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("**************************************************");
  DEBUG_PRINTLN("*             Provision_EEPROM                   *");
  DEBUG_PRINTLN("*                                                *");
  DEBUG_PRINTLN("* By Chase E. Stewart for Hidden Layer Design    *");
#if !defined(CORE_TEENSY)
  DEBUG_PRINTLN("* Running on non-Teensy!                         *");
#elif !defined(__arm__)
  DEBUG_PRINTLN("* Running on a non-arm Teensy like the 2.0/2.1++ *");
#elif defined(__MKL26Z64__)
  DEBUG_PRINTLN("* Running on a Teensy LC                         *");
#elif defined(__MK20DX256__)
  DEBUG_PRINTLN("* Running on a Teensy 3.1                        *");
#elif defined(__MK20DX128__)
  DEBUG_PRINTLN("* Running on a Teensy 3.0                        *");
#else
  DEBUG_PRINTLN("* Running on an unknown Teensy processor         *");
#endif

  DEBUG_PRINTLN("**************************************************");
  DEBUG_PRINTLN();

}
