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
#include "Station1BSP.h"
#include "DiscreteJoystick.h"
#include "Preferences.h"




DiscreteJoystick::DiscreteJoystick(uint8_t upPin, uint8_t downPin, uint8_t leftPin, uint8_t rightPin)
{
  _upPin = upPin;
  _downPin = downPin;
  _leftPin = leftPin;
  _rightPin = rightPin;
  wasXDirectionHeld = false;
  isXDirectionHeld = false;
  holdXStartMillis = 0;
  wasYDirectionHeld = false;
  isYDirectionHeld = false;
  holdYStartMillis = 0;
}


void DiscreteJoystick::UpdateNote(int *scaleNote)
{
  if (!scaleNote) return;

  upPressed    = !digitalRead(_upPin);
  downPressed  = !digitalRead(_downPin);
    
  isXDirectionHeld = (upPressed || downPressed);
  
  if(isXDirectionHeld && !wasXDirectionHeld )
  {
    holdXStartMillis = millis();
    if (upPressed) currentXDirection = JOY_UP;
    else if (downPressed) currentXDirection = JOY_DOWN;
  }
  else if (isXDirectionHeld && wasXDirectionHeld)
  {
    if ((millis() - holdXStartMillis) > JOYSTICK_MSEC_PER_NOTE)
    {
      *scaleNote = (JOY_DOWN == currentXDirection) ? *scaleNote - 1 : *scaleNote + 1;
      *scaleNote = constrain(*scaleNote, 0, 50);
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

void DiscreteJoystick::UpdateVolume(int *scaleVolume)
{
  if (!scaleVolume) return;

  leftPressed    = !digitalRead(_leftPin);
  rightPressed  = !digitalRead(_rightPin);
    
  isYDirectionHeld = (leftPressed || rightPressed);
  
  if(isYDirectionHeld && !wasYDirectionHeld )
  {
    holdYStartMillis = millis();
    if (leftPressed) currentYDirection = JOY_LEFT;
    else if (rightPressed) currentYDirection = JOY_RIGHT;
  }
  else if (isYDirectionHeld && wasYDirectionHeld)
  {
    if ((millis() - holdYStartMillis) > JOYSTICK_MSEC_PER_NOTE)
    {
      *scaleVolume = (JOY_LEFT == currentYDirection) ? *scaleVolume - 10 : *scaleVolume + 10;
      *scaleVolume = constrain(*scaleVolume, 0, 127);
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
