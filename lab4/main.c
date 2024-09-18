//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  TBD
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

void run_lab4_part1() 
{
  leds_on(0x3C);
}

void run_lab4_part2() 
{
  msec_delay(1000);
  leds_off();

  for(uint8_t i = 0; i < 10; i++) 
  {
    for(uint8_t i = 0; i < 8; i++) 
    {
      led_on(i);
      msec_delay(200);
      led_off(i);
    } /* for */
    
    for(int8_t i = 7; i >= 0; i--) 
    {
      led_on(i);
      msec_delay(200);
      led_off(i);
    } /* for */
  } /* for */
}

void run_lab4_part3()
{
  msec_delay(500);  

  uint8_t count = 0;
  while (count < 2)
  {
    for(uint16_t i = 0; i <= UINT8_MAX; i++)
    {
      msec_delay(250);  
      leds_on(i);
    } /* for */

    count++;
  } /* while */
}

void run_lab4_part4() 
{
  msec_delay(500);  
  led_disable();

  seg7_on(SEG7_LETTER_L, 0);
}

void run_lab4_part5() 
{
  msec_delay(500);  

  uint8_t count = 0;
  while (count < 4) 
  {
    seg7_hex(0x4, 2);
    msec_delay(3000);  
    seg7_off();
    msec_delay(2000);  

    count++;
  } /* while */
}

void run_lab4_part6() 
{
  msec_delay(500);  

  const uint8_t cafe[] = {
    0xC, 0xA, 0xF, 0xE
  };

  uint8_t count = 0;
  while(count < 200) {
    for(int i = 0; i < 4; i++)
    {
      seg7_hex(cafe[i], i);
      msec_delay(5);  
    } /* for */

    count++;
  }

  seg7_off();
}