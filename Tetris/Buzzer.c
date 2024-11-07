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
#include "stdlib.h"

#include <ti/devices/msp/msp.h>
#include "clock.h"
#include "ti/devices/msp/m0p/mspm0g350x.h"
#include "ti/devices/msp/peripherals/hw_gptimer.h"
#include "Buzzer.h"
#include "pitches.h"

#define TEMPO 144
#define WHOLE_NOTE (60000 * 4) / TEMPO
#define REST 34464

int melody[] = {
  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,

  NOTE_E5, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_C5,8,  NOTE_B4,8,
  NOTE_A4, 4,  NOTE_A4,8,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, -4,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,8,  NOTE_A4,4,  NOTE_B4,8,  NOTE_C5,8,

  NOTE_D5, -4,  NOTE_F5,8,  NOTE_A5,4,  NOTE_G5,8,  NOTE_F5,8,
  NOTE_E5, -4,  NOTE_C5,8,  NOTE_E5,4,  NOTE_D5,8,  NOTE_C5,8,
  NOTE_B4, 4,  NOTE_B4,8,  NOTE_C5,8,  NOTE_D5,4,  NOTE_E5,4,
  NOTE_C5, 4,  NOTE_A4,4,  NOTE_A4,4, REST, 4,
  

  NOTE_E5,2,  NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,2,   NOTE_A4,2,
  NOTE_GS4,2,  NOTE_B4,4,  REST,8, 
  NOTE_E5,2,   NOTE_C5,2,
  NOTE_D5,2,   NOTE_B4,2,
  NOTE_C5,4,   NOTE_E5,4,  NOTE_A5,2,
  NOTE_GS5,2,
};

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    This function is used to disable the SysTick timer when it is no longer 
//    needed or before reconfiguring the SysTick settings. The function also 
//    clears the current value register to ensure that no pending SysTick 
//    interrupts are triggered.
//
// INPUT PARAMETERS:
//   none
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void sys_tick_disable(void)
{
  SysTick->VAL  = 0;
  SysTick->LOAD = 0;
  SysTick->CTRL = 0;
} /* sys_tick_disable */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//    Initializes the SysTick timer with a specified period for periodic 
//    interrupts or delays. Configures SysTick to use the core clock, sets 
//    the reload value, and enables the timer and its interrupt. Set up 
//    the SysTick interrupt priority and ensure the timer is disabled during 
//    configuration.
//
// INPUT PARAMETERS:
//   period - a 32-bit valkue that represents the number clock cycles for
//            SysTick interrupts, period must be > 0.
//
// OUTPUT PARAMETERS:
//   none
//
// RETURN:
//   none
// -----------------------------------------------------------------------------
void sys_tick_init(uint32_t msec)
{
  // Ensure SysTick is disabled
  sys_tick_disable();

  // Set priority of SysTick interrupt 
  NVIC_SetPriority(SysTick_IRQn, 2);

  // Reconfigure the SysTick
  uint32_t bus_freq = get_bus_clock_freq();

  SysTick->LOAD = (bus_freq / 1000) * msec - 1;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | 
                  SysTick_CTRL_ENABLE_Msk;
} /* sys_tick_init */

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  
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
void SysTick_Handler(void)
{
  static uint16_t note_index = 0;
  static uint16_t loop_delay = 0;
  static uint16_t noteDuration = 0;

  if(loop_delay++ >= noteDuration)
  {
    int divider = melody[note_index + 1];
    if (divider > 0) {
      // regular note, just proceed
      noteDuration = (WHOLE_NOTE) / divider;
    } else if (divider < 0) {
      // dotted notes are represented with negative durations!!
      noteDuration = (WHOLE_NOTE) / abs(divider);
      noteDuration *= 1.5; // increases the duration in half for dotted notes
    }

    buzzer_tone(melody[note_index]);

    note_index += 2;
    if(note_index >= sizeof(melody)/sizeof(melody[0]))
    {
      note_index = 0;
    }

    loop_delay = 0;
  }
} /* SysTick_Handler */

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

void buzzer_tone(uint16_t frequency)
{
  // clock freq is 20khz so 20,000 / frequency = desired frequency
  // set timer reload value
  TIMA0->COUNTERREGS.LOAD = GPTIMER_LOAD_LD_MASK & ((TIMA_CLOCK_FREQ/frequency) - 1);

  // set 50% duty cycle
  uint32_t threshold = (TIMA0->COUNTERREGS.LOAD * 50) / 150;
  TIMA0->COUNTERREGS.CC_23[0] = GPTIMER_CC_23_CCVAL_MASK & threshold;
}

void buzzer_init(void) 
{
  IOMUX->SECCFG.PINCM[BUZZER_IOMUX] = IOMUX_PINCM17_PF_TIMA0_CCP2 | 
                                      IOMUX_PINCM_PC_CONNECTED;
  GPIOB->DOESET31_0 = BUZZER_MASK;
  
  pwm_init();
  buzzer_tone(REST);
 
  sys_tick_init(1);
}