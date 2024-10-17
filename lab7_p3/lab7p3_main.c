//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 7: Working with Interrupts
//
//      FILE NAME:  lab7p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to get used to interrupts in a program.
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
#include "LaunchPad.h"
#include "lcd1602.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab7_part3();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define MSP0_CLOCK_FREQUENCY                                              (40E6)
#define SYS_TICK_PERIOD                                               (10.24E-3)
#define SYS_TICK_PERIOD_COUNT     ((SYS_TICK_PERIOD * MSP0_CLOCK_FREQUENCY) - 1)

#define PART3_DELAY                                                          200
#define PART3_ITER_COUNT                                                     100

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


// Define a structure to hold different data types

int main(void)
{
  clock_init_40mhz();
  launchpad_gpio_init();
  dipsw_init();

  led_init();
  seg7_init();

  I2C_init();
  lcd1602_init();
  lcd_set_display_on();

  sys_tick_init(SYS_TICK_PERIOD_COUNT);
  
  run_lab7_part3();

  // Endless loop to prevent program from ending
  while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  The interupt is called every 10.24 msec and displays how many dipsw are 
//  switched on.
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
void SysTick_Handler(void)
{
  uint8_t num_switches_down = 0;
  for(uint8_t i = 0; i < MAX_NUM_DIPSW; i++)
  {
    num_switches_down += is_pb_down(i);
  } /* for */

  seg7_hex(num_switches_down, SEG7_DIG0_ENABLE_IDX);
} /* SysTick_Handler */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code continuously writes byte values to a specific position on an LCD 
//  screen in an infinite loop.
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
void run_lab7_part3()
{
  while(true)
  {
    for(uint8_t i = 0; i < PART3_ITER_COUNT; i++)
    {
      lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);

      lcd_write_byte(i);

      msec_delay(PART3_DELAY);
    } /* for */
  } /* while */
} /* run_lab7_part3 */