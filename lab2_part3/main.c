//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 2
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program tests what each built in types size turns as well as a test
//    on how memory is stored inside of a union. 
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
#include "clock.h"
#include "uart.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------

void msp_printf(char* buffer, unsigned int value);

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------


// Define a structure to hold different data types
typedef struct 
{
  unsigned  long  int  LA;
  signed    long  int  LB;
  unsigned        int  IA[2];
  signed          int  IB[2];
  unsigned  short int  SA[2];
  signed    short int  SB[2];
  unsigned       char  CA[4];
  signed         char  CB[4];  
} test_struct;

typedef union 
{
  unsigned  long  int  LA;
  signed    long  int  LB;
  unsigned        int  IA;
  signed          int  IB;
  unsigned  short int  SA[2];
  signed    short int  SB[2];
  unsigned       char  CA[4];
  signed         char  CB[4];  
} union_32;

int main(void)
{
  //Local variables
  test_struct struct_var;
  union_32    union_var;

  //Init
  union_var.LA = 0x1234ABCD;

  //Setup
  clock_init_40mhz();
  UART_init(115200);

  //Print block
  msp_printf(" *** PROGRAM RUNNING ***\r\n\r\n", 0);
  msp_printf("---------------------------------------------------------\r\n", 0);

  msp_printf(" unsigned  long  int = 0x%x bytes\r\n", sizeof(struct_var.LA));
  msp_printf(" signed    long  int = 0x%x bytes\r\n", sizeof(struct_var.LB));
  msp_printf(" unsigned        int = 0x%x bytes\r\n", sizeof(struct_var.IA[0]));
  msp_printf(" signed          int = 0x%x bytes\r\n", sizeof(struct_var.IB[0]));
  msp_printf(" unsigned  short int = 0x%x bytes\r\n", sizeof(struct_var.SA[0]));
  msp_printf(" signed    short int = 0x%x bytes\r\n", sizeof(struct_var.SB[0]));
  msp_printf(" unsigned       char = 0x%x bytes\r\n", sizeof(struct_var.CA[0]));
  msp_printf(" signed         char = 0x%x bytes\r\n", sizeof(struct_var.CB[0]));

  msp_printf("\r\n", 0);
  msp_printf(" The structure = 0x%x bytes\r\n", sizeof(struct_var));
  msp_printf("     The union = 0x%x bytes\r\n", sizeof(union_var));
  msp_printf("\r\n", 0);

  msp_printf("Dumping data values from Union\r\n", 0);
  msp_printf("---------------------------------------------------------\r\n", 0);
  msp_printf("      union unsigned long  int = 0x%x\r\n", union_var.LA);
  msp_printf("         union signed long int = 0x%x\r\n", union_var.LB);
  msp_printf("\r\n", 0);

  msp_printf("            union unsigned int = 0x%x\r\n", union_var.IA);
  msp_printf("              union signed int = 0x%x\r\n", union_var.IB);
  msp_printf("\r\n", 0);

  msp_printf("union unsigned short int SA[0] = 0x%x\r\n", union_var.SA[0]);
  msp_printf("  union signed short int SB[0] = 0x%x\r\n", union_var.SB[0]);
  msp_printf("union unsigned short int SA[1] = 0x%x\r\n", union_var.SA[1]);
  msp_printf("  union signed short int SB[1] = 0x%x\r\n", union_var.SB[1]);
  msp_printf("\r\n", 0);

  msp_printf("     union unsigned char CA[0] = 0x%x\r\n", union_var.CA[0]);
  msp_printf("     union unsigned char CA[1] = 0x%x\r\n", union_var.CA[1]);
  msp_printf("     union unsigned char CA[2] = 0x%x\r\n", union_var.CA[2]);
  msp_printf("     union unsigned char CA[3] = 0x%x\r\n", union_var.CA[3]);
  msp_printf("\r\n", 0);
  
  msp_printf("       union signed char CB[0] = 0x%x\r\n", union_var.CB[0]);
  msp_printf("       union signed char CB[1] = 0x%x\r\n", union_var.CB[1]);
  msp_printf("       union signed char CB[2] = 0x%x\r\n", union_var.CB[2]);
  msp_printf("       union signed char CB[3] = 0x%x\r\n", union_var.CB[3]);
  msp_printf("\r\n", 0);
  msp_printf("\r\n", 0);

  msp_printf(" *** PROGRAM TERMINATED ***\r\n ", 0);

  // Endless loop to prevent program from ending
  while (1);

} /* main */


//-----------------------------------------------------------------------------
// DESCRIPTION:
//  A custom implementation of printing using the MSPM0 with a printf format
//  
// INPUT PARAMETERS: 
//  buffer: A format string using the same format as sprintf.
//  value: The integer value to be formatted into the output string.
//  
// OUTPUT PARAMETERS:
//  none 
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void msp_printf(char* buffer, unsigned int value) {
  unsigned int i = 0;
  unsigned int len = 0;
  char string[80];

  len = sprintf(string, buffer, value);

  // Sends the string one bit at a time through UART
  for(i = 0; i < len; i++)
  {
    UART_out_char(string[i]);
  } /* for */

} /* msp _printf */