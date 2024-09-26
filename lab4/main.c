//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 4
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a test for how to drive both the led bar and the
//    7 segment display using the launchpad header and source files.
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdint.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "LaunchPad.h"
#include "clock.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab4_part1();
void run_lab4_part2();
void run_lab4_part3();
void run_lab4_part4();
void run_lab4_part5(); 
void run_lab4_part6(); 

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define SEG7_LETTER_L (1 << SEG7_SEG_F_IDX) | (1 << SEG7_SEG_E_IDX) | (1 << SEG7_SEG_D_IDX)
#define LED_3C        0x3C

#define LED_INDEX_RUN_ITER_COUNT    10
#define MAX8_ITER_COUNT             2
#define SEG7_BLINK_ITER_COUNT       4
#define MULTIPLEX_ITER_COUNT        200

#define THREE_SECOND      3000
#define TWO_SECOND        2000
#define ONE_SECOND        1000
#define HALF_SECOND       500
#define QUARTER_SECOND    250
#define TWO_HUNDRED_MSEC  200
#define MULTIPLEX_DELAY   5

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold different data types

int main(void)
{
  clock_init_40mhz();
  
  launchpad_gpio_init();

  led_init();
  seg7_init();
  led_enable();

  run_lab4_part1();
  run_lab4_part2();
  run_lab4_part3();
  run_lab4_part4();
  run_lab4_part5();
  run_lab4_part6();
 
 // Endless loop to prevent program from ending
 while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function Displays leds 2-5 on the led bar
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
void run_lab4_part1() 
{
  leds_on(LED_3C);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function runs along the led bar going from leds 0-7 then from 
//  7-0 10 times.
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
void run_lab4_part2() 
{
  msec_delay(ONE_SECOND);
  leds_off();

  //Go from leds 0-7 then from 7-0 10 times
  for(uint8_t i = 0; i < LED_INDEX_RUN_ITER_COUNT; i++) 
  {
    for(uint8_t i = 0; i < (MAX_NUM_LEDS - 1); i++) 
    {
      led_on(i);
      msec_delay(TWO_HUNDRED_MSEC);
      led_off(i);
    } /* for */
    
    for(int8_t i = (MAX_NUM_LEDS - 1); i > 0; i--) 
    {
      led_on(i);
      msec_delay(TWO_HUNDRED_MSEC);
      led_off(i);
    } /* for */
  } /* for */
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function counts to 255 twice and displays the bianary on the led bar 
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
void run_lab4_part3()
{
  msec_delay(HALF_SECOND);  

  //Count to 255 from 0 twice
  uint8_t count = 0;
  while (count < MAX8_ITER_COUNT)
  {
    for(uint16_t i = 0; i <= UINT8_MAX; i++)
    {
      msec_delay(QUARTER_SECOND);  
      leds_on(i);
    } /* for */

    count++;
  } /* while */
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function displays the letter L on the 7 segment display
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
void run_lab4_part4() 
{
  msec_delay(HALF_SECOND);  
  led_disable();

  seg7_on(SEG7_LETTER_L, 0);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function blinks the number 4 for 3 seconds on and 2 seconds off 4 times
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
void run_lab4_part5() 
{
  msec_delay(HALF_SECOND);  

  //Blink number 4 for 2 times
  uint8_t count = 0;
  while (count < SEG7_BLINK_ITER_COUNT) 
  {
    seg7_hex(0x4, 2);
    msec_delay(THREE_SECOND);  
    seg7_off();
    msec_delay(TWO_SECOND);  

    count++;
  } /* while */
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function displays CAFE across the 7 segment display using multiplexing
//  to display the whole word.
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
void run_lab4_part6() 
{
  msec_delay(HALF_SECOND);  

  const uint8_t cafe[] = {
    0xC, 0xA, 0xF, 0xE
  };

  uint8_t count = 0;
  while(count < MULTIPLEX_ITER_COUNT) {
    //Loop through the cafe array
    for(int i = 0; i < 4; i++)
    {
      seg7_hex(cafe[i], i);
      msec_delay(MULTIPLEX_DELAY);  
    } /* for */

    count++;
  }

  seg7_off();
}