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
#include "clock.h"
#include "ST7735S.h"
#include "LaunchPad.h"
#include "Joystick.h"
#include "uart.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void msp_printf(char* buffer, unsigned int value);

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
  UART_init(115200);

  ST7735S_init();

  joystick_init();

  //Game loop
  while (true) {
    ST7735S_set_addr(0, 0);
    ST7735S_write_command(LCD_RAMWR_CMD);
    for (uint16_t i = 0; i < (128 * 128); i++)
    {
      ST7735S_write_color(RGB_to_color(0xFF, 0x00, 0xFF));
      joystick_pos pos = read_joystick();
      msp_printf("Joystick X pos: %u\r\n", pos.x_pos);
      msp_printf("Joystick Y pos: %u\r\n", pos.y_pos);
      msec_delay(100);
    }
  }

  // Endless loop to prevent program from ending
  while (1);

} /* main */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//  A custom implementation of printing using the MSPM0 with a printf format
//  
// INPUT PARAMETERS: 
//  buffer: A format string using the same format as sprintf.
//  value: The integer value to be formatted into the output string.
//  
// OUTPUT PARAMETERS:
//  none 
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void msp_printf(char* buffer, unsigned int value) {
  unsigned int i = 0;
  unsigned int len = 0;
  char string[80];

  len = sprintf(string, buffer, value);

  // Sends the string one bit at a time through UART
  for(i = 0; i < len; i++)
  {
    UART_out_char(string[i]);
  } /* for */

} /* msp_printf */
