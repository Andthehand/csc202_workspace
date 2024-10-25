// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.2
//
//       FILE NAME:  adc.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    This file contains functions for configuring and managing the Analog-to-
//    Digital Converter (ADC) on the LP-MSPM0G3507 LaunchPad and the CSC202 
//    Expansion Board. The functions provided in this file facilitate ADC 
//    setup, sample acquisition, and temperature sensor calculations for 
//    embedded systems applications.
//
//    The header comments for each function describe its purpose, input
//    parameters, output parameters, and return values.
//
//    This code is based on Texas Instruments' LaunchPad project template for
//    the LP-MSPM0G3507, tailored for use with no RTOS and written in C.
//
// NOTES:
//    - Ensure that `ADC0_init` is called before `ADC0_in` to configure the ADC 
//      for sampling and conversion tasks. Proper initialization is necessary 
//      for accurate ADC operations.
//
//    - The `thermistor_calc_temperature` function requires a raw ADC value from 
//      the TMP61 sensor to compute the temperature. Make sure to configure the 
//      sensor and ADC correctly for accurate temperature measurements.
//
//    - Be aware of the potential for endless loops in `ADC0_init` and `ADC0_in` 
//      if the hardware status flags do not behave as expected.
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    This code was developed for educational purposes as part of the CSC202 
//    course and is provided "as is" without warranties of any kind, whether 
//    express, implied, or statutory.
//
//    The author and organization do not warrant the accuracy, completeness, or
//    reliability of the code. The author and organization shall not be liable
//    for any direct, indirect, incidental, special, exemplary, or consequential
//    damages arising out of the use of or inability to use the code, even if
//    advised of the possibility of such damages.
//
//    Use of this code is at your own risk, and it is recommended to validate
//    and adapt the code for your specific application and hardware requirements.
//
// Copyright (c) 2024 by TBD
//    You may use, edit, run or distribute this file as long as the above
//    copyright notice remains
// *****************************************************************************
//******************************************************************************

#ifndef __ADC_H__
#define __ADC_H__

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
} joystick_pos;

// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void joystick_init(void);
void ADC0_init();
void ADC1_init();

uint32_t ADC0_in(uint8_t channel);
uint32_t ADC1_in(uint8_t channel);

joystick_pos read_joystick();

#endif /* __ADC_H__ */