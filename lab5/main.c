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
#include "clock.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab5_part1();
void run_lab5_part2();
void run_lab5_part3();
void run_lab5_part4();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define DEBOUNCE_MSEC 10
#define HALF_SECOND   500


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
typedef enum
{
  GET_LOW  = 0,
  GET_HIGH = 1,
  DISPLAY  = 2
} READ_STATE;

// Define a structure to hold different data types

int main(void)
{
  clock_init_40mhz();

  launchpad_gpio_init();

  led_init();
  seg7_init();
  dipsw_init();
  keypad_init();
  lpsw_init();

  run_lab5_part1();
  run_lab5_part2();
  run_lab5_part3();
  run_lab5_part4();

  // Endless loop to prevent program from ending
  while (1);
} /* main */

void run_lab5_part1()
{
  for(uint8_t i = 0; i < 3; i++)
  {
    uint8_t count = 0;
    while (count < 2) 
    {
      bool pressed = false;
      while (is_pb_down(PB1_IDX)) {
        seg7_hex(0x03, 0);
        msec_delay(DEBOUNCE_MSEC);

        pressed = true;
      } /* while */

      if(pressed)
      {
        count++;
      } /* if */
    } /* while */

    seg7_off();
    leds_off();
  } /* for */
} /* run_lab5_part1 */

void run_lab5_part2()
{
  msec_delay(HALF_SECOND);
  
  READ_STATE state = GET_LOW;
  uint8_t count = 0;
  uint8_t value = 0;
  while (count < 3) 
  {
    switch (state) 
    {
      case GET_LOW:
        if(is_lpsw_down(LP_SW2_IDX))
        {
          value = dipsw_read();
          state = GET_HIGH;

          msec_delay(DEBOUNCE_MSEC);
          while(is_lpsw_down(LP_SW2_IDX));
          msec_delay(DEBOUNCE_MSEC);
        } /* if */
        break;
      case GET_HIGH:
        if(is_lpsw_down(LP_SW2_IDX))
        {
          value |= dipsw_read() << 4;
          state = DISPLAY;

          msec_delay(DEBOUNCE_MSEC);
          while(is_lpsw_down(LP_SW2_IDX));
          msec_delay(DEBOUNCE_MSEC);
        } /* if */
        break;
      case DISPLAY:
        if(is_pb_down(PB1_IDX))
        {
          seg7_on(value, 0);
        } /* if */
        else 
        {
          seg7_on(value, 2);
        } /* else */
        
        if(is_lpsw_down(LP_SW2_IDX))
        {
          leds_off();
          seg7_off();
          state = GET_LOW;
          count++;

          msec_delay(DEBOUNCE_MSEC);
          while(is_lpsw_down(LP_SW2_IDX));
          msec_delay(DEBOUNCE_MSEC);
        } /* if */
        break;
    } /* switch */
  } /* while */
} /* run_lab5_part2 */

void run_lab5_part3()
{
  msec_delay(HALF_SECOND);
  
  led_enable();
  uint8_t count = 0;
  while(count < 8)
  {
    uint8_t key = getkey_pressed();

    leds_on(key);


    msec_delay(DEBOUNCE_MSEC);
    wait_no_key_pressed();

    count++;
  } /* while */

  leds_off();
} /* run_lab5_part3 */

void run_lab5_part4()
{
  msec_delay(HALF_SECOND);

  led_enable();
  uint8_t count = 0;
  while(count < 4)
  {
    uint8_t key = getkey_pressed();
    for(uint8_t i = 0; i < key; i++)
    {
      leds_on(0xFF);
      msec_delay(HALF_SECOND);
      
      leds_off();
      msec_delay(HALF_SECOND);
    } /* for */

    count++;
  } /* while */
} /* run_lab5_part4 */