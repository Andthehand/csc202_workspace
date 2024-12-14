// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  Joystick.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file is used to initialize 2 ADCs to read both the x and y positions
//    of the Joystick. Currently in the Game of tetris I do not use the y pos or
//    the button on the Joystick.
//
// NOTES:
//    
//
// *****************************************************************************
//******************************************************************************

#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#define JOYSTICK_BUTTON_AXIS_PORT                                   (GPIO_PORTA)
#define JOYSTICK_BUTTON_AXIS_MASK                                      (1 << 26)
#define JOYSTICK_BUTTON_AXIS_IOMUX                               (IOMUX_PINCM59)

#define JOYSTICK_Y_AXIS_CHANNEL                                              (2)
#define JOYSTICK_X_AXIS_CHANNEL                                              (3)

#define JOYSTICK_CENTER_VALUE                                       ((2^12) / 2)
#define JOYSTICK_DEADZONE_VALUE                                              200

typedef struct {
  uint16_t x_pos;
  uint16_t y_pos;
} Joystick_pos;

// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void joystick_init(void);
void ADC0_init();
void ADC1_init();

uint32_t ADC0_in(uint8_t channel);
uint32_t ADC1_in(uint8_t channel);

Joystick_pos read_joystick();

#endif /* __ADC_H__ */
