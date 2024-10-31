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

void run_lab9_part3();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART3_STRING_SPEED                                         "MOTOR SPEED"
#define PART3_STRING_END                                       "Program Stopped"
#define PART3_STRING_ADC                                           "ADC VALUE ="
#define PART3_STRING_SERVO                                         "SERVO CNT ="
#define PART3_NIBBLE_TO_PERCENT                                         (100/16)
#define PART3_DELAY                                                          250
#define PART3_CHANNEL                                                          7

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_PB1_Pressed = false;
bool g_PB2_Pressed = false;

// Define a structure to hold different data types
typedef enum {
  MOTOR_OFF1,
  MOTOR_CW,
  MOTOR_OFF2,
  MOTOR_CCW
} MOTOR_STATE;

int main(void)
{
  clock_init_40mhz();
  launchpad_gpio_init();

  lcd1602_init();

  dipsw_init();

  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);

  I2C_init();
  lcd1602_init();

  motor0_init();
  motor0_pwm_init(4000, 0);
  motor0_pwm_enable();

  init_PB1_irq();
  init_PB2_irq();

  run_lab9_part3();

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

void run_lab9_part3() 
{
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_3);
  lcd_write_string(PART3_STRING_SPEED);

  led_on(1);
  led_off(2);

  MOTOR_STATE state = MOTOR_OFF2;
  while (!g_PB1_Pressed) 
  {
    uint16_t value = ADC0_in(PART3_CHANNEL);

    value >>= 2;

    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string(PART3_STRING_ADC);
    lcd_write_doublebyte(value);

    uint16_t servo = ((value * (500 - 100))/1024) + 100;

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string(PART3_STRING_SERVO);
    lcd_write_doublebyte(servo);

    motor0_set_pwm_count(servo);
  }

  motor0_pwm_disable();
  led_disable();

  lcd_clear();
  lcd_write_string(PART3_STRING_END);
}
