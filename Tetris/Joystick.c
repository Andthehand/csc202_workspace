// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Bruce Link
//
//         VERSION:  0.2
//
//       FILE NAME:  adc.c
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


//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <math.h>
#include <stdint.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "ti/devices/msp/peripherals/hw_adc12.h"
#include "clock.h"
#include "Joystick.h"

void joystick_init(void) 
{
  ADC0_init();
  ADC1_init();
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function initializes the ADC0 peripheral for a single channel 
//   and reference voltage. It configures the ADC clock, control registers, 
//   and conversion memory to prepare the ADC for single-ended analog-to-digital 
//   conversions.
//
//   The initialization process includes the following steps:
//   - Resetting the ADC and VREF peripherals (if required)
//   - Enabling power to the ADC and VREF peripherals
//   - Configuring the ADC clock source and frequency
//   - Setting the ADC control registers for single-ended conversions
//   - Configuring the conversion memory control register for the specified 
//     channel
//   - Setting the sample time for the ADC conversions
//
//   Note: This function does not start any conversions. It only sets up the ADC
//   for future use based on the specified parameters.
//
// INPUT PARAMETERS:
//   reference - The reference voltage for the ADC. This can be set to a 
//               specific reference source (e.g., internal reference or 
//               external VDD) based on the desired measurement accuracy 
//               and configuration.
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void ADC0_init()
{
  // Reset ADC and VREF
  ADC0->ULLMEM.GPRCM.RSTCTL = (ADC12_RSTCTL_KEY_UNLOCK_W | 
                               ADC12_RSTCTL_RESETSTKYCLR_CLR | 
                               ADC12_RSTCTL_RESETASSERT_ASSERT);
  
  // Enable power ADC and VREF
  ADC0->ULLMEM.GPRCM.PWREN = (ADC12_PWREN_KEY_UNLOCK_W |
                              ADC12_PWREN_ENABLE_ENABLE);
  
  clock_delay(24); // time for ADC and VREF to power up
  
  // Set ADC clock configuration
  ADC0->ULLMEM.GPRCM.CLKCFG = (ADC12_CLKCFG_KEY_UNLOCK_W | 
                               ADC12_CLKCFG_CCONSTOP_DISABLE | 
                               ADC12_CLKCFG_CCONRUN_DISABLE | 
                               ADC12_CLKCFG_SAMPCLK_ULPCLK); 

  // Set sampling clock frequency range
  ADC0->ULLMEM.CLKFREQ = ADC12_CLKFREQ_FRANGE_RANGE40TO48;
  
  // Configure ADC Control Register 0
  ADC0->ULLMEM.CTL0 = ADC12_CTL0_SCLKDIV_DIV_BY_8 | ADC12_CTL0_PWRDN_MANUAL |
                      ADC12_CTL0_ENC_OFF;

  // Configure Sample Time Compare 0 Register
  ADC0->ULLMEM.SCOMP0 = 0; // 8 sample clocks
} /* ADC0_init */

void ADC1_init()
{
  // Reset ADC and VREF
  ADC1->ULLMEM.GPRCM.RSTCTL = (ADC12_RSTCTL_KEY_UNLOCK_W | 
                               ADC12_RSTCTL_RESETSTKYCLR_CLR | 
                               ADC12_RSTCTL_RESETASSERT_ASSERT);
  
  // Enable power ADC and VREF
  ADC1->ULLMEM.GPRCM.PWREN = (ADC12_PWREN_KEY_UNLOCK_W |
                              ADC12_PWREN_ENABLE_ENABLE);
  
  clock_delay(24); // time for ADC and VREF to power up
  
  // Set ADC clock configuration
  ADC1->ULLMEM.GPRCM.CLKCFG = (ADC12_CLKCFG_KEY_UNLOCK_W | 
                               ADC12_CLKCFG_CCONSTOP_DISABLE | 
                               ADC12_CLKCFG_CCONRUN_DISABLE | 
                               ADC12_CLKCFG_SAMPCLK_ULPCLK); 

  // Set sampling clock frequency range
  ADC1->ULLMEM.CLKFREQ = ADC12_CLKFREQ_FRANGE_RANGE40TO48;
  
  // Configure ADC Control Register 0
  ADC1->ULLMEM.CTL0 = ADC12_CTL0_SCLKDIV_DIV_BY_8 | ADC12_CTL0_PWRDN_MANUAL |
                      ADC12_CTL0_ENC_OFF;

  // Configure Sample Time Compare 0 Register
  ADC1->ULLMEM.SCOMP0 = 0; // 8 sample clocks
} /* ADC0_init */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//   This function starts an ADC conversion on the ADC0 peripheral and waits 
//   for the conversion to complete. It then reads the conversion result from 
//   the ADC result register and returns it.
//
//   The function performs the following steps:
//   - Starts the ADC conversion by setting the ENC bit in the CTL0 register.
//   - Triggers the conversion process by setting the SC bit in the CTL1 register.
//   - Waits for the conversion to complete by checking the BUSY bit in the 
//     STATUS register.
//   - Reads the result of the conversion from the MEMRES[0] register and 
//     returns it.
//
//   This function assumes that the ADC has been properly initialized using
//   the `ADC0_init` function before calling this function.
//
// INPUT PARAMETERS:
//   channel  - The ADC input channel to be used for the conversion. This
//               parameter specifies which input pin the ADC will sample from.
//
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   uint32_t - The result of the ADC conversion. This value represents the 
//              digital output corresponding to the analog input signal.
// -----------------------------------------------------------------------------
uint32_t ADC0_in(uint8_t channel)
{
  // Configure ADC Control Register 1
  ADC0->ULLMEM.CTL1 = (ADC12_CTL1_AVGD_SHIFT0 | ADC12_CTL1_AVGN_DISABLE |
                       ADC12_CTL1_SAMPMODE_AUTO | ADC12_CTL1_CONSEQ_SINGLE |
                       ADC12_CTL1_SC_STOP | ADC12_CTL1_TRIGSRC_SOFTWARE);
                       
  // Configure ADC Control Register 2
  ADC0->ULLMEM.CTL2 = (ADC12_CTL2_ENDADD_ADDR_00 | ADC12_CTL2_STARTADD_ADDR_00 |
                       ADC12_CTL2_SAMPCNT_MIN | ADC12_CTL2_FIFOEN_DISABLE |
                       ADC12_CTL2_DMAEN_DISABLE | ADC12_CTL2_RES_BIT_12 |
                       ADC12_CTL2_DF_UNSIGNED);

  // Configure Conversion Memory Control Register
  ADC0->ULLMEM.MEMCTL[0] =  ADC12_MEMCTL_WINCOMP_DISABLE | 
                      ADC12_MEMCTL_TRIG_AUTO_NEXT | ADC12_MEMCTL_BCSEN_DISABLE | 
                      ADC12_MEMCTL_AVGEN_DISABLE | ADC12_MEMCTL_STIME_SEL_SCOMP0 | 
                      ADC12_MEMCTL_VRSEL_VDDA_VSSA | channel;

  ADC0->ULLMEM.CTL0 |= ADC12_CTL0_ENC_ON;
  ADC0->ULLMEM.CTL1 |= ADC12_CTL1_SC_START; 
  
  clock_delay(2); // TODO: required for 80Mhz clock to work TBR TODO:

  volatile uint32_t *status_reg = (volatile uint32_t *)&(ADC0->ULLMEM.STATUS);

  // wait here until the conversion completes
  while((*status_reg & ADC12_STATUS_BUSY_MASK) == ADC12_STATUS_BUSY_ACTIVE);
  
  return ADC0->ULLMEM.MEMRES[0];

} /* ADC0_in */

uint32_t ADC1_in(uint8_t channel)
{
  // Configure ADC Control Register 1
  ADC1->ULLMEM.CTL1 = (ADC12_CTL1_AVGD_SHIFT0 | ADC12_CTL1_AVGN_DISABLE |
                       ADC12_CTL1_SAMPMODE_AUTO | ADC12_CTL1_CONSEQ_SINGLE |
                       ADC12_CTL1_SC_STOP | ADC12_CTL1_TRIGSRC_SOFTWARE);
                       
  // Configure ADC Control Register 2
  ADC1->ULLMEM.CTL2 = (ADC12_CTL2_ENDADD_ADDR_00 | ADC12_CTL2_STARTADD_ADDR_00 |
                       ADC12_CTL2_SAMPCNT_MIN | ADC12_CTL2_FIFOEN_DISABLE |
                       ADC12_CTL2_DMAEN_DISABLE | ADC12_CTL2_RES_BIT_12 |
                       ADC12_CTL2_DF_UNSIGNED);

  // Configure Conversion Memory Control Register
  ADC1->ULLMEM.MEMCTL[0] =  ADC12_MEMCTL_WINCOMP_DISABLE | 
                      ADC12_MEMCTL_TRIG_AUTO_NEXT | ADC12_MEMCTL_BCSEN_DISABLE | 
                      ADC12_MEMCTL_AVGEN_DISABLE | ADC12_MEMCTL_STIME_SEL_SCOMP0 | 
                      ADC12_MEMCTL_VRSEL_VDDA_VSSA | channel;

  ADC1->ULLMEM.CTL0 |= ADC12_CTL0_ENC_ON;
  ADC1->ULLMEM.CTL1 |= ADC12_CTL1_SC_START; 
  
  clock_delay(2); // TODO: required for 80Mhz clock to work TBR TODO:

  volatile uint32_t *status_reg = (volatile uint32_t *)&(ADC1->ULLMEM.STATUS);

  // wait here until the conversion completes
  while((*status_reg & ADC12_STATUS_BUSY_MASK) == ADC12_STATUS_BUSY_ACTIVE);
  
  return ADC1->ULLMEM.MEMRES[0];

} /* ADC0_in */

joystick_pos read_joystick() 
{
  joystick_pos pos;
  pos.x_pos = ADC1_in(JOYSTICK_X_AXIS_CHANNEL);
  pos.y_pos = ADC0_in(JOYSTICK_Y_AXIS_CHANNEL);

  return pos;
}
