//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 8
//
//      FILE NAME:  lab8p2_main.c
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
void disable_PB1_irq();

void run_lab8_part2();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART2_STRING_ADC                                               "ADC = "
#define PART2_STRING_END                                      "Program Stopped"
#define PART2_CHANNEL                                                         7
#define PART2_SECTION_DIVIDE                                         (4096 / 9)

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_PB1_Pressed = false;

// Define a structure to hold different data types

int main(void)
{
  clock_init_40mhz();
  launchpad_gpio_init();

  // I2C_init();
  // lcd1602_init();

  led_init();
  led_enable();

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  dipsw_init();

  init_PB1_irq();
  run_lab8_part2();
  disable_PB1_irq();
 
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
} /* init_PB2_irq */

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
// ----------------------------------------------------------------------------
void GROUP1_IRQHandler(void)
{
  uint32_t group_iidx_status;
  uint32_t gpio_mis;

  do
  {
    group_iidx_status = CPUSS->INT_GROUP[1].IIDX;

    switch (group_iidx_status) 
    {
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
} /* disable_PB2_irq */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code reads a value from an ADC channel and displays it on an LCD, then 
//  turns on a corresponding number of LEDs based on the value's section. It 
//  continues this process until a button is pressed, after which it clears the 
//  display, shows an end message, and disables the LEDs.
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
void run_lab8_part2() {
  while (!g_PB1_Pressed) {
    uint16_t value = ADC0_in(PART2_CHANNEL);

    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string(PART2_STRING_ADC);
    lcd_write_doublebyte(value);

    leds_off();
    int section = value / PART2_SECTION_DIVIDE;
    for(uint8_t i = 0; i < section; i++) {
      led_on(i);
    } /* for */
  } /* while */

  lcd_clear();
  lcd_write_string(PART2_STRING_END);
  led_disable();
} /* run_lab8_part2 */
