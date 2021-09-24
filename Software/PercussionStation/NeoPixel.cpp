/*******************************************************
 *  File: NeoPixel.cpp
 *    
 *  Author: Chase E. Stewart
 *  For Hidden Layer Design
 *  
 *  License: Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 *
 *******************************************************/
#include "NeoPixel.h"

static void applyFrame(WS2812Serial NeoStick, neoStickFrame_t frame)
{
  for (int i = 0; i < NeoStick_count; i++)
  {
    NeoStick.setPixel(i, frame.pixelArray[i].green, frame.pixelArray[i].red, frame.pixelArray[i].blue);
  }
  NeoStick.show();
}


void updateNeoPixelStick(WS2812Serial NeoStick, uint8_t value)
{
  value  = constrain(value, 0, 127);
  applyFrame(NeoStick, countdownArray[value / 8]); 
}
