// *****************************************************************************
// ***************************    C Source Code     ****************************
// *****************************************************************************
//   DESIGNER NAME:  Andrew DeFord
//
//         VERSION:  0.1
//
//       FILE NAME:  Game.c
//
//-----------------------------------------------------------------------------
// DESCRIPTION
//    
//
// NOTES:
//    
//
// *****************************************************************************
//******************************************************************************


//-----------------------------------------------------------------------------
// Load standard C include files
//-----------------------------------------------------------------------------
#include <math.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

//-----------------------------------------------------------------------------
// Loads MSP launchpad board support macros and definitions
//-----------------------------------------------------------------------------
#include <ti/devices/msp/msp.h>
#include "Events.h"
#include "gfx.h"
#include "Joystick.h"

#include "clock.h"

//-----------------------------------------------------------------------------
// Define symbolic constants used by the program
//-----------------------------------------------------------------------------
#define TETRIS_HEIGHT                                                        20
#define TETRIS_WIDTH                                                         10
#define TETRIS_BLOCK_SIZE                                                     5
#define TETRIS_NO_BLOCK                                                       0
#define TETRIS_LOOPS_TILL_FALL                                                1

#define JOYSTICK_RIGHT_DEADBAND                                            3000
#define JOYSTICK_LEFT_DEADBAND                                             1000

//-----------------------------------------------------------------------------
// Define a structure to hold different data types used by the program
//-----------------------------------------------------------------------------
typedef struct {
  uint8_t x;
  uint8_t y;
} Vector2;

typedef struct {
  uint8_t array[8];
} Shape;

typedef enum {
  NONE = 0,
  GRID,
  SELECTED_BLOCK
} CheckType;

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void update_dirty_grid(CheckType dirty_grid[TETRIS_WIDTH][TETRIS_HEIGHT], 
                        Vector2 previous_pos, Vector2 current_pos, Shape block);
void render_screen(CheckType dirty_grid[TETRIS_WIDTH][TETRIS_HEIGHT], 
                    uint8_t block_grid[TETRIS_WIDTH][TETRIS_HEIGHT]);

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------

void game_init(void)
{
  uint8_t block_grid[TETRIS_WIDTH][TETRIS_HEIGHT] = {{0}};
  CheckType dirty_grid[TETRIS_WIDTH][TETRIS_HEIGHT];

  const Shape SHAPES[7] = {
    { 1, 1, 1, 1, 0, 0, 0, 0 }, //O
    { 1, 0, 1, 0, 1, 0, 1, 0 }, //I
    { 1, 0, 1, 1, 0, 1, 0, 0 }, //S
    { 0, 1, 1, 1, 1, 0, 0, 0 }, //Z
    { 1, 0, 1, 0, 1, 1, 0, 0 }, //L
    { 0, 1, 0, 1, 1, 1, 0, 0 }, //J
    { 0, 1, 1, 1, 0, 1, 0, 0 }  //T
  };

  Vector2 selected_block_pos = {0};
  Shape current_shape = SHAPES[0];

  memset(dirty_grid, GRID, sizeof(dirty_grid));

  uint32_t loop_count = 0;
  while (true) {
    render_screen(dirty_grid, block_grid);

    if(read_joystick().y_pos > JOYSTICK_RIGHT_DEADBAND && 
        selected_block_pos.x < TETRIS_WIDTH - 2)
    {
      Vector2 previous_pos = selected_block_pos;
      selected_block_pos.x++;
      
      update_dirty_grid(dirty_grid, previous_pos, selected_block_pos, current_shape);
    }
    else if (read_joystick().y_pos < JOYSTICK_LEFT_DEADBAND && 
              selected_block_pos.x > 0) {
      Vector2 previous_pos = selected_block_pos;
      selected_block_pos.x--;

      update_dirty_grid(dirty_grid, previous_pos, selected_block_pos, current_shape);
    }

    loop_count++;
    if((loop_count % TETRIS_LOOPS_TILL_FALL) == 0)
    {
      Vector2 previous_pos = selected_block_pos;
      selected_block_pos.y++;

      if((selected_block_pos.y % (TETRIS_HEIGHT - 1)) == 0) 
      {
        for(uint8_t y = 0; y < 4; y++)
        {
          for(uint8_t x = 0; x < 2; x++)
          {
            if(current_shape.array[(y * 2) + x] != 0)
            {
              block_grid[x + selected_block_pos.x][y + selected_block_pos.y] 
                          = current_shape.array[(y * 2) + x];
              dirty_grid[x + selected_block_pos.x][y + selected_block_pos.y]
                          = GRID;
            }
          }
        }

        current_shape = SHAPES[rand() % (sizeof(SHAPES) / sizeof(SHAPES[0]))];
        selected_block_pos.y = 0;

        for(uint8_t y = 0; y < 4; y++)
        {
          for(uint8_t x = 0; x < 2; x++)
          {
            if(current_shape.array[(y * 2) + x] != 0)
            {
              dirty_grid[x + selected_block_pos.x][y + selected_block_pos.y]
                = SELECTED_BLOCK;
            }
          }
        }
      }
      else 
      {
        update_dirty_grid(dirty_grid, previous_pos, selected_block_pos, current_shape);
      }
    }

    msec_delay(100);
  }
}

void update_dirty_grid(CheckType dirty_grid[TETRIS_WIDTH][TETRIS_HEIGHT], 
                        Vector2 previous_pos, Vector2 current_pos, Shape block) 
{
  for(uint8_t y = 0; y < 4; y++)
  {
    for(uint8_t x = 0; x < 2; x++)
    {
      if(block.array[(y * 2) + x] != 0)
      {
        dirty_grid[x + previous_pos.x][y + previous_pos.y] = GRID;
      }
    }
  }

  for(uint8_t y = 0; y < 4; y++)
  {
    for(uint8_t x = 0; x < 2; x++)
    {
      if(block.array[(y * 2) + x] != 0)
      {
        dirty_grid[x + current_pos.x][y + current_pos.y] = SELECTED_BLOCK;
      }
    }
  }
}

void render_screen(CheckType dirty_grid[TETRIS_WIDTH][TETRIS_HEIGHT], 
                    uint8_t block_grid[TETRIS_WIDTH][TETRIS_HEIGHT])
{
  static const color565_t BLOCK_COLOR[] = {
    (color565_t){ .r = 0xFF, .g = 0xFF, .b = 0xFF},
    (color565_t){ .r = 0xFF, .g = 0x00, .b = 0x00}
  };

  for (uint8_t y = 0; y < TETRIS_HEIGHT; y++) 
  {
    for (uint8_t x = 0; x < TETRIS_WIDTH; x++)
    {
      switch (dirty_grid[x][y]) {
        case GRID:
          draw_rectangle((x * (TETRIS_BLOCK_SIZE + 1)) + 34, 
                (y * (TETRIS_BLOCK_SIZE + 1)) + 4, 
                TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 
                BLOCK_COLOR[block_grid[x][y]]);
          break;
        case SELECTED_BLOCK:
            draw_rectangle((x * (TETRIS_BLOCK_SIZE + 1)) + 34, 
                (y * (TETRIS_BLOCK_SIZE + 1)) + 4, 
                TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 
                RGB_to_color(0xFF, 0x00, 0x00));
          break;
        case NONE:
          break;
      }

      dirty_grid[x][y] = NONE;
    }
  }
}
