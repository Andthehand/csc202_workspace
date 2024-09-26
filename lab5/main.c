//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 5
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
#define DEBOUNCE_MSEC                 10
#define HALF_SECOND                   500

#define SEG7_BLINK_ITER_COUNT         3
#define SW_READ_ITER_COUNT            3
#define KEY_READ_ITER_COUNT           8
#define LED_BAR_BLINK_ITER_COUNT      4

#define LED_STATES                    2


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold different data types

typedef enum
{
  GET_LOW  = 0,
  GET_HIGH = 1,
  DISPLAY  = 2
} READ_STATE;

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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function blinks a 3 on the seg7 display every time you press PB1.
//  Blinking 3 times before moving on.
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
void run_lab5_part1()
{
  for(uint8_t i = 0; i < SEG7_BLINK_ITER_COUNT; i++)
  {
    uint8_t count = 0;
    while(count < LED_STATES) 
    {
      while(!is_pb_down(PB1_IDX));

      seg7_hex(0x03, 0);
      count++;

      msec_delay(DEBOUNCE_MSEC);
      while(is_pb_down(PB1_IDX));
      msec_delay(DEBOUNCE_MSEC);
    } /* while */

    seg7_off();
    leds_off();
  } /* for */
} /* run_lab5_part1 */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function reads the dipsw twice to populate an 8 bit number to then
//  display either seg7 0 or seg7 2. It advances the read state every time SW2  
//  on the launchpad.
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
void run_lab5_part2()
{
  msec_delay(HALF_SECOND);
  
  READ_STATE state = GET_LOW;
  uint8_t count = 0;
  uint8_t value = 0;
  while (count < SW_READ_ITER_COUNT) 
  {
    bool advanceNextState = is_lpsw_down(LP_SW2_IDX);

    switch (state) 
    {
      case GET_LOW:
        if(advanceNextState)
        {
          value = dipsw_read();
          state = GET_HIGH;
        } /* if */
        break;
      case GET_HIGH:
        if(advanceNextState)
        {
          value |= dipsw_read() << 4;
          state = DISPLAY;
        } /* if */
        break;
      case DISPLAY:
        if(is_pb_down(PB1_IDX))
        {
          seg7_on(value, 2);
        } /* if */
        else 
        {
          seg7_on(value, 0);
        } /* else */
        
        if(advanceNextState)
        {
          leds_off();
          seg7_off();
          state = GET_LOW;
          count++;
        } /* if */
        break;
    } /* switch */

    if(advanceNextState)
    {
      msec_delay(DEBOUNCE_MSEC);
      while (is_lpsw_down(LP_SW2_IDX));
      msec_delay(DEBOUNCE_MSEC);
    } /* if */
  } /* while */
} /* run_lab5_part2 */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function reads the keypad then displays the number in binary on the 
//  LED light bar.
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
void run_lab5_part3()
{
  msec_delay(HALF_SECOND);
  
  led_enable();
  uint8_t count = 0;
  while(count < KEY_READ_ITER_COUNT)
  {
    uint8_t key = getkey_pressed();

    leds_on(key);
    count++;

    msec_delay(DEBOUNCE_MSEC);
    wait_no_key_pressed();
    msec_delay(DEBOUNCE_MSEC);
  } /* while */

  leds_off();
} /* run_lab5_part3 */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function reads the keypad then blinks the LED bar as many times
//  as the value of the key pressed on the keypad.
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
void run_lab5_part4()
{
  msec_delay(HALF_SECOND);

  led_enable();
  uint8_t count = 0;
  while(count < LED_BAR_BLINK_ITER_COUNT)
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
