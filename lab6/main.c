//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 6: Interfacing to Liquid Crystal Display
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to interface with the lcd Screen.
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
#include "lcd1602.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void run_lab6_part1();
void run_lab6_part2();
void run_lab6_part3();
void run_lab6_part4();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define DEBOUNCE_MSEC                 20

#define SECOND_DELAY                  1000
#define ONE_FIFTH_SECOND_DELAY        200

#define PRESS_PB2_CONTINUE_STRING     "Press PB2"

#define PART1_STARTING_CHAR           'A'
#define PART1_NEXT_LINE_CHAR          'Q'
#define PART1_ENDING_CHAR             'Z'

#define PART2_STRING_RUNNING          "Running Part 2"
#define PART2_STRING_FINISHED         "Part 2 Done."
#define PART2_QUADBYTE                ((uint32_t)1234567890)
#define PART2_DOUBLEBYTE              ((uint16_t)1234)
#define PART2_CHAR                    ((uint8_t)123)

#define PART3_STRING_RUNNING          "Running Part 3"
#define PART3_STRING_FINISHED         "Part 3 Done"
#define PART3_DEFAULT_TIMER_VALUE     100

#define PART4_STRING_RUNNING          "Running Part 4"
#define PART4_STRING_FINISHED         "Program Stopped"


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


// Define a structure to hold different data types

int main(void)
{
  clock_init_40mhz();
  launchpad_gpio_init();

  I2C_init();
  lcd1602_init();
  lcd_set_display_on();

  dipsw_init();
  keypad_init();

  run_lab6_part1();
  run_lab6_part2();
  run_lab6_part3();
  run_lab6_part4();

  // Endless loop to prevent program from ending
  while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function tells the LCD to display A-P on the first line and Q-Z 
//  centered on the Second.
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
void run_lab6_part1()
{
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  for(uint8_t i = PART1_STARTING_CHAR; i <= PART1_ENDING_CHAR; i++) 
  {
    if(i == PART1_NEXT_LINE_CHAR)
    {
      lcd_set_ddram_addr(LCD_LINE2_ADDR);
    } /* if */
    
    lcd_write_char(i);
  } /* for */
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function controls the sequence of LCD operations and button debouncing 
//  for displaying various strings and values during runtime, waiting for button 
//  presses to proceed through different stages of operation.
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
void run_lab6_part2()
{
  while(is_pb_up(PB2_IDX));

  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_clear();
  lcd_write_string(PART2_STRING_RUNNING);
  msec_delay(SECOND_DELAY);

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_4);

  lcd_write_quadbyte(PART2_QUADBYTE);

  while(is_pb_up(PB1_IDX));
  lcd_set_ddram_addr(LCD_LINE2_ADDR + LCD_CHAR_POSITION_6);
  lcd_write_doublebyte(PART2_DOUBLEBYTE);

  msec_delay(DEBOUNCE_MSEC);
  while(is_pb_down(PB1_IDX));
  msec_delay(DEBOUNCE_MSEC);

  while(is_pb_up(PB1_IDX));
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);

  lcd_write_byte(PART2_CHAR);
  
  msec_delay(DEBOUNCE_MSEC);
  while(is_pb_down(PB1_IDX));
  msec_delay(DEBOUNCE_MSEC);

  while(is_pb_up(PB1_IDX));
  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);

  lcd_write_string(PART2_STRING_FINISHED);

  msec_delay(DEBOUNCE_MSEC);
  while(is_pb_down(PB1_IDX));
}


//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function manages a countdown timer displayed on the LCD, starting after 
//  button press detection. It continuously decrements the timer, resetting if 
//  button 1 is pressed, and ends when button 2 is pressed. Additionally, it 
//  provides user prompts and feedback throughout the process using the LCD 
//  display.
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
void run_lab6_part3()
{
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string(PRESS_PB2_CONTINUE_STRING);

  while(is_pb_up(PB2_IDX));
  lcd_clear();
  lcd_write_string(PART3_STRING_RUNNING);
  msec_delay(SECOND_DELAY);
  
  lcd_clear();

  uint8_t timer_count = PART3_DEFAULT_TIMER_VALUE;
  bool done = false;
  while (!done) 
  {
    lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_7);
    lcd_write_byte(timer_count);

    msec_delay(ONE_FIFTH_SECOND_DELAY);

    timer_count--;
    if(timer_count == UINT8_MAX || is_pb_down(PB1_IDX))
    {
      timer_count = PART3_DEFAULT_TIMER_VALUE;
    } /* if */

    if(is_pb_down(PB2_IDX))
    {
      done = true;
    } /* if */
  } /* while */

  lcd_clear();
  lcd_write_string(PART3_STRING_FINISHED);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function handles LCD display updates based on keypad input and button 
//  interactions. It prompts the user to start, displays a running message, and 
//  waits for keypad input to display characters sequentially. The screen clears 
//  if button 1 is pressed, and the process ends when button 2 is pressed. It 
//  uses a two-line LCD display to output characters, resetting as needed based 
//  on input conditions.
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
void run_lab6_part4()
{
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string(PRESS_PB2_CONTINUE_STRING);

  while(is_pb_up(PB2_IDX));
  lcd_clear();
  lcd_write_string(PART4_STRING_RUNNING);
  msec_delay(SECOND_DELAY);

  lcd_clear();

  bool done = false;
  uint8_t index = 0;
  while(!done)
  {
    uint8_t key = keypad_scan();

    if(is_pb_down(PB1_IDX))
    {
      lcd_clear();
      index = 0;
    } /* if */

    if(is_pb_down(PB2_IDX))
    {
      done = true;
    } /* if */

    if(key != NO_KEY_PRESSED)
    {
      if(index == CHARACTERS_PER_LCD_LINE)
      {
        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        index++;
      }  /* if */
      else if(index / CHARACTERS_PER_LCD_LINE == LINES_PER_LCD)
      {
        lcd_clear();
        index = 0;
      } /* else if */
      else {
        index++;
      } /* else */

      lcd_write_char(hex_to_ascii(key));
      wait_no_key_pressed();
    }
  } /* while */

  lcd_clear();
  lcd_write_string(PART4_STRING_FINISHED);
}
