//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 8
//
//      FILE NAME:  lab8p3_main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a test for the ADC
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
#include "adc.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void init_PB1_irq();
void init_PB2_irq();

void disable_PB1_irq();
void disable_PB2_irq();

void run_lab8_part3();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART3_STRING_ADC                                               "ADC = "
#define PART3_STRING_TEMP                                             "Temp = "
#define PART3_CHAR_FAHRENHEIT                                               'F'
#define PART3_CHAR_DEGREE                                                  0xDF
#define PART3_STRING_END                                      "Program Stopped"
#define PART3_CHANNEL_PHOTODIODE                                              7
#define PART3_CHANNEL_TEMP                                                    5
#define PART3_SECTION_DIVIDE                                         (4096 / 9)
#define PART3_FAHRENHEIT_CONVERSTION                             9.0 / 5.0 + 32


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_PB1_Pressed = false;
bool g_PB2_Pressed = false;

// Define a structure to hold different data types

int main(void)
{
  clock_init_40mhz();
  launchpad_gpio_init();

  I2C_init();
  lcd1602_init();

  led_init();
  led_enable();

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  dipsw_init();

  init_PB1_irq();
  init_PB2_irq();

  run_lab8_part3();

  disable_PB1_irq();
  disable_PB2_irq();
 
  // Endless loop to prevent program from ending
  while (1);

} /* main */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code configures an interrupt for GPIO pin DIO18 on GPIOB
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
void init_PB1_irq()
{
  GPIOB->POLARITY31_16 = GPIO_POLARITY31_16_DIO18_RISE;
  GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
  GPIOB->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO18_SET;

  NVIC_SetPriority(GPIOB_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOB_INT_IRQn);
} /* init_PB1_irq */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code configures an interrupt for GPIO pin DIO15 on GPIOA
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
void init_PB2_irq()
{
  GPIOA->POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;
  GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
} /* init_PB2_irq */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code disables all interrupts for GPIOB
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
void disable_PB1_irq()
{
  NVIC_DisableIRQ(GPIOB_INT_IRQn);
} /* disable_PB1_irq */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code disables all interrupts for GPIOA
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
void disable_PB2_irq()
{
  NVIC_DisableIRQ(GPIOA_INT_IRQn);
} /* disable_PB2_irq */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code snippet handles interrupts from two GPIO groups (GPIOA and GPIOB) 
//  in a loop, checking for specific interrupt statuses.
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
void GROUP1_IRQHandler(void)
{
  uint32_t group_iidx_status;
  uint32_t gpio_mis;

  do
  {
    group_iidx_status = CPUSS->INT_GROUP[1].IIDX;

    switch (group_iidx_status) 
    {
      case (CPUSS_INT_GROUP_IIDX_STAT_INT0): //GPIOA
        gpio_mis = GPIOA->CPU_INT.MIS;
        if((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) == GPIO_CPU_INT_MIS_DIO15_SET)
        {
          g_PB2_Pressed = true;
          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        } /* if */
        break;

      case (CPUSS_INT_GROUP_IIDX_STAT_INT1): //GPIOB
        gpio_mis = GPIOB->CPU_INT.MIS;
        if((gpio_mis & GPIO_CPU_INT_MIS_DIO18_MASK) == GPIO_CPU_INT_MIS_DIO18_SET)
        {
          g_PB1_Pressed = true;
          GPIOB->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO18_CLR;
        } /* if */
        break;
    } /* switch */
  } while(group_iidx_status != 0);
} /* GROUP1_IRQHandler */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code reads an ADC value from a photodiode, displays it on an LCD, and 
//  lights up LEDs based on the value's section. If a second button is pressed, 
//  it reads a temperature sensor, converts the temperature to Fahrenheit, and 
//  updates the LCD display with the temperature. This loop continues until a 
//  button is pressed, after which the display is cleared and LEDs are disabled.
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
void run_lab8_part3() {
  float fahrenheit_temp = 0;

  while (!g_PB1_Pressed) {
    uint16_t value = ADC0_in(PART3_CHANNEL_PHOTODIODE);

    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string(PART3_STRING_ADC);
    lcd_write_doublebyte(value);

    leds_off();
    int section = value / PART3_SECTION_DIVIDE;
    for(uint8_t i = 0; i < section; i++) {
      led_on(i);
    } /* for */

    if(g_PB2_Pressed) {
      uint16_t temp_value = ADC0_in(PART3_CHANNEL_TEMP);
      float celsius_temp = thermistor_calc_temperature(temp_value);
      fahrenheit_temp = celsius_temp * PART3_FAHRENHEIT_CONVERSTION;

      g_PB2_Pressed = false;
    } /* if */

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string(PART3_STRING_TEMP);
    lcd_write_byte(fahrenheit_temp);
    lcd_write_char(PART3_CHAR_DEGREE);
    lcd_write_char(PART3_CHAR_FAHRENHEIT);
  } /* while */

  lcd_clear();
  lcd_write_string(PART3_STRING_END);
  led_disable();
} /* run_lab8_part3 */
