/*******************************************************
 *  File: DiscreteJoystick.cpp
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#include "Arduino.h"
#include "TestStationBSP.h"
#include "DiscreteJoystick.h"
#include "Preferences.h"


DiscreteJoystick::DiscreteJoystick(uint8_t upPin, uint8_t downPin, uint8_t leftPin, uint8_t rightPin, int xAxis_cc, int yAxis_cc)
{
  _upPin = upPin;
  _downPin = downPin;
  _leftPin = leftPin;
  _rightPin = rightPin;
  _xAxisCC = xAxis_cc;
  _yAxisCC = yAxis_cc;
}

void DiscreteJoystick::UpdateXAxis(config_t in_config)
{
  leftPressed    = !digitalRead(_leftPin);
  rightPressed  = !digitalRead(_rightPin);
    
  isXDirectionHeld = (leftPressed || rightPressed);
  
  if(isXDirectionHeld && !wasXDirectionHeld )
  {
    holdXStartMillis = millis();
    if (leftPressed) currentXDirection = JOY_LEFT;
    else if (rightPressed) currentXDirection = JOY_RIGHT;
  }
  else if (isXDirectionHeld && wasXDirectionHeld)
  {
    if ((millis() - holdXStartMillis) > PREFS_JOYSTICK_DEBOUNCE_MSEC)
    {
      xAxisVal = (JOY_LEFT == currentXDirection) ? xAxisVal - 10 : xAxisVal + 10;
      xAxisVal = constrain(xAxisVal, 0, 127);
      usbMIDI.sendControlChange(_xAxisCC, xAxisVal, in_config.MIDI_Channel);
      holdXStartMillis = millis();
    }

    if (lastXDirection != currentXDirection)
    {      
      holdXStartMillis = millis();
    }
  }
  lastXDirection = currentXDirection;
  wasXDirectionHeld = isXDirectionHeld;
}

void DiscreteJoystick::UpdateYAxis(config_t in_config)
{
  upPressed    = !digitalRead(_upPin);
  downPressed  = !digitalRead(_downPin);
    
  isYDirectionHeld = (upPressed || downPressed);
  
  if(isYDirectionHeld && !wasYDirectionHeld )
  {
    holdYStartMillis = millis();
    if (upPressed) currentYDirection = JOY_UP;
    else if (downPressed) currentYDirection = JOY_DOWN;
  }
  else if (isYDirectionHeld && wasYDirectionHeld)
  {
    if ((millis() - holdYStartMillis) > PREFS_JOYSTICK_DEBOUNCE_MSEC)
    {
      yAxisVal = (JOY_DOWN == currentYDirection) ? yAxisVal - 10 : yAxisVal + 10;
      yAxisVal = constrain(yAxisVal, 0, 127);
      usbMIDI.sendControlChange(_yAxisCC, yAxisVal, in_config.MIDI_Channel);   
      holdYStartMillis = millis();
    }

    if (lastYDirection != currentYDirection)
    {      
      holdYStartMillis = millis();
    }
  }
  lastYDirection = currentYDirection;
  wasYDirectionHeld = isYDirectionHeld;
}
