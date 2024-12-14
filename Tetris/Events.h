// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  Event.h
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

#ifndef __ADC_H__
#define __ADC_H__

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#define PB1_PORT                                                    (GPIO_PORTA)
#define PB1_MASK                                                       (1 << 11)
#define PB1_IOMUX                                                (IOMUX_PINCM22)
#define PB2_PORT                                                    (GPIO_PORTA)
#define PB2_MASK                                                       (1 << 12)
#define PB2_IOMUX                                                (IOMUX_PINCM34)

#define PINCM_GPIO_PIN_FUNC                              ((uint32_t)0x00000001U)

// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------
void init_events(void);

bool is_s1_down();
bool has_s2_pressed();

#endif /* __ADC_H__ */
