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

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void init_PB1_irq();
void init_PB2_irq();

void disable_PB1_irq();
void disable_PB2_irq();

void run_lab9_part1();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PART1_STRING_SPEED                                         "MOTOR SPEED"
#define PART1_STRING_PERCENT                                                0x52
#define PART1_STRING_END                                       "Program Stopped"
#define PART1_NIBBLE_TO_PERCENT                                         (100/16)
#define PART1_DELAY                                                          100

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
  keypad_init();

  motor0_init();
  motor0_pwm_init(4000, 0);
  motor0_pwm_enable();

  run_lab9_part1();
 
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

void run_lab9_part1() 
{
  lcd_set_ddram_addr(LCD_LINE1_ADDR + LCD_CHAR_POSITION_3);
  lcd_write_string(PART1_STRING_SPEED);

  MOTOR_STATE state = MOTOR_OFF1;
  while (!g_PB1_Pressed) 
  {
    if(keypad_scan() != 0x10)
    {
      uint8_t speed = keypad_scan() * PART1_NIBBLE_TO_PERCENT;

      motor0_set_pwm_dc(speed);

      lcd_set_ddram_addr(LCD_LINE2_ADDR + LCD_CHAR_POSITION_6);
      lcd_write_byte(speed);
      lcd_write_char(PART1_STRING_PERCENT);

      wait_no_key_pressed();
    }

    if (g_PB2_Pressed) {
      switch (state) {
        case MOTOR_OFF1:
          motor0_pwm_disable();
          state = MOTOR_CW;
          break;
        case MOTOR_CW:
          motor0_pwm_enable();
          led_on(1);
          led_off(2);
          state = MOTOR_OFF2;
          break;
        case MOTOR_OFF2:
          motor0_pwm_disable();
          state = MOTOR_CCW;
          break;
        case MOTOR_CCW:
          motor0_pwm_enable();
          led_on(2);
          led_off(1);
          state = MOTOR_OFF1;
          break;
      }

      msec_delay(PART1_DELAY);
      g_PB2_Pressed = false;
    }
  }

  lcd_clear();
  lcd_write_string(PART1_STRING_END);
}
