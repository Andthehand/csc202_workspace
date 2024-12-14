//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Final Project
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a tetris clone using the ti MKII expansion board.
//
//*****************************************************************************
//*****************************************************************************

//-----------------------------------------------------------------------------
// Loads standard C include files
//-----------------------------------------------------------------------------
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>

#include "ST7735S.h"
#include "Joystick.h"
#include "Events.h"
#include "Buzzer.h"
#include "Game.h"

#include "LaunchPad.h"
#include "clock.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

// Define a structure to hold different data types


int main(void){
  clock_init_40mhz();
  launchpad_gpio_init();

  ST7735S_init();
  joystick_init();
  buzzer_init();

  init_events();

  game_init();
  game_loop();

  // Endless loop to prevent program from ending
  while (1);
} /* main */
