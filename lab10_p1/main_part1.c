//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 10
//
//      FILE NAME:  main_part1.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to use UART out
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
#define PART1_CHAR_RETURN                                                  '\r'
#define PART1_CHAR_BACKSPACE                                               '\b'
#define PART1_CHAR_END_STRING                                              '\0'
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


//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function captures a user's name input via UART, character by character,
//  and displays it on an LCD screen. 
//
// INPUT PARAMETERS:
//    none
//
// OUTPUT PARAMETERS:
//    none
//
// RETURN:
//    none
// -----------------------------------------------------------------------------
void run_lab10_part1()
{
  char input;
  char name[PART1_NAME_LENGTH];
  uint8_t index = 0;
  do 
  {
    input = UART_in_char();
    UART_out_char(input);

    if(input == PART1_CHAR_BACKSPACE)
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
  while(input != PART1_CHAR_RETURN);

  name[index - 1] = PART1_CHAR_END_STRING;

  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(PART1_STRING_NAME);
  lcd_write_string(name);

  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string(PART1_STRING_END);
}