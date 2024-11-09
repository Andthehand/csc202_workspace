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
#include "adc.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void msp_printf(char* string);

void run_lab10_part2();

void flash_leds();
void inc_seg7(bool incrament);
void display_temp();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART2_STRING_MENU                                    "MENU OPTIONS\r\n"
#define PART2_STRING_MENU_1     "\t1. Increment count on 7-segment display\r\n"
#define PART2_STRING_MENU_2                 "\t2. Show current temperature\r\n"
#define PART2_STRING_MENU_3                       "\t3. Flash LEDs 3 times\r\n"
#define PART2_STRING_MENU_4                              "\t4. End Program\r\n"
#define PART2_STRING_ENTER_SEL                         "Enter your selection: "

#define PART2_STRING_THANKS               "Thank you for using the program\r\n"
#define PART2_STRING_NOT_VALID             "ERROR: Your input is not valid\r\n"
#define PART2_STRING_NEW_LINE                                            "\r\n"

#define PART2_STRING_TEMP                                             "Temp = "
#define PART2_STRING_END                                      "Program Stopped"

#define PART2_CHAR_DEGREE                                                  0xDF
#define PART2_CHAR_FAHRENHEIT                                               'F'
#define PART2_CHAR_END_STRING                                              '\0'

#define PART2_FAHRENHEIT_CONVERSTION                             9.0 / 5.0 + 32
#define PART2_CHANNEL_TEMP                                                    5
#define PART2_LCD_MAX                                                         9
#define PART2_BLINK_COUNT                                                     3
#define PART2_BLINK_DELAY                                                   250

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

  led_init();
  seg7_init();

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  I2C_init();
  lcd1602_init();

  run_lab10_part2();
  
  // Endless loop to prevent program from ending
  while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  A custom implementation of printing using the MSPM0
//  
// INPUT PARAMETERS: 
//  string: A char array terminated with \0.
//  
// OUTPUT PARAMETERS:
//  none 
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void msp_printf(char* string) {
  while (*string != PART2_CHAR_END_STRING)
  {
    UART_out_char(*string++);
  } /* while */
} /* msp _printf */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function displays a menu on a connected device and executes specific 
//  actions based on user input until the user chooses to exit. 
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
void run_lab10_part2()
{
  leds_on(0xFF);
  seg7_hex(0, SEG7_DIG0_ENABLE_IDX);

  bool finished = false;
  while (!finished) {
    msp_printf(PART2_STRING_MENU);
    msp_printf(PART2_STRING_MENU_1);
    msp_printf(PART2_STRING_MENU_2);
    msp_printf(PART2_STRING_MENU_3);
    msp_printf(PART2_STRING_MENU_4);
    msp_printf(PART2_STRING_ENTER_SEL);

    char input = UART_in_char();
    UART_out_char(input);
    msp_printf(PART2_STRING_NEW_LINE);
    msp_printf(PART2_STRING_NEW_LINE);
    switch (input) {
      case '1':
        inc_seg7(true);
        break;
      case '2':
        display_temp();
        break;
      case '3':
        seg7_off();
        flash_leds();
        inc_seg7(false);
        break;
      case '4':
        msp_printf(PART2_STRING_THANKS);
        finished = true;
        break;
      default:
        msp_printf(PART2_STRING_NOT_VALID);
        break;
    } 
  }

  seg7_off();
  leds_off();

  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(PART2_STRING_END);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function blinks all LEDs a specified number of times.
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
void flash_leds()
{
  leds_on(0xFF);

  for(uint8_t i = 0; i < PART2_BLINK_COUNT; i++)
  {
    led_enable();
    msec_delay(PART2_BLINK_DELAY);
    led_disable();
    msec_delay(PART2_BLINK_DELAY);
  }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function increments a counter (with a maximum of 9) and displays the 
//  current count on a 7-segment display.
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
void inc_seg7(bool incrament)
{
  static uint8_t count = 0;

  if(incrament && ++count > PART2_LCD_MAX)
    count = 0;
  
  seg7_hex(count, SEG7_DIG0_ENABLE_IDX);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function reads and displays temperature in Fahrenheit on an LCD. 
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
void display_temp()
{
  uint16_t temp_value = ADC0_in(PART2_CHANNEL_TEMP);
  float celsius_temp = thermistor_calc_temperature(temp_value);
  float fahrenheit_temp = celsius_temp * PART2_FAHRENHEIT_CONVERSTION;

  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(PART2_STRING_TEMP);
  lcd_write_doublebyte(fahrenheit_temp);
  lcd_write_char(PART2_CHAR_DEGREE);
  lcd_write_char(PART2_CHAR_FAHRENHEIT);
}
