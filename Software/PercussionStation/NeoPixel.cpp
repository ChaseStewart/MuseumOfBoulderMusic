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
#include "Nonvolatile.h"

static void applyFrame(WS2812Serial NeoStick, neoStickFrame_t frame)
{
  for (int i = 0; i < NeoStick_count; i++)
  {
    NeoStick.setPixel(i, frame.pixelArray[i].green, frame.pixelArray[i].red, frame.pixelArray[i].blue);
  }
  NeoStick.show();
}


void updateNeoPixelStick(WS2812Serial NeoStick, uint8_t value, stationType_t hwType)
{
  value  = constrain(value, 0, 127);
  switch(hwType)
  {
    case STATION_TYPE_PERCUSSION:
      // NeoStick.setPixel has a prototype like setPixel(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
      applyFrame(NeoStick, blueArray[value / 8]); 
      break;
      
    case STATION_TYPE_MELODIC:
      applyFrame(NeoStick, yellowArray[value / 8]); 
      break;
      
    case   STATION_TYPE_SWEEP:
      applyFrame(NeoStick, whiteArray[value / 8]); 
      break;

    default :
      // should never get here
      break;     
  }  
}
