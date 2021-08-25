/******************************************************* 
 *  File: Joystick.h
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)    
 *  
 *******************************************************/
#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "Arduino.h"
#include "MIDIConstants.h"

enum joystickDirectionId 
{
  JOY_UP = 0,
  JOY_DOWN = 1,
  JOY_LEFT = 2,
  JOY_RIGHT = 3,
  JOY_LIMIT = 3
};

class DiscreteJoystick
{
  public:
    DiscreteJoystick(uint8_t upPin, uint8_t downPin, uint8_t leftPin, uint8_t rightPin, int xAxis_cc, int yAxis_cc);
    void UpdateXAxis(config_t in_config);
    void UpdateYAxis(config_t in_config);
    
  private:
    int _upPin;
    int _downPin;
    int _leftPin;
    int _rightPin;
    int _xAxisCC;
    int _yAxisCC;
    int xAxisVal;
    int yAxisVal;
    bool upPressed;
    bool downPressed;
    bool leftPressed;
    bool rightPressed;
    bool isXDirectionHeld;
    bool wasXDirectionHeld;
    joystickDirectionId currentXDirection;
    joystickDirectionId lastXDirection;
    unsigned long holdXStartMillis;
    bool isYDirectionHeld;
    bool wasYDirectionHeld;
    joystickDirectionId currentYDirection;
    joystickDirectionId lastYDirection;
    unsigned long holdYStartMillis;
};

#endif // __JOYSTICK_H__
