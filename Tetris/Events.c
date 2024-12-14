// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  Event.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file is used for both input pulling and setting up interupts for both
//    Switch 1 and Switch 2 which despite what they sound like are both buttons.
//  
// NOTES:
//    
//
// *****************************************************************************
//******************************************************************************


//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <math.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "Events.h"

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_S2_Pressed = false;

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Configures the PB1 GPIO pin for input with pull-up/down functionality, 
//  inversion, and proper connectivity via the IOMUX module.
//
// INPUT PARAMETERS: 
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void init_s1()
{
  uint32_t gpio_pincm = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |
                    PINCM_GPIO_PIN_FUNC | IOMUX_PINCM_INV_ENABLE;
  IOMUX->SECCFG.PINCM[PB1_IOMUX] = gpio_pincm;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Configures the PB2 GPIO pin for input with interrupt capabilities. Sets up 
//  the interrupt polarity, clears any pending interrupts, and enables the mask 
//  for CPU interrupt handling.
//
// INPUT PARAMETERS: 
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void init_s2_irq()
{
  uint32_t gpio_pincm = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |
                    PINCM_GPIO_PIN_FUNC | IOMUX_PINCM_INV_ENABLE;
  IOMUX->SECCFG.PINCM[PB2_IOMUX] = gpio_pincm;

  GPIOA->POLARITY15_0 |= GPIO_POLARITY15_0_DIO12_RISE;
  GPIOA->CPU_INT.ICLR &= ~GPIO_CPU_INT_ICLR_DIO12_CLR;
  GPIOA->CPU_INT.IMASK |= GPIO_CPU_INT_IMASK_DIO12_SET;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Initializes the events by configuring the S1 button input and S2 button 
//  interrupt. Sets the interrupt priority and enables the interrupt for GPIOA.
//
// INPUT PARAMETERS: 
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void init_events(void)
{
  init_s1();
  init_s2_irq();

  NVIC_SetPriority(GPIOA_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This code snippet can handle interrupts from two GPIO groups (GPIOA and GPIOB) 
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
        
        if ((gpio_mis & GPIO_CPU_INT_MIS_DIO12_MASK) == GPIO_CPU_INT_MIS_DIO12_SET) 
        {
          g_S2_Pressed = true;
          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO12_CLR;
        } /* if */
        break;
    } /* switch */
  } while(group_iidx_status != 0);
} /* GROUP1_IRQHandler */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Checks if the S1 button (PB1) is pressed (logic low) by reading the GPIO input.
//
// INPUT PARAMETERS: 
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  true if S1 is pressed, false otherwise
//-----------------------------------------------------------------------------
bool is_s1_down() 
{
  return GPIOA->DIN31_0 & PB1_MASK;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Checks if the S2 button was pressed by reading a flag. Resets the flag 
//  after checking its state.
//
// INPUT PARAMETERS: 
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  true if S2 was pressed, false otherwise
//-----------------------------------------------------------------------------
bool has_s2_pressed() 
{
  bool temp = g_S2_Pressed;
  g_S2_Pressed = false;

  return temp;
}
