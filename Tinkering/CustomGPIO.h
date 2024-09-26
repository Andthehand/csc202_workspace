// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  CustomGPIO.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    Made for Educational use to light a led.
//
//
//
//******************************************************************************
//******************************************************************************

#ifndef __CustomGPIO_H__
#define __CustomGPIO_H__

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
//IOMUX PINCM definitions
#include <ti/devices/msp/peripherals/hw_iomux.h>

// see ti_devices_msp_peripherals_hw_iomux__include hw_iomux.h
#define IOMUX_PINCM_PF_GPIO                               ((uint32_t)0x00000001)
#define IOMUX_PINCM_CONNECTED                           IOMUX_PINCM_PC_CONNECTED

// Defines for GPIO
#define GPIO_PORTA                                                  ((uint8_t)0)
#define GPIO_PORTB                                                  ((uint8_t)1)

//Defines all enable pin indexes
#define LED_ENABLE_PINCM                                         (IOMUX_PINCM40)
#define LED_ENABLE_OFFSET                                              (1 << 18)

// Defines for LEDs on MSPM0G3507 LaunchPad board
#define LED0_PINCM                                                (IOMUX_PINCM3)
#define LED0_GPIO_OFFSET                                               (1 << 28)
#define LED0_GPIO_PORT                                                GPIO_PORTA

#define LED1_PINCM                                                (IOMUX_PINCM6)
#define LED1_GPIO_OFFSET                                               (1 << 31)
#define LED1_GPIO_PORT                                                GPIO_PORTA

#define LED2_PINCM                                               (IOMUX_PINCM48)
#define LED2_GPIO_OFFSET                                               (1 << 20)
#define LED2_GPIO_PORT                                                GPIO_PORTB

#define LED3_PINCM                                               (IOMUX_PINCM30)
#define LED3_GPIO_OFFSET                                               (1 << 13)
#define LED3_GPIO_PORT                                                GPIO_PORTB

#define LED4_PINCM                                               (IOMUX_PINCM34)
#define LED4_GPIO_OFFSET                                               (1 << 12)
#define LED4_GPIO_PORT                                                GPIO_PORTA

#define LED5_PINCM                                               (IOMUX_PINCM35)
#define LED5_GPIO_OFFSET                                               (1 << 13)
#define LED5_GPIO_PORT                                                GPIO_PORTA

#define LED6_PINCM                                               (IOMUX_PINCM38)
#define LED6_GPIO_OFFSET                                               (1 << 16)
#define LED6_GPIO_PORT                                                GPIO_PORTA

#define LED7_PINCM                                               (IOMUX_PINCM39)
#define LED7_GPIO_OFFSET                                               (1 << 17)
#define LED7_GPIO_PORT                                                GPIO_PORTA


//Custom structs
typedef struct {
  IOMUX_PINCM pin_cm;
  uint32_t    offset;
  uint8_t     port;
  bool        polarity;
} gpio_struct;

// --------------------------------------------------------------------------
// Prototype for CustomGPIO support functions
// --------------------------------------------------------------------------
void gpio_init();
void init_leds();

void led_on(uint8_t index);
void leds_on(uint8_t value);

#endif // __CustomGPIO_H__
