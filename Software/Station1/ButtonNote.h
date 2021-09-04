/******************************************************* 
 *  File: ButtonNote.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#ifndef __BUTTON_NOTE_H__
#define __BUTTON_NOTE_H__

#include "Preferences.h"
#include "Nonvolatile.h"

/* Definitions */
#define BUTTON_AVERAGING_ARRAY_LEN 10 // averaging array
#define BUTTON_MODE_TOGGLE true // see bool isToggleNotMomentary in constructor
#define BUTTON_MODE_MOMENTARY false // see bool isToggleNotMomentary in constructor

enum buttonNoteId 
{
  BUTTON_0 = 0,
  BUTTON_1 = 1,
  BUTTON_2 = 2,
  BUTTON_3 = 3,
  BUTTON_4 = 4,
  BUTTON_LIMIT = 4
};

class ButtonNote
{
  public:
    ButtonNote(int pin, int cc_parameter, buttonNoteId id, bool isToggleNotMomentary);
    void Update(config_t in_config);
    bool GetReading(void);
    void SendControlCode(config_t in_config);
    void CheckMIDINeedsUpdate(void);
    bool ShouldSendNote(void);

    unsigned long update_midi_msec;
    bool midi_needs_update;
    unsigned long press_time;
    
  private:
    buttonNoteId _id;
    bool toggle_state;
    bool _isToggleNotMomentary;
    int _pin; // pin number of Arduino that is connected with SIG pin of Ultrasonic Ranger.
    int button_note_array[BUTTON_AVERAGING_ARRAY_LEN];
    int array_idx;
    int current_reading;
    int prev_midi_needs_update;
    int _cc_parameter;
};

#endif /* __BUTTON_NOTE_H__ */
