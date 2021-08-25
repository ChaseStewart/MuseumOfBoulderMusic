/*******************************************************
 *  File: ButtonNote.cpp
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#include "Arduino.h"
#include "ButtonNote.h"
#include "MIDIConstants.h"
#include "Station1BSP.h"

ButtonNote::ButtonNote(int pin, int cc_parameter, buttonNoteId id, bool isToggleNotMomentary)
{
  _id = id;
  _pin = pin;
  array_idx = 0;
  current_reading = LOW;
  midi_needs_update= true;
  update_midi_msec = 0;
  _cc_parameter = cc_parameter;
  _isToggleNotMomentary = isToggleNotMomentary;

  for (int i=0; i<BUTTON_NOTE_ARRAY_LEN; i++)
  {
    button_note_array[i] = 0;
  }

}

/**
 * Capture the readout of the sensor, update the state variables
 */
void ButtonNote::Update(config_t in_config)
{
  current_reading = digitalRead(_pin);
  if (current_reading && midi_needs_update)
  {
    press_time = millis();
  }
  
  button_note_array[array_idx] = current_reading;
  array_idx = (array_idx + 1) % BUTTON_NOTE_ARRAY_LEN;

  if (!_isToggleNotMomentary)
  {
    CheckMIDINeedsUpdate();
    if (midi_needs_update != prev_midi_needs_update)
    {
      usbMIDI.sendControlChange(_cc_parameter, (midi_needs_update) ? 0: 127, in_config.MIDI_Channel);   
    }
    prev_midi_needs_update = midi_needs_update; 
  }  
}

/**
 * Return whether this reading is new 
 */
void ButtonNote::CheckMIDINeedsUpdate(void)
{
  int sum = 0;
  for (int i=0; i < BUTTON_NOTE_ARRAY_LEN; i++)
  {
    sum += button_note_array[i]; 
  }
  midi_needs_update= (!sum);
}

/**
 * Return the current reading
 */
bool ButtonNote::GetReading(void)
{
  return current_reading;
}

void ButtonNote::SendControlCode(config_t in_config)
{
  if (_isToggleNotMomentary)
  {
    toggle_state = 1-toggle_state;
    usbMIDI.sendControlChange(_cc_parameter, (toggle_state) ? 0:127, in_config.MIDI_Channel);   
    update_midi_msec  = millis() + BUTTON_NOTE_DEBOUNCE_DELAY;
    midi_needs_update = false;
  }
  else
  {
    // no-op
  }
  
}

bool ButtonNote::ShouldSendNote(void)
{
    if (_isToggleNotMomentary)
    {
      return (GetReading() && midi_needs_update && 
              millis() > update_midi_msec);       
    }
    else
    {
      return false;
    }
}
