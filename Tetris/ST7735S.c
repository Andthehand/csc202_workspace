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
#include "ti/devices/msp/peripherals/hw_iomux.h"
#include "ti/devices/msp/peripherals/hw_spi.h"
#include "ST7735S.h"
#include "math.h"

#include "clock.h"

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Sends a sequence of initialization commands to configure the ST7735S
//  display. This includes resetting the display, setting color format,
//  orientation, and filling the screen with a default background color.
//
// INPUT PARAMETERS: 
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void send_init_commands(void)
{
  //Pulse reset
  GPIOB->DOUT31_0 &= (~ST7735S_RESET_MASK);
  msec_delay(STARTUP_DELAY);
  GPIOB->DOUT31_0 |= ST7735S_RESET_MASK;
  msec_delay(STARTUP_DELAY);

  ST7735S_write_command(LCD_SWRESET_CMD);
  msec_delay(STARTUP_DELAY);

  ST7735S_write_command(LCD_SLPOUT_CMD);
  msec_delay(STARTUP_DELAY);

  ST7735S_write_command(LCD_COLMOD_CMD);
  ST7735S_write_data(LCD_IFPF_16bit);
  msec_delay(STARTUP_DELAY);

  ST7735S_write_command(LCD_MADCTL_CMD);
  ST7735S_write_data(LCD_MADCTL_MY_MASK | LCD_MADCTL_MX_MASK);

  ST7735S_set_addr(0, 0, LCD_WIDTH, LCD_HEIGHT);
  ST7735S_write_command(LCD_RAMWR_CMD);

  color565_t background_color = RGB_to_color(0x00, 0x00, 0x00);
  for (uint16_t i = 0; i < (LCD_WIDTH * LCD_HEIGHT); i++)
  {
    ST7735S_write_color(background_color);
  }

  ST7735S_write_command(LCD_IDMOFF_CMD);
  ST7735S_write_command(LCD_NORON_CMD);

  ST7735S_write_command(LCD_DISPON_CMD);
  msec_delay(STARTUP_LAST_DELAY);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Initializes the ST7735S display controller and configures the SPI1 module
//  for communication. Sets up GPIO pins for reset and register select, and
//  sends initialization commands to the display.
//
// INPUT PARAMETERS: 
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void ST7735S_init(void)
{
  // Reset SPI1
  SPI1->GPRCM.RSTCTL = (GPTIMER_RSTCTL_KEY_UNLOCK_W | 
					    GPTIMER_RSTCTL_RESETSTKYCLR_CLR |
					    GPTIMER_RSTCTL_RESETASSERT_ASSERT);

  // Enable power to SPI1
  SPI1->GPRCM.PWREN = (GPTIMER_PWREN_KEY_UNLOCK_W | 
                       GPTIMER_PWREN_ENABLE_ENABLE);

  clock_delay(24);
  
  IOMUX->SECCFG.PINCM[LP_SPI_CLK_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_CLK_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_MOSI_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_MOSI_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_MISO_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                     IOMUX_PINCM_INENA_ENABLE | LP_SPI_MISO_PFMODE);

  IOMUX->SECCFG.PINCM[LP_SPI_CS0_IOMUX] = (IOMUX_PINCM_PC_CONNECTED |
                      LP_SPI_CS0_PFMODE);

  // Select BusClk (SysClk) source for SPI module
  SPI1->CLKSEL = (SPI_CLKSEL_SYSCLK_SEL_ENABLE | SPI_CLKSEL_MFCLK_SEL_DISABLE |
                  SPI_CLKSEL_LFCLK_SEL_DISABLE);

  // Set clock division
  SPI1->CLKDIV = SPI_CLKDIV_RATIO_DIV_BY_1;

  // Both SPI modules are on PD1 
  uint32_t bus_clock = get_bus_clock_freq();

  // Set clock prescaler to get final SPI clock frequency
  SPI1->CLKCTL = SPI_CLKCTL_SCR_MINIMUM;

  // Configure SPI control register 0
  SPI1->CTL0 = (SPI_CTL0_CSCLR_DISABLE | SPI_CTL0_CSSEL_CSSEL_0 | 
                SPI_CTL0_SPH_FIRST | SPI_CTL0_SPO_LOW | 
                SPI_CTL0_PACKEN_DISABLED | SPI_CTL0_FRF_MOTOROLA_4WIRE | 
                SPI_CTL0_DSS_DSS_8);

  // Configure SPI control register 1
  SPI1->CTL1 = (SPI_CTL1_RXTIMEOUT_MINIMUM | SPI_CTL1_REPEATTX_DISABLE |
                SPI_CTL1_CDMODE_MINIMUM | SPI_CTL1_CDENABLE_DISABLE |
                SPI_CTL1_PTEN_DISABLE | SPI_CTL1_PES_DISABLE | 
                SPI_CTL1_PREN_DISABLE | SPI_CTL1_MSB_ENABLE |
                SPI_CTL1_POD_DISABLE | SPI_CTL1_CP_ENABLE | 
                SPI_CTL1_LBM_DISABLE | SPI_CTL1_ENABLE_ENABLE);

  //Setup Reset Pin as input
  IOMUX->SECCFG.PINCM[ST7735S_RESET_IOMUX] = (IOMUX_PINCM_PC_CONNECTED | 
                      PINCM_GPIO_PIN_FUNC);
  GPIOB->DOE31_0 |= ST7735S_RESET_MASK;

  //Setup Register Select
  IOMUX->SECCFG.PINCM[ST7735S_REG_SEL_IOMUX] = (IOMUX_PINCM_PC_CONNECTED | 
                      PINCM_GPIO_PIN_FUNC | IOMUX_PINCM_INENA_ENABLE);
  GPIOA->DOE31_0 |= ST7735S_REG_SEL_MASK;

  send_init_commands();
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Sends an 8-bit command to the ST7735S display via SPI. Ensures the SPI
//  interface is not busy before and after the command is sent.
//
// INPUT PARAMETERS: 
//  data: The 8-bit command to be written to the display.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void ST7735S_write_command(uint8_t data)
{
  while((SPI1->STAT & SPI_STAT_BUSY_MASK) == SPI_STAT_BUSY_ACTIVE); 
  GPIOA->DOUT31_0 &= ~ST7735S_REG_SEL_MASK;

  SPI1->TXDATA = data;

  while((SPI1->STAT & SPI_STAT_BUSY_MASK) == SPI_STAT_BUSY_ACTIVE); 
  GPIOA->DOUT31_0 |= ST7735S_REG_SEL_MASK;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Writes an 8-bit data value to the ST7735S display via SPI. Ensures the TX
//  FIFO is not full before writing to avoid data loss.
//
// INPUT PARAMETERS: 
//  data: The 8-bit data value to be written to the display.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void ST7735S_write_data(uint8_t data)
{
  // Wait here until TX FIFO is not full
  while((SPI1->STAT & SPI_STAT_TNF_MASK) == SPI_STAT_TNF_FULL); 
  
  SPI1->TXDATA = data;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Writes a 16-bit RGB565 color value to the ST7735S display.
//
// INPUT PARAMETERS: 
//  color: A color565_t structure containing the 16-bit RGB565 color value
//         to be written.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void ST7735S_write_color(color565_t color)
{
  ST7735S_write_data(color.packet[1]);
  ST7735S_write_data(color.packet[0]);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Sets the address window for pixel updates on the ST7735S display. This 
//  specifies a rectangular area where subsequent pixel data will be written.
//
// INPUT PARAMETERS: 
//  x0: The starting x-coordinate of the address window.
//  y0: The starting y-coordinate of the address window.
//  x1: The ending x-coordinate of the address window.
//  y1: The ending y-coordinate of the address window.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
//-----------------------------------------------------------------------------
void ST7735S_set_addr(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1) 
{
  ST7735S_write_command(LCD_CASET_CMD);
  ST7735S_write_data(0x00);
  ST7735S_write_data(LCD_XS_OFFSET + x0);
  ST7735S_write_data(0x00);
  ST7735S_write_data(LCD_XE_OFFSET + x1);

  ST7735S_write_command(LCD_RASET_CMD);
  ST7735S_write_data(0x00);
  ST7735S_write_data(LCD_YS_OFFSET + y0);
  ST7735S_write_data(0x00);
  ST7735S_write_data(LCD_YE_OFFSET + y1);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Converts 8-bit RGB color values to a 16-bit RGB565 format.
//
// INPUT PARAMETERS: 
//  r: The red color component (0-255).
//  g: The green color component (0-255).
//  b: The blue color component (0-255).
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  A color565_t structure containing the 16-bit RGB565 color representation.
//-----------------------------------------------------------------------------
color565_t RGB_to_color(uint8_t r, uint8_t g, uint8_t b) 
{
  return (color565_t){ .r = ((float)r / (float)UINT8_MAX) * UINT5_MAX, 
                       .g = ((float)g / (float)UINT8_MAX) * UINT6_MAX, 
                       .b = ((float)b / (float)UINT8_MAX) * UINT5_MAX};
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Draws a filled rectangle on the display at a specified position, with
//  given dimensions and color.
//
// INPUT PARAMETERS: 
//  x: The x-coordinate of the top-left corner of the rectangle.
//  y: The y-coordinate of the top-left corner of the rectangle.
//  width: The width of the rectangle in pixels.
//  height: The height of the rectangle in pixels.
//  color: The fill color of the rectangle, represented as a 16-bit RGB565 value.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void draw_rectangle(uint8_t x, uint8_t y, uint8_t width, uint8_t height, color565_t color)
{
  ST7735S_set_addr(x, y, x + width, y + height);
  ST7735S_write_command(LCD_RAMWR_CMD);
  for(uint16_t i = 0; i < (width * height); i++)
  {
    ST7735S_write_color(color);
  }
}
