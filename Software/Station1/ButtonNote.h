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

#define BUTTON_NOTE_ARRAY_LEN 2

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
    ButtonNote(int pin, buttonNoteId id);
    void Update(void);
    bool GetReading(void);
    void SendNote(int int_note, int analog_volume, config_t in_config);
    void CheckMIDINeedsUpdate(void);
    bool ShouldSendNote(int curr_note_ofst, int curr_volume);

    int previous_note;
    int current_note;
    unsigned long update_midi_msec;
    bool midi_needs_update;
    unsigned long press_time;
    
  private:
    buttonNoteId _id;
    int _pin; // pin number of Arduino that is connected with SIG pin of Ultrasonic Ranger.
    int button_note_array[BUTTON_NOTE_ARRAY_LEN];
    int array_idx;
    int current_reading;
    int prev_note_ofst;
    int prev_volume;
};

#endif /* __BUTTON_NOTE_H__ */
