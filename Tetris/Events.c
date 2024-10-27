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
//    
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
bool g_PB1_Pressed = false;
bool g_PB2_Pressed = false;

void init_PB1_irq()
{
  uint32_t gpio_pincm = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |
                    PINCM_GPIO_PIN_FUNC | IOMUX_PINCM_INV_ENABLE;
  IOMUX->SECCFG.PINCM[PB1_IOMUX] = gpio_pincm;

  GPIOA->POLARITY15_0 |= GPIO_POLARITY15_0_DIO11_RISE;
  GPIOA->CPU_INT.ICLR &= ~GPIO_CPU_INT_ICLR_DIO11_CLR;
  GPIOA->CPU_INT.IMASK |= GPIO_CPU_INT_IMASK_DIO11_SET;
}

void init_PB2_irq()
{
  uint32_t gpio_pincm = IOMUX_PINCM_INENA_ENABLE | IOMUX_PINCM_PC_CONNECTED |
                    PINCM_GPIO_PIN_FUNC | IOMUX_PINCM_INV_ENABLE;
  IOMUX->SECCFG.PINCM[PB2_IOMUX] = gpio_pincm;

  GPIOA->POLARITY15_0 |= GPIO_POLARITY15_0_DIO12_RISE;
  GPIOA->CPU_INT.ICLR &= ~GPIO_CPU_INT_ICLR_DIO12_CLR;
  GPIOA->CPU_INT.IMASK |= GPIO_CPU_INT_IMASK_DIO12_SET;
}

void init_events(void)
{
  init_PB1_irq();
  init_PB2_irq();

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
        
        if((gpio_mis & GPIO_CPU_INT_MIS_DIO11_MASK) == GPIO_CPU_INT_MIS_DIO11_SET)
        {
          g_PB1_Pressed = true;
          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO11_CLR;
        } /* if */
        else if ((gpio_mis & GPIO_CPU_INT_MIS_DIO12_MASK) == GPIO_CPU_INT_MIS_DIO12_SET) 
        {
          g_PB2_Pressed = true;
          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO12_CLR;
        }
        break;
    } /* switch */
  } while(group_iidx_status != 0);
} /* GROUP1_IRQHandler */

bool is_pb1_down(bool handled) 
{
  bool temp = g_PB1_Pressed;

  if(handled && g_PB1_Pressed) 
  {
    msec_delay(DEBOUNCE_DELAY);
    g_PB1_Pressed = false;
  }

  return temp;
}

bool is_pb2_down(bool handled) 
{
  bool temp = g_PB2_Pressed;

  if(handled && g_PB2_Pressed) 
  {
    msec_delay(DEBOUNCE_DELAY);
    g_PB2_Pressed = false;
  }

  return temp;
}
