// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  Buzzer.h
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    Initializes the pizeo buzzer to play music depending on the hertz using 
//    PWM. The song currently configured is the main song of tetris.
//
//    This was made with the help of:
//    https://www.reddit.com/r/arduino/comments/mqodmu/music_piezo_buzzer_box_question/
//
//-----------------------------------------------------------------------------
// DISCLAIMER
//    
// *****************************************************************************
//******************************************************************************

#ifndef __BUZZER_H__
#define __BUZZER_H__

#define BUZZER_PORT                                                 (GPIO_PORTB)
#define BUZZER_MASK                                                     (1 << 4)
#define BUZZER_IOMUX                                             (IOMUX_PINCM17)

#define TIMA_CLOCK_FREQ                                                   200000

// ----------------------------------------------------------------------------
// Prototype for support functions
// ----------------------------------------------------------------------------

void buzzer_init(void);

void buzzer_tone(uint16_t frequency);

#endif /* __BUZZER_H__ */
