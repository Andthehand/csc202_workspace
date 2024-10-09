//*****************************************************************************
//*****************************    C Source Code    ***************************
//*****************************************************************************
//  DESIGNER NAME:  Andrew DeFord
//
//       LAB NAME:  Lab 7: Working with Interrupts
//
//      FILE NAME:  main.c
//
//-----------------------------------------------------------------------------
//
// DESCRIPTION:
//    This program serves as a way to get used to interrupts in a program.
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
#include "LaunchPad.h"
#include "lcd1602.h"

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void init_PB2_irq();
void disable_PB2_irq();

void run_lab7_part1();
void run_lab7_part2();

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define MSP0_CLOCK_FREQUENCY                                              (40E6)
#define SYS_TICK_PERIOD                                               (10.25E-3)
#define SYS_TICK_PERIOD_COUNT           (SYS_TICK_PERIOD * MSP0_CLOCK_FREQUENCY)

#define NUM_STATES                                                            18

#define LCD_LAST_ADDR                                                     (0x4F)
#define LCD_BLANK_CHAR                                                       ' '

#define PART1_MESSAGE                                "Microcontrollers are fun."
#define PART1_MESSAGE_END                                          "Part 1 Done"
#define PART1_DELAY                                                          100

#define PART2_MESSAGE         "Microcontrollers are fun. I love programming "  \
                              "in MSPM0+ assembly code!!!"
#define PART2_MESSAGE_START                                          "Press PB2"
#define PART2_MESSAGE_RUNNING                                   "Running Part 2"
#define PART2_MESSAGE_END                                          "Part 2 Done"

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
bool g_PB2_Pressed = false;

// Define a structure to hold different data types
const uint8_t seg7_letter_code[] =
{
  0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00,
  0x3F, 0x00, 0x3F, 0x00, 0x3F, 0x00,
  0x6D, 0x00, 0x6D, 0x00, 0x6D, 0x00
};

const uint8_t delay_count[] = 
{
  0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x18, // dots
  0x24, 0x24, 0x24, 0x24, 0x24, 0x24, // dash
  0x0C, 0x0C, 0x0C, 0x0C, 0x0C, 0x30  // dots
};


int main(void)
{
  clock_init_40mhz();
  launchpad_gpio_init();
  dipsw_init();

  led_init();
  led_disable();
  seg7_init();

  I2C_init();
  lcd1602_init();
  lcd_set_display_on();

  sys_tick_init(SYS_TICK_PERIOD_COUNT);
  init_PB2_irq();

  run_lab7_part1();
  run_lab7_part2();

  // Endless loop to prevent program from ending
  while (1);

} /* main */

void GROUP1_IRQHandler(void)
{
  uint32_t group_iidx_status;
  uint32_t gpio_mis;

  do
  {
    group_iidx_status = CPUSS->INT_GROUP[1].IIDX;

    switch (group_iidx_status) 
    {
      case (CPUSS_INT_GROUP_IIDX_STAT_INT0): //GPIOA
        gpio_mis = GPIOA->CPU_INT.MIS;
        if((gpio_mis & GPIO_CPU_INT_MIS_DIO15_MASK) == GPIO_CPU_INT_MIS_DIO15_SET)
        {
          g_PB2_Pressed = true;
          GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
        } /* if */
        break;
    } /* switch */
  } while(group_iidx_status != 0);
} /* GROUP1_IRQHandler */

void SysTick_Handler(void)
{
  static uint16_t delay_time = 1;
  static uint16_t code_index = 0;

  delay_time--;
  if (delay_time == 0)
  {
    seg7_on(seg7_letter_code [code_index], SEG7_DIG0_ENABLE_IDX);

    delay_time = delay_count [code_index];
    code_index++;

    if (code_index == NUM_STATES)
    {
      code_index = 0;
    } /* if */
  } /* if */
} /* SysTick_Handler */

void init_PB2_irq()
{
  GPIOA->POLARITY15_0 = GPIO_POLARITY15_0_DIO15_RISE;
  GPIOA->CPU_INT.ICLR = GPIO_CPU_INT_ICLR_DIO15_CLR;
  GPIOA->CPU_INT.IMASK = GPIO_CPU_INT_IMASK_DIO15_SET;

  NVIC_SetPriority(GPIOA_INT_IRQn, 2);
  NVIC_EnableIRQ(GPIOA_INT_IRQn);
} /* init_PB2_irq */

void disable_PB2_irq()
{
  NVIC_DisableIRQ(GPIOA_INT_IRQn);
} /* disable_PB2_irq */

void run_lab7_part1()
{
  const char* message = PART1_MESSAGE;
  while (!g_PB2_Pressed) 
  {
    for(uint8_t offset = 0; offset < CHARACTERS_PER_LCD_LINE
                            && !g_PB2_Pressed; offset++) 
    {
      lcd_set_ddram_addr(LCD_LAST_ADDR - offset);
      for(uint8_t i = 0; i <= offset; i++)
      {
        lcd_write_char(message[i]);
      } /* for */

      msec_delay(PART1_DELAY);
    } /* for */



    for (uint8_t offset = 1; message[offset] != '\0' &&
                              !g_PB2_Pressed; offset++) 
    {
      lcd_set_ddram_addr(LCD_LINE2_ADDR);
      for(char i = 0; message[offset + i] != '\0' &&
                        i < CHARACTERS_PER_LCD_LINE &&
                        !g_PB2_Pressed; i++)
      {
        lcd_write_char(message[offset + i]);
      } /* for */
      lcd_write_char(LCD_BLANK_CHAR);

      msec_delay(PART1_DELAY);
    } /* for */

    lcd_clear();
  } /* while */

  lcd_clear();
  lcd_write_string(PART1_MESSAGE_END);

  g_PB2_Pressed = false;
} /* run_lab7_part1 */

void run_lab7_part2()
{
  lcd_set_ddram_addr(LCD_LINE2_ADDR);
  lcd_write_string(PART2_MESSAGE_START);

  while (!g_PB2_Pressed);

  lcd_clear();
  lcd_write_string(PART2_MESSAGE_RUNNING);
  msec_delay(1000);

  lcd_clear();
  g_PB2_Pressed = false;

  const char* message = PART2_MESSAGE;
  while (!g_PB2_Pressed) 
  {
    for(uint8_t offset = 0; offset < CHARACTERS_PER_LCD_LINE &&
                            !g_PB2_Pressed; offset++) 
    {
      lcd_set_ddram_addr(LCD_LAST_ADDR - offset);
      for(uint8_t i = 0; i <= offset; i++)
      {
        lcd_write_char(message[i]);
      } /* for */

      msec_delay(PART1_DELAY);
    } /* for */



    for (uint8_t offset = 1; message[offset] != '\0'
                              && !g_PB2_Pressed; offset++) 
    {
      lcd_set_ddram_addr(LCD_LINE2_ADDR);
      for(char i = 0; message[offset + i] != '\0' &&
                        i < CHARACTERS_PER_LCD_LINE &&
                        !g_PB2_Pressed; i++)
      {
        lcd_write_char(message[offset + i]);
      } /* for */
      lcd_write_char(LCD_BLANK_CHAR);

      msec_delay(PART1_DELAY);
    } /* for */

    lcd_clear();
  } /* while */

  lcd_clear();
  lcd_write_string(PART2_MESSAGE_END);

  sys_tick_disable();
  disable_PB2_irq();
} /* run_lab7_part2 */
