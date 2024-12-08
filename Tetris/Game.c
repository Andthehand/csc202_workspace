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
#define TETRIS_LOOPS_TILL_FALL                                               15
#define TETRIS_FILL_DELAY                                                    50
#define TETRIS_LOOP_DELAY                                                    16


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
  int8_t blocks[4][4];
  uint8_t color;
  uint8_t size;
} Shape;

typedef enum {
  NONE = 0,
  UPDATE_GRID,
  UPDATE_SELECTED_BLOCK
} CheckType;

//-----------------------------------------------------------------------------
// Define function prototypes used by the program
//-----------------------------------------------------------------------------
void input_handler();
void clear_rows();
Shape rotate_shape();
void place_shape();
bool can_move(int8_t x_step, int8_t y_step);
bool move(int8_t x_step, int8_t y_step);
void update_dirty_grid_move(Vector2 previous_pos, Vector2 current_pos);
void update_dirty_grid_rotate(const Shape* previous_shape, const Shape* current_shape);
void update_dirty_grid_full_block_grid();
void render_screen();

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
uint8_t block_grid[TETRIS_WIDTH][TETRIS_HEIGHT] = {{0}};
CheckType dirty_grid[TETRIS_WIDTH][TETRIS_HEIGHT];

const Shape SHAPES[] = {
  {{ {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, 1, 4}, // I
  {{ {1, 1}, {1, 1} }, 2, 2},                                         // O
  {{ {0, 1, 1}, {1, 1, 0}, {0, 0, 0} }, 0, 3},                        // S
  {{ {1, 1, 0}, {0, 1, 1}, {0, 0, 0} }, 0, 3},                        // Z
  {{ {1, 0, 0}, {1, 1, 1}, {0, 0, 0} }, 0, 3},                        // L
  {{ {0, 0, 1}, {1, 1, 1}, {0, 0, 0} }, 3, 3},                        // J
  {{ {0, 1, 0}, {1, 1, 1}, {0, 0, 0} }, 0, 3}                         // T
};

Vector2 selected_block_pos = {0};
Shape current_block;
bool game_over = false;

void game_init(void)
{
  current_block = SHAPES[1];
  
  memset(dirty_grid, UPDATE_GRID, sizeof(dirty_grid));

  update_dirty_grid_move(selected_block_pos, selected_block_pos);
}

void game_loop(void)
{
  uint32_t loop_count = 0;
  while (!game_over) {
    render_screen();
    input_handler();

    loop_count++;
    if((loop_count % TETRIS_LOOPS_TILL_FALL) == 0)
    {
      bool moved = move(0, 1);

      if(!moved)
      {
        place_shape();
      }
    }

    msec_delay(TETRIS_LOOP_DELAY);
  }

  memset(block_grid, 1, sizeof(block_grid));
  for(uint8_t x = 0; x < TETRIS_HEIGHT; x++)
  {
    memset(dirty_grid[x], UPDATE_GRID, sizeof(dirty_grid[0]));
    render_screen();
    msec_delay(TETRIS_FILL_DELAY);
  }
}

void input_handler() {
  static bool joystick_last_frame_moved = false;

  Joystick_pos pos = read_joystick();
  if (pos.y_pos > JOYSTICK_RIGHT_DEADBAND && can_move(1, 0)) {
    if(!joystick_last_frame_moved)
    {
      Vector2 previous_pos = selected_block_pos;
      selected_block_pos.x++;

      update_dirty_grid_move(previous_pos, selected_block_pos);
    }

    joystick_last_frame_moved = true;
  }
  else if (pos.y_pos < JOYSTICK_LEFT_DEADBAND && can_move(-1, 0)) {
    if(!joystick_last_frame_moved)
    {
      Vector2 previous_pos = selected_block_pos;
      selected_block_pos.x--;

      update_dirty_grid_move(previous_pos, selected_block_pos);
    }

    joystick_last_frame_moved = true;
  }
  else {
    joystick_last_frame_moved = false;
  }
  
  if (has_pb2_pressed()) {
    Shape rotated_shape = rotate_shape();

    update_dirty_grid_rotate(&current_block, &rotated_shape);

    current_block = rotated_shape;
  }

  if (is_pb1_down() && can_move(0, 1)) {
    Vector2 previous_pos = selected_block_pos;
    selected_block_pos.y++;

    update_dirty_grid_move(previous_pos, selected_block_pos);
  }
}

void clear_rows() {
  for (int y = 0; y < TETRIS_HEIGHT; y++) {
    bool full = true;
    for (int x = 0; x < TETRIS_WIDTH; x++) {
      if (!block_grid[x][y]) {
        full = false;
      }
    }
    
    if (full) {
      update_dirty_grid_full_block_grid();

      for (int ny = y; ny > 0; ny--) {
        for (int x = 0; x < TETRIS_WIDTH; x++) {
          block_grid[x][ny] = block_grid[x][ny - 1];
        }
      }

      memset(block_grid[0], 0, TETRIS_WIDTH);
    }
  }
}

Shape rotate_shape() {
  Shape rotated = current_block;
  for (int y = 0; y < current_block.size; y++) {
    for (int x = 0; x < current_block.size; x++) {
      rotated.blocks[x][current_block.size - 1 - y] = current_block.blocks[y][x];
    }
  }

  if(can_move(0, 0))
  {
    return rotated;
  }
  else 
  {
    return current_block;
  }
}

void place_shape() {
  for (int y = 0; y < current_block.size; y++) {
    for (int x = 0; x < current_block.size; x++) {
      if (current_block.blocks[x][y]) {
        block_grid[selected_block_pos.x + x][selected_block_pos.y + y] = current_block.color;
        dirty_grid[selected_block_pos.x + x][selected_block_pos.y + y] = UPDATE_GRID;
      }
    }
  }
  clear_rows();
  current_block = SHAPES[rand() % (sizeof(SHAPES) / sizeof(SHAPES[0]))];
  selected_block_pos.x = (TETRIS_WIDTH - current_block.size) / 2;
  selected_block_pos.y = 0;

  update_dirty_grid_move(selected_block_pos, selected_block_pos);

  if (!can_move(0, 0)) {
    game_over = true;
  }
}

bool can_move(int8_t x_step, int8_t y_step) {
  for (int8_t y = 0; y < current_block.size; y++) {
    for (int8_t x = 0; x < current_block.size; x++) {
      if (current_block.blocks[x][y] != TETRIS_NO_BLOCK) {
        int8_t nx = selected_block_pos.x + x + x_step;
        int8_t ny = selected_block_pos.y + y + y_step;

        if (nx < 0 || nx >= TETRIS_WIDTH || ny >= TETRIS_HEIGHT || 
            (ny >= 0 && block_grid[nx][ny])) {
          return false;
        }
      }
    }
  }
  
  return true;
}

bool move(int8_t x_step, int8_t y_step)
{
  bool movable = can_move(x_step, y_step);
  if(movable)
  {
    Vector2 previous_pos = selected_block_pos;

    selected_block_pos.x += x_step;
    selected_block_pos.y += y_step;

    update_dirty_grid_move(previous_pos, selected_block_pos);
  }

  return movable;
}

void update_dirty_grid_move(Vector2 previous_pos, Vector2 current_pos) 
{
  for(uint8_t y = 0; y < current_block.size; y++)
  {
    for(uint8_t x = 0; x < current_block.size; x++)
    {
      if(current_block.blocks[x][y] != 0)
      {
        dirty_grid[x + previous_pos.x][y + previous_pos.y] = UPDATE_GRID;
      }
    }
  }

  for(uint8_t y = 0; y < current_block.size; y++)
  {
    for(uint8_t x = 0; x < current_block.size; x++)
    {
      if(current_block.blocks[x][y] != 0)
      {
        dirty_grid[x + current_pos.x][y + current_pos.y] = UPDATE_SELECTED_BLOCK;
      }
    }
  }
}

void update_dirty_grid_rotate(const Shape* previous_shape, const Shape* current_shape)
{
  for(uint8_t y = 0; y < previous_shape->size; y++)
  {
    for(uint8_t x = 0; x < previous_shape->size; x++)
    {
      if(previous_shape->blocks[x][y] != TETRIS_NO_BLOCK)
      {
        dirty_grid[x + selected_block_pos.x][y + selected_block_pos.y] = UPDATE_GRID;
      }
    }
  }

  for(uint8_t y = 0; y < current_shape->size; y++)
  {
    for(uint8_t x = 0; x < current_shape->size; x++)
    {
      if(current_shape->blocks[x][y] != TETRIS_NO_BLOCK)
      {
        dirty_grid[x + selected_block_pos.x][y + selected_block_pos.y] = UPDATE_SELECTED_BLOCK;
      }
    }
  }
}

void update_dirty_grid_full_block_grid()
{
  for(uint8_t y = 0; y < TETRIS_HEIGHT; y++)
  {
    for(uint8_t x = 0; x < TETRIS_WIDTH; x++)
    {
      if(block_grid[x][y] != TETRIS_NO_BLOCK)
      {
        dirty_grid[x][y] = UPDATE_GRID;
      }
    }
  }
}

void render_screen()
{
  const color565_t BLOCK_COLOR[] = {
    RGB_to_color(0xFF, 0xFF, 0xFF),
    RGB_to_color(0xFF, 0x00, 0x00),
    RGB_to_color(255, 192, 203),
    RGB_to_color(255, 20, 0)
  };

  for (uint8_t y = 0; y < TETRIS_HEIGHT; y++) 
  {
    for (uint8_t x = 0; x < TETRIS_WIDTH; x++)
    {
      switch (dirty_grid[x][y]) {
        case UPDATE_GRID:
          draw_rectangle((x * (TETRIS_BLOCK_SIZE + 1)) + 34, 
                (y * (TETRIS_BLOCK_SIZE + 1)) + 4, 
                TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 
                BLOCK_COLOR[block_grid[x][y]]);
          break;
        case UPDATE_SELECTED_BLOCK:
            draw_rectangle((x * (TETRIS_BLOCK_SIZE + 1)) + 34, 
                (y * (TETRIS_BLOCK_SIZE + 1)) + 4, 
                TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 
               BLOCK_COLOR[current_block.color]);
          break;
        case NONE:
          break;
      }

      dirty_grid[x][y] = NONE;
    }
  }
}
