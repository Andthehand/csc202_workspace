//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 3, part 2
//
//      FILE NAME:  lab3_p2.s
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program Is used to create assembly functions to be called from C.
//    These functions are used to set, clear, and check specific bits using a 
//    bit mask.
//
//*****************************************************************************
//*****************************************************************************

// Used to make these function findable by the linker and used by C code
.global my_asm_16bitset
.global my_asm_16bitclr
.global my_asm_16bitcheck

my_asm_16bitset:
  ORRS R0, R1
  BX LR

my_asm_16bitclr:
  //~bitmask
  MVNS R0, R0
  ANDS R0, R1
  BX LR

my_asm_16bitcheck:
  ANDS R0, R1
  BX LR