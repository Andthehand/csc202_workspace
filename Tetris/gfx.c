// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  ST7735S.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    
// *****************************************************************************
//******************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "gfx.h"

void draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, color565_t color)
{
  ST7735S_set_addr(x, y, x + width, y + width);
  ST7735S_write_command(LCD_RAMWR_CMD);
  for(uint16_t i = 0; i < width * height; i++)
  {
    ST7735S_write_color(color);
  }
}