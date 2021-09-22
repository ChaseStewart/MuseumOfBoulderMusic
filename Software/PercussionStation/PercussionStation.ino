

/******************************************************* 
 *  File: PercussionStation.ino
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *  NOTE: Teensyduino and USB Type: "Serial + MIDI" are REQUIRED for this firmware
 *  Tested with Teensy-LC 
 *******************************************************/
#include <EEPROM.h>
#include <NewPing.h>
#include <WS2812Serial.h>

#include "Preferences.h"
#include "PercussionStationBSP.h"
#include "MIDIConstants.h"
#include "Nonvolatile.h"
#include "Ultrasonic.h"
#include "ArcadeButton.h"

#include "USBHost_t36.h"

USBHost myusb;
USBHub hub1(myusb);
USBHIDParser hid1(myusb);

#define COUNT_JOYSTICKS 1
JoystickController joysticks(myusb);
int user_axis[64];
uint32_t buttons_prev = 0;


USBDriver *drivers[] = {&hub1, &joysticks, &hid1};
#define CNT_DEVICES (sizeof(drivers)/sizeof(drivers[0]))
const char * driver_names[CNT_DEVICES] = {"Hub1", "joystick[0D]", "HID1"};
bool driver_active[CNT_DEVICES] = {false};

// Lets also look at HID Input devices
USBHIDInput *hiddrivers[] = {&joysticks};
#define CNT_HIDDEVICES (sizeof(hiddrivers)/sizeof(hiddrivers[0]))
const char * hid_driver_names[CNT_HIDDEVICES] = {"joystick[0H]"};
bool hid_driver_active[CNT_HIDDEVICES] = {false};

uint8_t joystick_left_trigger_value[COUNT_JOYSTICKS] = {0};
uint8_t joystick_right_trigger_value[COUNT_JOYSTICKS] = {0};
uint64_t joystick_full_notify_mask = (uint64_t) - 1;

bool show_changed_only = true;


int psAxis[64];



/* Globals */
unsigned long ping_time;
unsigned long range_in_us;
unsigned long range_in_cm;
bool pir_state = LOW;
bool prev_pir_state = HIGH;
int curr_bend_val = 1;
int prev_bend_val = 0;
config_t in_config = {0};

const int NeoStick_count = 8;
byte NeoStick_drawingMemory[NeoStick_count*3];         //  3 bytes per LED
DMAMEM byte NeoStick_displayMemory[NeoStick_count*12]; // 12 bytes per LED

const int NeoButtons_count = 1;
byte NeoButtons_drawingMemory[NeoButtons_count*3];         //  3 bytes per LED
DMAMEM byte NeoButtons_displayMemory[NeoButtons_count*12]; // 12 bytes per LED

/* Static Prototypes */
static void initPins(void); // Just init the pins as input/output/input_pullup
static void pingCheck(void); // Ultrasonic callback function
static void printBanner(void); // Print a serial welcome banner
static void myControlChange(byte channel, byte control, byte value); // callback handler for reading a ControlChange from Max/MSP
static void updateNeoPixelStick(uint8_t value); // 
static void PrintDeviceListChanges(void);


/* Global class instances */
ArcadeButton ArcadeButton0(PERCUSSION_STATION_BUTTON_0, PERCUSSION_STATION_LED_0, BUTTON_0);                      
ArcadeButton ArcadeButton1(PERCUSSION_STATION_BUTTON_1, PERCUSSION_STATION_LED_1, BUTTON_1);                    
ArcadeButton ArcadeButton2(PERCUSSION_STATION_BUTTON_2, PERCUSSION_STATION_LED_2, BUTTON_2);
ArcadeButton ArcadeButton3(PERCUSSION_STATION_BUTTON_3, PERCUSSION_STATION_LED_3, BUTTON_3);
ArcadeButton ArcadeButton4(PERCUSSION_STATION_BUTTON_4, PERCUSSION_STATION_LED_4, BUTTON_4);
ArcadeButton ArcadeButton5(PERCUSSION_STATION_BUTTON_5, PERCUSSION_STATION_LED_5, BUTTON_5); 

NewPing ultrasonic(PERCUSSION_STATION_ULTRA_TRIG, // Trigger pin
                   PERCUSSION_STATION_ULTRA_SENS, // Sense pin
                   PREFS_ULTRA_MAX_CM); // Max distance limit

WS2812Serial NeoStick(NeoStick_count, NeoStick_displayMemory, NeoStick_drawingMemory, PERCUSSION_STATION_NEO_STRIP, WS2812_GRB);
WS2812Serial NeoButtons(NeoButtons_count, NeoButtons_displayMemory, NeoButtons_drawingMemory, PERCUSSION_STATION_NEO_BUTTONS, WS2812_GRB);


/**
 * Setup pinouts and serial 
 */
