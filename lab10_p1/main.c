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
#include "LaunchPad.h"
#include "lcd1602.h"
#include "clock.h"
#include "uart.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab10_part1();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART1_STRING_NAME                                               "Name:"
#define PART1_STRING_END                                      "Program Stopped"
#define PART1_NAME_LENGTH                                                    12

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

  I2C_init();
  lcd1602_init();

  run_lab10_part1();
  
  // Endless loop to prevent program from ending
  while (1);

} /* main */

void run_lab10_part1()
{
  char input;
  char name[PART1_NAME_LENGTH];
  uint8_t index = 0;
  do 
  {
    input = UART_in_char();
    UART_out_char(input);

    if(input == '\b')
    {
      if(index > 0)
      {
        index--;
      }
    }
    else if(index < PART1_NAME_LENGTH && index <= PART1_NAME_LENGTH) {
      name[index] = input;
      index++;
    }
  }
  while(input != '\r');

  name[index - 1] = '\0';

  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(PART1_STRING_NAME);
  lcd_write_string(name);

  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string(PART1_STRING_END);
}