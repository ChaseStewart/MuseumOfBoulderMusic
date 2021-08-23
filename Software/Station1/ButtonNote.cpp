/******************************************************* Pr
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

/**
 * Static function to get note in MIDI scale from 
 */
static int getScaledNote(int ofst, buttonNoteId id, config_t in_config)
{
  int delta;
  
  switch(id)
  {
    case BUTTON_0:
      delta = ofst;
      break;
      
    case BUTTON_1:
      delta = ofst + in_config.button1_offset;
      break;
    
    case BUTTON_2:
      delta = ofst + in_config.button2_offset;
      break;

    case BUTTON_3:
      delta = ofst + in_config.button3_offset;
      break;
      
    default:
      digitalWrite(TEENSY_LED_PIN, HIGH);
      return in_config.root_note; 
  }
  
  if (in_config.scale == MOD_MAJOR)
  {
    return in_config.octave + in_config.root_note + MAJOR_DELTAS[delta];
  }
  else if (in_config.scale == MOD_MINOR)
  {
    return in_config.octave + in_config.root_note + MINOR_DELTAS[delta];
  }
  else if (in_config.scale == MOD_MIXOLYDIAN)
  {
    return in_config.octave + in_config.root_note + MIXOLYDIAN_DELTAS[delta];
  }
  else if (in_config.scale == MOD_DORIAN)
  {
    return in_config.octave + in_config.root_note + DORIAN_DELTAS[delta];
  }
  else if (in_config.scale == MOD_CHROMATIC)
  {
    return in_config.octave + in_config.root_note + CHROMATIC_DELTAS[delta];
  }
  else 
  {  
    return in_config.octave + in_config.root_note;
  }
}

ButtonNote::ButtonNote(int pin, buttonNoteId id)
{
  _id = id;
  _pin = pin;
  array_idx = 0;
  current_reading = LOW;
  midi_needs_update= true;
  update_midi_msec = 0;

  for (int i=0; i<BUTTON_NOTE_ARRAY_LEN; i++)
  {
    button_note_array[i] = 0;
  }

}

/**
 * Capture the readout of the sensor, update the state variables
 */
void ButtonNote::Update(void)
{
  current_reading = digitalRead(_pin);
  if (current_reading && midi_needs_update)
  {
    press_time = millis();
  }
  
  button_note_array[array_idx] = current_reading;
  array_idx = (array_idx + 1) % BUTTON_NOTE_ARRAY_LEN;
}

/**
 * Return the current reading
 */
bool ButtonNote::GetReading(void)
{
  return current_reading;
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

void ButtonNote::SendNote(int ofst, int analog_volume, config_t in_config)
{
  current_note = getScaledNote(ofst, _id, in_config);
  usbMIDI.sendNoteOff(previous_note, 0, in_config.MIDI_Channel);   
  usbMIDI.sendNoteOn(current_note, 100, in_config.MIDI_Channel);
  update_midi_msec  = millis() + BUTTON_NOTE_DEBOUNCE_DELAY;
  midi_needs_update = false;
  previous_note = current_note;  
}

bool ButtonNote::ShouldSendNote(void)
{
  return (GetReading() && 
          midi_needs_update && 
          millis() > update_midi_msec);
}
