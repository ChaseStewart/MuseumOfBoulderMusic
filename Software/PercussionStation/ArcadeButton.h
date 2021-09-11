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
    ArcadeButton(int pin, ArcadeButtonId id);
    void Update(void);
    bool GetReading(void);
    void CheckMIDINeedsUpdate(void);
    void SetMIDIParams(uint8_t new_channel, uint8_t new_cc);

    unsigned long update_midi_msec = 0;
    bool midi_needs_update = true;
    unsigned long press_time;
    
  private:
    ArcadeButtonId _id;
    int _pin; // pin number of Arduino that is connected with SIG pin of Ultrasonic Ranger.
    int arcade_button_array[BUTTON_AVERAGING_ARRAY_LEN] = {0};
    int array_idx = 0;
    int current_reading = LOW;
    int prev_midi_needs_update = false;
    int _cc_parameter;
    int _midi_channel;
};

#endif /* __ARCADE_BUTTON_H__ */
