/******************************************************* 
 *  File: ArcadeButton.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#ifndef __ARCADE_BUTTON_H__
#define __ARCADE_BUTTON_H__

#include "Preferences.h"
#include "Nonvolatile.h"

/* Definitions */
#define BUTTON_AVERAGING_ARRAY_LEN 10 // averaging array

enum ArcadeButtonId 
{
  BUTTON_0 = 0,
  BUTTON_1 = 1,
  BUTTON_2 = 2,
  BUTTON_3 = 3,
  BUTTON_4 = 4,
  BUTTON_5 = 5,
  BUTTON_LIMIT = 5
};

class ArcadeButton
{
  public:
    ArcadeButton(int sensPin, int ledPin, ArcadeButtonId id);
    void Update(void);
    bool GetReading(void);
    void CheckMIDINeedsUpdate(void);
    void SetMIDIParams(uint8_t new_channel, uint8_t new_cc);
    void SetLowValue(uint8_t new_low_value);

    unsigned long press_time;
    unsigned long update_midi_msec = 0;
    bool midi_needs_update = true;
    
  private:
    ArcadeButtonId _id;
    int _pin; // pin number of Arduino that is connected with SIG pin of Ultrasonic Ranger.
    int _led;
    int arcade_button_array[BUTTON_AVERAGING_ARRAY_LEN] = {0};
    int array_idx = 0;
    int current_reading = LOW;
    bool prev_midi_needs_update = false;
    uint8_t _cc_parameter;
    uint8_t _midi_channel;
    uint8_t _low_value;
    uint8_t _high_value;
};

#endif /* __ARCADE_BUTTON_H__ */
