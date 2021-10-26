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

static void applyFrame(WS2812Serial NeoStick, neoStickFrame_t frame, bool invert)
{
  for (int i = 0; i < NeoStick_count; i++)
  {
    /* Flip the NeoStick lights if EEPROM dictates that we should (this is based on physical install) */
    int pixelIdx = (invert) ? (NeoStick_count - i) : i;    
    NeoStick.setPixel(i, frame.pixelArray[pixelIdx].green, frame.pixelArray[pixelIdx].red, frame.pixelArray[pixelIdx].blue);
  }
  NeoStick.show();
}


void updateNeoPixelStick(WS2812Serial NeoStick, uint8_t value, stationType_t hwType, neoStripInvertState_t invert)
{
  value  = constrain(value, 0, 127);
  switch(hwType)
  {
    case STATION_TYPE_PERCUSSION:
      // NeoStick.setPixel has a prototype like setPixel(uint16_t index, uint8_t red, uint8_t green, uint8_t blue)
      applyFrame(NeoStick, blueArray[value / 8], (invert == NEOSTRIP_INVERT_YES)); 
      break;
      
    case STATION_TYPE_MELODIC:
      applyFrame(NeoStick, yellowArray[value / 8], (invert == NEOSTRIP_INVERT_YES)); 
      break;
      
    case   STATION_TYPE_SWEEP:
      applyFrame(NeoStick, whiteArray[value / 8], (invert == NEOSTRIP_INVERT_YES)); 
      break;

    default :
      // should never get here
      break;     
  }  
}
