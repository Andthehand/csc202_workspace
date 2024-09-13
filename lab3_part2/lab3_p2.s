.thumb

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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function sets the specified bit(s) in a 16-bit register value using
//  the provided bit mask. It performs a bitwise OR operation to set the bit(s).
//
// INPUT PARAMETERS:
//  reg_value - The original 16-bit register value.
//  bit_mask  - The bit mask indicating which bit(s) to clear.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  uint16_t - The modified register value with the specified bit(s) set.
// -----------------------------------------------------------------------------
my_asm_16bitset:
  ORRS R0, R1
  BX LR

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function clears the specified bit(s) in a 16-bit register value using
//  the provided bit mask. It performs a bitwise AND operation with the
//  complement of the bit mask to clear the bit(s).
//
// INPUT PARAMETERS:
//  reg_value - The original 16-bit register value.
//  bit_mask  - The bit mask indicating which bit(s) to clear.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  uint16_t - The modified register value with the specified bit(s) cleared.
// -----------------------------------------------------------------------------
my_asm_16bitclr:
  //~bitmask
  MVNS R1, R1
  ANDS R0, R1
  BX LR

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  This function checks if the specified bit(s) in a 16-bit register value are
//  set using the provided bit mask. It performs a bitwise AND operation to 
//  verify if the bit(s) are set.
//
// INPUT PARAMETERS:
//  reg_value - a 16-bit register value to check.
//  bit_mask  - a 16-bit mask indicating which bit(s) to check.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  bool - true if the specified bit(s) are set, false otherwise.
// -----------------------------------------------------------------------------
my_asm_16bitcheck:
  ANDS R0, R1
  CMP R0, R1
  BEQ Same
  LDR R0, =0
  BX LR

Same:
  LDR R0, =1
  BX LR

.end