void setup() 
{
  initPins();
  digitalWrite(TEENSY_LED_PIN, LOW);
  NeoStick.begin();
  NeoButtons.begin();
  myusb.begin();

  // Arduino does not seem to support designated initializers
  in_config.button0_cc = MIDI_GEN_PURPOSE_1;
  in_config.button1_cc = MIDI_GEN_PURPOSE_2;
  in_config.button2_cc = MIDI_GEN_PURPOSE_3;
  in_config.button3_cc = MIDI_GEN_PURPOSE_4;
  in_config.button4_cc = MIDI_GEN_PURPOSE_5;
  in_config.button5_cc = MIDI_GEN_PURPOSE_6;
  in_config.pbend_cc   = MIDI_GEN_PURPOSE_7;
  in_config.MIDI_Channel = EEPROM.read(EEPROM_ADDR_MIDI_CHANNEL);

  ArcadeButton0.SetMIDIParams(in_config.MIDI_Channel, in_config.button0_cc);
  ArcadeButton1.SetMIDIParams(in_config.MIDI_Channel, in_config.button1_cc);
  ArcadeButton2.SetMIDIParams(in_config.MIDI_Channel, in_config.button2_cc);
  ArcadeButton3.SetMIDIParams(in_config.MIDI_Channel, in_config.button3_cc);
  ArcadeButton4.SetMIDIParams(in_config.MIDI_Channel, in_config.button4_cc);
  ArcadeButton5.SetMIDIParams(in_config.MIDI_Channel, in_config.button5_cc);
  
#ifdef DEBUG
  Serial.begin(9600);
#endif // DEBUG 

  /* We will read speciific MIDI CC messages to set the countdown light */
  usbMIDI.setHandleControlChange(myControlChange);
  printBanner();
  printNonvolConfig();

  for (int i = 0; i < NeoButtons_count; i++)
  {
    NeoButtons.setPixel(i, 0, 0, 0);
  }

  for (int j = 0; j < NeoStick_count; j++)
  {
    NeoStick.setPixel(j, 0, 0, 0);
  }
  
  NeoButtons.show();
  NeoStick.show();
}

/**
 * Just print all sensor outputs to serial to ensure proper functionality
 */
void loop() 
{
  myusb.Task();
  PrintDeviceListChanges();
  
  /* Get Ultrasonic Distance sensor reading */
  if (micros() >= ping_time)
  {
    /* NOTE: due to using newPing timer, this has to indirectly set range_in_us */
    ultrasonic.ping_timer(pingCheck);
    range_in_cm = range_in_us / US_ROUNDTRIP_CM;
    ping_time += PREFS_ULTRA_PING_PERIOD;
  }

  /* constrain range_in_cm, but sufficiently low values are treated as high ones */
  if ((range_in_cm < P_BEND_MIN_CM) || (range_in_cm > P_BEND_MAX_CM))
  {
    range_in_cm = P_BEND_MAX_CM;
  }

  /* convert ultrasonic range to value for MIDI CC and send it */
  curr_bend_val = P_BEND_ONEBYTE_VALUE(range_in_cm);
  if ((curr_bend_val != prev_bend_val) && (abs(curr_bend_val- prev_bend_val) < PREFS_P_BEND_ONEBYTE_MAX_DELTA))
  {
    usbMIDI.sendControlChange(in_config.pbend_cc, curr_bend_val, in_config.MIDI_Channel);
  }
  prev_bend_val = curr_bend_val;

  /* Read buttons and update averaging arrays */
  ArcadeButton0.Update();
  ArcadeButton1.Update();
  ArcadeButton2.Update();
  ArcadeButton3.Update();
  ArcadeButton4.Update();
  ArcadeButton5.Update();
      
  /* Handle PIR sensor */
  pir_state = digitalRead(PERCUSSION_STATION_PIR_SENS);

  if (pir_state != prev_pir_state)
  {
    digitalWrite(TEENSY_LED_PIN, pir_state);
  }
  prev_pir_state = pir_state;


  /* Flush any queued messages */
  usbMIDI.send_now();

  /* This will trigger */
  while (usbMIDI.read(PREFS_MIDI_INPUT_CHANNEL))
  {
    // no-op, handled by callback(s)  
  }
}


/**
 * Callback function to check whether ultrasonic sonar has returned data-
 * Provides a "spring constant" to the rangefinder reading, pushing it back to a detune of zero
 * when the user's hand is away from the rangefinder beam
 */
static void pingCheck(void)
{
  range_in_us = (ultrasonic.check_timer()) ? ultrasonic.ping_result : range_in_us +2;
}


/**
 * Just init the pins for this project- this is to de-clutter setup()
 */
static void initPins(void)
{
  pinMode(PERCUSSION_STATION_BUTTON_0, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_1, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_2, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_3, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_4, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_BUTTON_5, INPUT_PULLUP);
  pinMode(PERCUSSION_STATION_PIR_SENS, INPUT);
  pinMode(PERCUSSION_STATION_LED_0, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_1, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_2, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_3, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_4, OUTPUT);
  pinMode(PERCUSSION_STATION_LED_5, OUTPUT);

  pinMode(TEENSY_LED_PIN, OUTPUT);
}

/**
 * Just print a quick serial banner- this is to de-clutter setup()
 */
void printBanner(void)
{
  DEBUG_PRINTLN();
  DEBUG_PRINTLN();
  DEBUG_PRINTLN("**************************************************");
  DEBUG_PRINTLN("*             Percussion Station Firmware        *");
  DEBUG_PRINTLN("*                                                *");
  DEBUG_PRINTLN("* By Chase E. Stewart for Hidden Layer Design    *");
  DEBUG_PRINTLN("*                                                *");

#if !defined(CORE_TEENSY)
  DEBUG_PRINTLN("* Running on non-Teensy!                         *");
#elif defined(__IMXRT1062__)
  DEBUG_PRINTLN("* Running on a Teensy 4.x                        *");
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


/**
 * Callback function for receiving MIDI control change back from Max/MSP
 */
static void myControlChange(byte channel, byte control, byte value)
{
  if ((channel != PREFS_MIDI_INPUT_CHANNEL) || (control != PREFS_MIDI_INPUT_CC))
  {
    return;
  }
  updateNeoPixelStick(value);
}


/**
 * Use MIDI CC value 0-127 to set neopixel indicator brightness
 */
static void updateNeoPixelStick(uint8_t value)
{
  (void) value;
  // TODO
  return;  
}



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
