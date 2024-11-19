//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 11
//
//      FILE NAME:  lab11p1_main.c
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
#include "spi.h"


//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void msp_printf(char* string);

void run_lab11_part1();

uint8_t get_spi_data();

uint16_t string_to_uint16(char* string);
void uint16_to_string(uint16_t value, char* string);

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
#define PART1_STRING_SEE_SERIAL                               "See serial port"
#define PART1_STRING_START                                    "Program running"
#define PART1_STRING_END                                      "Program Stopped"

#define PART1_STRING_XMIT                 "Set SPI xmit data menu selected\r\n"
#define PART1_STRING_ENTER_VAL      "Enter a valid decimal number (0 to 255): "
#define PART1_STRING_SPI_SET                                "SPI Data set to: "
#define PART1_STRING_SEND                                              "SEND: "
#define PART1_STRING_RECV                                              "RECV: "

#define PART1_CHAR_DEGREE                                                  0xDF
#define PART1_CHAR_FAHRENHEIT                                               'F'
#define PART1_CHAR_END_STRING                                              '\0'
#define PART1_CHAR_RETURN                                                  '\r'
#define PART1_CHAR_BACKSPACE                                               '\b'
#define PART1_CHAR_ZERO                                                     '0'

#define PART1_BLINK_DELAY                                                   250
#define BUFFER_SIZE                                                           6

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

  spi1_init();

  I2C_init();
  lcd1602_init();

  IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                    PINCM_GPIO_PIN_FUNC);
  GPIOB->DOE31_0 |= LP_SPI_CS0_MASK;
  GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;

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
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(PART1_STRING_START);

  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string(PART1_STRING_SEE_SERIAL);

  uint8_t spi_data = 0;
  char str[BUFFER_SIZE];

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
        spi_data = get_spi_data();
        break;
      case '2':
        lcd_clear();

        lcd_set_ddram_addr(LCD_LINE1_ADDR);
        lcd_write_string(PART1_STRING_SEND);
        uint16_to_string(spi_data, str);
        lcd_write_string(str);
        spi1_write_data(spi_data);

        lcd_set_ddram_addr(LCD_LINE2_ADDR);
        lcd_write_string(PART1_STRING_RECV);
        uint16_to_string(spi1_read_data(), str);
        lcd_write_string(str);
        break;
      case '3':
        GPIOB->DOUT31_0 |= LP_SPI_CS0_MASK;
        msec_delay(PART1_BLINK_DELAY);
        GPIOB->DOUT31_0 &= ~LP_SPI_CS0_MASK;
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

  lcd_clear();
  lcd_set_ddram_addr(LCD_LINE1_ADDR);
  lcd_write_string(PART1_STRING_END);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function stores the data that we want to send through spi from the UART
//  in functions.
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
uint8_t get_spi_data()
{
  msp_printf(PART1_STRING_XMIT);
  msp_printf(PART1_STRING_ENTER_VAL);

  char full_num[BUFFER_SIZE];
  uint8_t index = 0;
  do {
    char input = UART_in_char();
    UART_out_char(input);

    if(input == PART1_CHAR_RETURN)
    {
      full_num[index] = PART1_CHAR_END_STRING;
      index++;
    }
    else if (input == PART1_CHAR_BACKSPACE) 
    {
      index--;
    }
    else if (index < (sizeof(full_num) / sizeof(full_num[0])) - 1) {
      full_num[index] = input;
      index++;
    }
  }while (full_num[index - 1] != PART1_CHAR_END_STRING);

  msp_printf(PART1_STRING_NEW_LINE);

  uint16_t number = string_to_uint16(full_num);
  if(number <= UINT8_MAX)
  {
    msp_printf(PART1_STRING_SPI_SET);
    msp_printf(full_num);
    msp_printf(PART1_STRING_NEW_LINE);
    msp_printf(PART1_STRING_NEW_LINE);
  }
  else 
  {
    number = 0;

    msp_printf(PART1_STRING_NOT_VALID);
    msp_printf(PART1_STRING_SPI_SET);
    UART_out_char(PART1_CHAR_ZERO);
    msp_printf(PART1_STRING_NEW_LINE);
    msp_printf(PART1_STRING_NEW_LINE);
  }

  return (uint8_t)number;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function converts a uint16_t to a string
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
uint16_t string_to_uint16(char* string)
{
  uint16_t number = 0;
  uint16_t index = 0;
  while (string[index] != PART1_CHAR_END_STRING)
  {
    number = number * 10 + (string[index] - PART1_CHAR_ZERO);
    index++;
  } /* while */

  return number;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function converts a string to a uint16_t
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
void uint16_to_string(uint16_t number, char *buffer)
{
  // Handle the special case of 0
  if (number == 0)
  {
    buffer[0] = PART1_CHAR_ZERO;
    buffer[1] = PART1_CHAR_END_STRING;
  }
  else
  {
    // Start from the end of the buffer
    uint8_t index = BUFFER_SIZE - 1;
    buffer[index] = PART1_CHAR_END_STRING; // Null-terminate the string
    // Convert the number to string in reverse order 
    while (number > 0)
    {
      index--;
      // Convert last digit to character and stuff into buffer 
      buffer[index] = (number % 10) + PART1_CHAR_ZERO;
      // Remove the last digit and update number
      number /= 10;
    } /* while */
    
    // Loop above ended at index, so first character is at buffer[index]
    // so slide string to left in buffer as needed
    for (uint8_t i = index; i < BUFFER_SIZE; i++)
    {
      buffer[i - index] = buffer[i];
    } /* for */
  } /* else */
} /* uint16_to_string */
