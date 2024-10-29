// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  ST7735S.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    
// *****************************************************************************
//******************************************************************************

//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "Buzzer.h"

void pwm_init()
{
    // Reset TIMA0
    TIMA0->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
                            GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
                            GPTIMER_RSTCTL_RESETASSERT_ASSERT);

    // Enable power to TIMA0
    TIMA0->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | 
                            GPTIMER_PWREN_ENABLE_ENABLE);

    clock_delay(24);

    TIMA0->CLKSEL = (GPTIMER_CLKSEL_BUSCLK_SEL_ENABLE | 
                    GPTIMER_CLKSEL_MFCLK_SEL_DISABLE |  
                    GPTIMER_CLKSEL_LFCLK_SEL_DISABLE);

    TIMA0->CLKDIV = GPTIMER_CLKDIV_RATIO_DIV_BY_8;

    // set the pre-scale count value that divides select clock by PCNT+1
    // TimerClock = BusCock / (DIVIDER * (PRESCALER))
    // 200,000 Hz = 40,000,000 Hz / (8 * (24 + 1))
    TIMA0->COMMONREGS.CPS = GPTIMER_CPS_PCNT_MASK & 0x18;

    // Set action for compare
    // On Zero, set output HIGH; On Compares up, set output LOW
    TIMA0->COUNTERREGS.CCACT_23[0] = (GPTIMER_CCACT_23_FENACT_DISABLED | 
            GPTIMER_CCACT_23_CC2UACT_DISABLED | GPTIMER_CCACT_23_CC2DACT_DISABLED |
            GPTIMER_CCACT_23_CUACT_CCP_LOW | GPTIMER_CCACT_23_CDACT_DISABLED | 
            GPTIMER_CCACT_23_LACT_DISABLED | GPTIMER_CCACT_23_ZACT_CCP_HIGH);

    // set timer reload value
    TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & (100 - 1);

    // set timer compare value
    TIMA0->COUNTERREGS.CC_23[0] = GPTIMER_CC_23_CCVAL_MASK & 50;

    // set compare control for PWM func with output initially low
    TIMA0->COUNTERREGS.OCTL_23[0] = (GPTIMER_OCTL_23_CCPIV_LOW | 
                    GPTIMER_OCTL_23_CCPOINV_NOINV | GPTIMER_OCTL_23_CCPO_FUNCVAL);
    //
    TIMA0->COUNTERREGS.CCCTL_23[0] = GPTIMER_CCCTL_23_CCUPD_IMMEDIATELY;

    // When enabled load 0, set timer to count up
    TIMA0->COUNTERREGS.CTRCTL = GPTIMER_CTRCTL_CVAE_ZEROVAL | 
                    GPTIMER_CTRCTL_REPEAT_REPEAT_1 | GPTIMER_CTRCTL_CM_UP;
    TIMA0->COUNTERREGS.CTRCTL |= (GPTIMER_CTRCTL_EN_ENABLED);

    TIMA0->COMMONREGS.CCLKCTL = GPTIMER_CCLKCTL_CLKEN_ENABLED;

    // No interrupt is required
    TIMA0->CPU_INT.IMASK = GPTIMER_CPU_INT_IMASK_L_CLR;

    // set C0 as output
    TIMA0->COMMONREGS.CCPD = (GPTIMER_CCPD_C0CCP3_INPUT | 
            GPTIMER_CCPD_C0CCP2_OUTPUT | GPTIMER_CCPD_C0CCP1_INPUT | 
            GPTIMER_CCPD_C0CCP0_INPUT);
}

void buzzer_init(void) 
{
    IOMUX->SECCFG.PINCM[BUZZER_IOMUX] = IOMUX_PINCM17_PF_TIMA0_CCP2 | 
                                        IOMUX_PINCM_PC_CONNECTED;
    GPIOB->DOESET31_0 = BUZZER_MASK;
    
    pwm_init();
}

void buzzer_tone(uint16_t frequency)
{
    // clock freq is 20khz so 20,000 / frequency = desired frequency
    // set timer reload value
    TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & ((TIMA_CLOCK_FREQ/frequency) - 1);

    // set 50% duty cycle
    uint32_t threshold = (TIMA0->COUNTERREGS.LOAD * 50) / 100;
    TIMA0->COUNTERREGS.CC_23[0] = GPTIMER_CC_23_CCVAL_MASK & threshold;
}