/*******************************************************
 *  File: ArcadeButton.cpp
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#include "Arduino.h"
#include "ArcadeButton.h"
#include "MIDIConstants.h"
#include "PercussionStationBSP.h"

ArcadeButton::ArcadeButton(int sensPin, int ledPin, ArcadeButtonId id)
{
  _id = id;
  _pin = sensPin;
  _led = ledPin;
  _cc_parameter = MIDI_GEN_PURPOSE_1; // DEFAULT VALUE
  _midi_channel = MIDI_CHANNEL_1; // DEFAULT_VALUE
  _low_value = PREFS_ARCADE_BUTTON_PWM_LOW_ABSENCE;
  _high_value = PREFS_BUTTON_CC_HIGH_VAL;
}

/**
   Capture the readout of the sensor, update the state variables
*/
void ArcadeButton::Update(void)
{
  current_reading = digitalRead(_pin);

  if (current_reading && midi_needs_update)
  {
    press_time = millis();
  }

  arcade_button_array[array_idx] = current_reading;
  array_idx = (array_idx + 1) % BUTTON_AVERAGING_ARRAY_LEN;

  CheckMIDINeedsUpdate();
  if (midi_needs_update != prev_midi_needs_update)
  {
    analogWrite(_led, (midi_needs_update) ? _high_value : _low_value);
    usbMIDI.sendControlChange(_cc_parameter, (midi_needs_update) ? PREFS_BUTTON_CC_HIGH_VAL : PREFS_BUTTON_CC_LOW_VAL, _midi_channel);
  }

  prev_midi_needs_update = midi_needs_update;
}

/**
   Set the ControlCode for this ArcadeButton
*/
void ArcadeButton::SetMIDIParams(uint8_t new_channel, uint8_t new_cc)
{
  _cc_parameter = new_cc;
  _midi_channel = new_channel;
}

void ArcadeButton::SetLowValue(uint8_t new_low_value)
{
  _low_value = new_low_value;
  analogWrite(_led, (midi_needs_update) ? _high_value : _low_value);
}


/**
   Return whether this reading is new
*/
void ArcadeButton::CheckMIDINeedsUpdate(void)
{
  int sum = 0;

  for (int i = 0; i < BUTTON_AVERAGING_ARRAY_LEN; i++)
  {
    sum += arcade_button_array[i];
  }

  midi_needs_update = (!sum);
}

/**
   Return the current reading
*/
bool ArcadeButton::GetReading(void)
{
  return current_reading;
}
