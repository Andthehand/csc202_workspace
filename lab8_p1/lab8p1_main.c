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
#include "lcd1602.h"
#include "adc.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void OPA0_init(void);

void init_PB1_irq();
void disable_PB1_irq();

void run_lab8_part1();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define LIGHT_SENSOR_ADC_CHANNEL                                              7
#define LIGHT_SENSOR_DARK_VALUE                                              35

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

  I2C_init();
  lcd1602_init();

  OPA0_init();
  ADC0_init(ADC12_MEMCTL_VRSEL_VDDA_VSSA);
 
  dipsw_init();
  init_PB1_irq();

  run_lab8_part1();

  disable_PB1_irq();

  // Endless loop to prevent program from ending
  while (1);

} /* main */

void OPA0_init(void) {
  OPA0->GPRCM.RSTCTL = (OA_RSTCTL_KEY_UNLOCK_W | OA_RSTCTL_RESETSTKYCLR_CLR |
                        OA_RSTCTL_RESETASSERT_ASSERT);

  OPA0->GPRCM.PWREN = (OA_PWREN_KEY_UNLOCK_W | OA_PWREN_ENABLE_ENABLE);

  // time for OPA to power up
  clock_delay(24);

  OPA0->CFGBASE &= ~(OA_CFGBASE_RRI_MASK);

  OPA0->CFGBASE |= ((uint32_t) OA_CFGBASE_GBW_HIGHGAIN);

  OPA0->CFG |= (OA_CFG_GAIN_MINIMUM | OA_CFG_MSEL_NC | OA_CFG_NSEL_EXTPIN0 |
               OA_CFG_PSEL_EXTPIN0 | OA_CFG_OUTPIN_ENABLED | OA_CFG_CHOP_OFF);
               
  OPA0->CTL |= OA_CTL_ENABLE_ON;
}


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

void run_lab8_part1() {
  const char* status_string = "Status: ";
  const char* dark_string   = "Dark ";
  const char* light_string  = "Light";

  const char* ADC_string    = "ADC = ";

  while (!g_PB1_Pressed) {
    uint16_t light_value = ADC0_in(LIGHT_SENSOR_ADC_CHANNEL);

    lcd_set_ddram_addr(LCD_LINE1_ADDR);
    lcd_write_string(status_string);
    if(light_value < LIGHT_SENSOR_DARK_VALUE) 
    {
      lcd_write_string(dark_string);
    } 
    else 
    {
      lcd_write_string(light_string);
    }    

    lcd_set_ddram_addr(LCD_LINE2_ADDR);
    lcd_write_string(ADC_string);
    lcd_write_doublebyte(light_value);
  }

  lcd_clear();
  lcd_write_string("Program Stopped");

}
