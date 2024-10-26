//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  TBD
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a ... 
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>

#include "ST7735S.h"
#include "Joystick.h"
#include "Events.h"

#include "LaunchPad.h"
#include "clock.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


// Define a structure to hold different data types

int main(void)
{
  clock_init_40mhz();
  launchpad_gpio_init();

  ST7735S_init();
  joystick_init();

  init_events();

  bool pressed = false;
  //Game loop
  while (true) {
    ST7735S_set_addr(0, 0);
    ST7735S_write_command(LCD_RAMWR_CMD);
    if (is_pb1_down(true)) {
      pressed = !pressed;
    }
    for (uint16_t i = 0; i < (128 * 128); i++)
    {
      ST7735S_write_color(RGB_to_color(pressed ? 0x00 : 0xFF, 0x00, 0xFF));
    }
  }

  // Endless loop to prevent program from ending
  while (1);

} /* main */
