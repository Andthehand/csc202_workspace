//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  TBD
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
#include <stdio.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>

#include "ST7735S.h"
#include "Joystick.h"
#include "Events.h"
#include "gfx.h"
#include "Buzzer.h"
#include "pitches.h"

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
    // buzzer_init();

    init_events();

    //Game loop
    while (true) {
        draw_rectangle(0, 0, 50, 50, RGB_to_color(0xFF, 0x00, 0x00));
        draw_rectangle(55, 0, 50, 50, RGB_to_color(0x00, 0xFF, 0x00)); 
    }

    // Endless loop to prevent program from ending
    while (1);

} /* main */
