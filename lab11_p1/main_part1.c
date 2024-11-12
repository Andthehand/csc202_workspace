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
#include "spi.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void msp_printf(char* string);

void run_lab11_part1();

void get_spi_data();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART1_STRING_MENU                                    "MENU OPTIONS\r\n"
#define PART1_STRING_MENU_1                         "\t1. Set Data to Send\r\n"
#define PART1_STRING_MENU_2                                "\t2. Send Data\r\n"
#define PART1_STRING_MENU_3                              "\t3. Update LEDs\r\n"
#define PART1_STRING_MENU_4                              "\t4. End Program\r\n"
#define PART1_STRING_ENTER_SEL                         "Enter your selection: "

#define PART1_STRING_THANKS               "Thank you for using the program\r\n"
#define PART1_STRING_NOT_VALID             "ERROR: Your input is not valid\r\n"
#define PART1_STRING_NEW_LINE                                            "\r\n"

#define PART1_STRING_XMIT                 "Set SPI xmit data menu selected\r\n"
#define PART1_STRING_ENTER_VAL      "Enter a valid decimal number (0 to 255): "
#define PART1_STRING_SPI_SET                                "SPI Data set to: "
#define PART2_STRING_END                                      "Program Stopped"

#define PART1_CHAR_DEGREE                                                  0xDF
#define PART1_CHAR_FAHRENHEIT                                               'F'
#define PART1_CHAR_END_STRING                                              '\0'
#define PART1_CHAR_END_LINE                                                '\n'
#define PART1_CHAR_ZERO                                                     '0'

#define PART1_FAHRENHEIT_CONVERSTION                             9.0 / 5.0 + 32
#define PART1_CHANNEL_TEMP                                                    5
#define PART1_LCD_MAX                                                         9
#define PART1_BLINK_COUNT                                                     3
#define PART1_BLINK_DELAY                                                   250

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
  spi1_init();

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  I2C_init();
  lcd1602_init();

  run_lab11_part1();
  
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
  while (*string != PART1_CHAR_END_STRING)
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
void run_lab11_part1()
{
  leds_on(0xFF);
  seg7_hex(0, SEG7_DIG0_ENABLE_IDX);

  bool finished = false;
  while (!finished) {
    msp_printf(PART1_STRING_MENU);
    msp_printf(PART1_STRING_MENU_1);
    msp_printf(PART1_STRING_MENU_2);
    msp_printf(PART1_STRING_MENU_3);
    msp_printf(PART1_STRING_MENU_4);
    msp_printf(PART1_STRING_ENTER_SEL);

    char input = UART_in_char();
    UART_out_char(input);
    msp_printf(PART1_STRING_NEW_LINE);
    msp_printf(PART1_STRING_NEW_LINE);
    switch (input) {
      case '1':
        get_spi_data();
        break;
      case '2':
        break;
      case '3':
        break;
      case '4':
        msp_printf(PART1_STRING_THANKS);
        finished = true;
        break;
      default:
        msp_printf(PART1_STRING_NOT_VALID);
        break;
    } 
  }

  seg7_off();
  leds_off();

  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(PART1_STRING_END);
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
void get_spi_data()
{
  msp_printf(PART1_STRING_XMIT);
  msp_printf(PART1_STRING_ENTER_VAL);


  uint32_t number = 0;
  do {
    char input = UART_in_char();
    number = input - PART1_CHAR_ZERO
  }while (input != PART1_CHAR_END_LINE);

  msp_printf(PART1_STRING_NEW_LINE);
  msp_printf(PART1_STRING_NEW_LINE);

  if(number <= 255)
  {
    spi1_write_data(number);

    msp_printf(PART1_STRING_SPI_SET + number);
    msp_printf(PART1_STRING_NOT_VALID);
    msp_printf(PART1_STRING_NEW_LINE);
    msp_printf(PART1_STRING_NEW_LINE);
  }
  else 
  {
    spi1_write_data(0);

    msp_printf(PART1_STRING_NOT_VALID)
    msp_printf(PART1_STRING_SPI_SET + '0');
    msp_printf(PART1_STRING_NEW_LINE);
    msp_printf(PART1_STRING_NEW_LINE);
  }
}
