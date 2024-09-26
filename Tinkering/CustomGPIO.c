//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  None
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
#include <stdint.h>
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "CustomGPIO.h"
#include "clock.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void set_gpio_on(const gpio_struct pin);
void set_gpio_off(const gpio_struct pin);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define PERIPHERIAL_PWR_UP_DELAY                       24
#define ACTIVE_LOW                                      0
#define ACTIVE_HIGH                                     1

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold different data types
const gpio_struct led_config[] = {
  {LED0_PINCM, LED0_GPIO_OFFSET, LED0_GPIO_PORT, ACTIVE_LOW},
  {LED1_PINCM, LED1_GPIO_OFFSET, LED1_GPIO_PORT, ACTIVE_LOW},
  {LED2_PINCM, LED2_GPIO_OFFSET, LED2_GPIO_PORT, ACTIVE_LOW},
  {LED3_PINCM, LED3_GPIO_OFFSET, LED3_GPIO_PORT, ACTIVE_LOW},
  {LED4_PINCM, LED4_GPIO_OFFSET, LED4_GPIO_PORT, ACTIVE_LOW},
  {LED5_PINCM, LED5_GPIO_OFFSET, LED5_GPIO_PORT, ACTIVE_LOW},
  {LED6_PINCM, LED6_GPIO_OFFSET, LED6_GPIO_PORT, ACTIVE_LOW},
  {LED7_PINCM, LED7_GPIO_OFFSET, LED7_GPIO_PORT, ACTIVE_LOW}
};

void set_gpio_on(const gpio_struct pin) 
{
  if(pin.port == GPIO_PORTA)
  {
    if(pin.polarity == ACTIVE_HIGH) 
    {
      GPIOA->DOUTSET31_0 = pin.offset;
    }
    else 
    {
      GPIOA->DOUTCLR31_0 = pin.offset;
    }
  }
  else 
  {
    if(pin.polarity == ACTIVE_HIGH) 
    {
      GPIOB->DOUTSET31_0 = pin.offset;
    }
    else 
    {
      GPIOB->DOUTCLR31_0 = pin.offset;
    }
  }

}

void set_gpio_off(const gpio_struct pin)
{
  if(pin.port == GPIO_PORTA)
  {
    if(pin.polarity == ACTIVE_HIGH) 
    {
      GPIOA->DOUTCLR31_0  &= ~pin.offset;
    }
    else 
    {
      GPIOA->DOUTSET31_0 |= pin.offset;
    }
  }
  else
  {
    if(pin.polarity == ACTIVE_HIGH) 
    {
      GPIOB->DOUTCLR31_0  &= ~pin.offset;
    }
    else 
    {
      GPIOB->DOUTSET31_0 |= pin.offset;
    }
  }

}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function initializes the GPIO peripherals on the MSPM0G3507
//    LaunchPad. It performs a reset of GPIO Ports A and B by writing to
//    their reset control registers. Then, it enables power to these GPIO
//    peripherals by setting the power enable registers. Finally, it
//    introduces a small delay to ensure that the GPIO peripherals have
//    sufficient time to power up.
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
void gpio_init()
{
  // Reset two GPIO peripherals
  GPIOA->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W |
                         GPIO_RSTCTL_RESETSTKYCLR_CLR |
                         GPIO_RSTCTL_RESETASSERT_ASSERT);

  GPIOB->GPRCM.RSTCTL = (GPIO_RSTCTL_KEY_UNLOCK_W |
                         GPIO_RSTCTL_RESETSTKYCLR_CLR |
                         GPIO_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to two GPIO peripherals
  GPIOA->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);
  GPIOB->GPRCM.PWREN = (GPIO_PWREN_KEY_UNLOCK_W | GPIO_PWREN_ENABLE_ENABLE);

  // Provide a small delay for gpio to power up
  clock_delay(PERIPHERIAL_PWR_UP_DELAY);

} /* launchpad_gpio_init */

void init_leds() {
  uint32_t iomux_pincm = IOMUX_PINCM_PC_CONNECTED | IOMUX_PINCM_PF_GPIO;

  uint8_t array_size = sizeof(led_config)/sizeof(gpio_struct);
  for(uint8_t i = 0; i < array_size; i++) 
  {
    IOMUX->SECCFG.PINCM[led_config[i].pin_cm] |= iomux_pincm;

    if(led_config[i].port == GPIO_PORTA) 
    {
      GPIOA->DOE31_0  |= led_config[i].offset;
      //Set to off
      set_gpio_off(led_config[i]);
    }
    else 
    {
      GPIOB->DOE31_0  |= led_config[i].offset;
      //Set to off
      set_gpio_off(led_config[i]);
    }

  }

  //Set Led enable pin
  IOMUX->SECCFG.PINCM[LED_ENABLE_PINCM] |= iomux_pincm;
  GPIOA->DOE31_0  |= LED_ENABLE_OFFSET;
  GPIOA->DOUT31_0 |= LED_ENABLE_OFFSET;
}

void led_on(uint8_t index) 
{
  set_gpio_on(led_config[index]);
}

void leds_on(uint8_t value)
{

}