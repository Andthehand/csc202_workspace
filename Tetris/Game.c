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
//    This file is used as the main game logic for tetris using some outside
//    helper function for input pulling and the such.
//
//    Reference: https://stackoverflow.com/questions/233850/tetris-piece-rotation-algorithm
//
// NOTES:
//    
//
// *****************************************************************************
//******************************************************************************
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
#include "Joystick.h"
#include "ST7735S.h"

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

#define TETRIS_PADDING                                                        1
#define TETRIS_X_OFFSET                                                      34
#define TETRIS_Y_OFFSET                                                       4


#define JOYSTICK_RIGHT_DEADBAND                                            3000
#define JOYSTICK_LEFT_DEADBAND                                             1000

//-----------------------------------------------------------------------------
// Define a structure to hold different data types used by the program
//-----------------------------------------------------------------------------
typedef struct {
  int8_t x;
  int8_t y;
} Vector2;

typedef enum {
  BLANK = 0,
  RED = 1,
} BLOCK_COLOR_INDEX;

typedef struct {
  int8_t blocks[4][4];
  BLOCK_COLOR_INDEX color;
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
void render_game_over();
void render_screen();

//-----------------------------------------------------------------------------
// Define global variables and structures here.
// NOTE: when possible avoid using global variables
//-----------------------------------------------------------------------------
uint8_t block_grid[TETRIS_WIDTH][TETRIS_HEIGHT] = {{0}};
CheckType dirty_grid[TETRIS_WIDTH][TETRIS_HEIGHT];

const Shape SHAPES[] = {
  {{ {1, 1, 1, 1}, {0, 0, 0, 0}, {0, 0, 0, 0}, {0, 0, 0, 0} }, RED, 4}, // I
  {{ {1, 1}, {1, 1} }, RED, 2},                                         // O
  {{ {0, 1, 1}, {1, 1, 0}, {0, 0, 0} }, RED, 3},                        // S
  {{ {1, 1, 0}, {0, 1, 1}, {0, 0, 0} }, RED, 3},                        // Z
  {{ {1, 0, 0}, {1, 1, 1}, {0, 0, 0} }, RED, 3},                        // L
  {{ {0, 0, 1}, {1, 1, 1}, {0, 0, 0} }, RED, 3},                        // J
  {{ {0, 1, 0}, {1, 1, 1}, {0, 0, 0} }, RED, 3}                         // T
};

Vector2 selected_block_pos = {0};
Shape current_block;
bool game_over = false;

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Initializes the game state, including setting up the dirty grid, 
//  selecting the initial block, and positioning it at the starting location.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void game_init(void)
{
  memset(dirty_grid, UPDATE_GRID, sizeof(dirty_grid));

  current_block = SHAPES[rand() % (sizeof(SHAPES) / sizeof(SHAPES[0]))];
  selected_block_pos.x = (TETRIS_WIDTH - current_block.size) / 2;
  selected_block_pos.y = 0;

  update_dirty_grid_move(selected_block_pos, selected_block_pos);
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  The main game loop responsible for rendering the screen, handling input,
//  updating game logic, and managing the timing of block movement and placement.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void game_loop(void)
{
  uint32_t loop_count = 0;
  while (!game_over) {
    render_screen();
    input_handler();

    loop_count++;
    if((loop_count % TETRIS_LOOPS_TILL_FALL) == 0)
    {
      loop_count = 0;
      
      bool moved = move(0, 1);
      if(!moved)
      {
        place_shape();
      }
    }

    msec_delay(TETRIS_LOOP_DELAY);
  }

  render_game_over();
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Handles user input from the joystick and push buttons to move, rotate, 
//  or accelerate the current block in the Tetris game. Ensures movements 
//  are valid and updates the game state accordingly.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
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
  
  if (has_s2_pressed()) {
    Shape original_shape = current_block;
    current_block = rotate_shape();

    if (can_move(0, 0)) {
      update_dirty_grid_rotate(&original_shape, &current_block);
    }
    else 
    {
      current_block = original_shape;
    }
  }

  if (is_s1_down() && can_move(0, 1)) {
    Vector2 previous_pos = selected_block_pos;
    selected_block_pos.y++;

    update_dirty_grid_move(previous_pos, selected_block_pos);
  }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Checks each row of the Tetris grid to determine if it is fully occupied.
//  If a full row is found, it clears the row, shifts rows above it downward, 
//  and updates the game state to reflect the changes.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void clear_rows() {
  for (int y = 0; y < TETRIS_HEIGHT; y++) {
    bool full_row = true;
    for (int x = 0; x < TETRIS_WIDTH; x++) {
      if (!block_grid[x][y]) {
        full_row = false;
      }
    }
    
    if (full_row) {
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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Rotates the current Tetris block 90 degrees clockwise. If the rotated block 
//  cannot be placed in its current position due to collisions, the rotation 
//  is canceled, and the block remains in its original orientation.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  Shape: The rotated block if the rotation is valid; otherwise, 
//         the original block.
// 
//-----------------------------------------------------------------------------
Shape rotate_shape() {
  Shape rotated = current_block;
  for (int y = 0; y < current_block.size; y++) {
    for (int x = 0; x < current_block.size; x++) {
      rotated.blocks[x][current_block.size - 1 - y] = current_block.blocks[y][x];
    }
  }

  return rotated;
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Places the current Tetris block on the game grid, marking its cells as 
//  occupied. Checks for and clears any completed rows, then spawns a new 
//  block at the starting position. Ends the game if the new block cannot be 
//  placed due to collisions.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Checks if the current Tetris block can be moved by the specified 
//  vertical (x_step) and horizontal (y_step) steps. This function verifies 
//  that the block does not collide with walls or other placed blocks.
//
// INPUT PARAMETERS:
//  x_step: The horizontal movement step (positive for down, negative for up).
//  y_step: The vertical movement step (positive for right, negative for left).
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  0 if the movement is blocked by the walls or other blocks.
//  1 if the block can move by the specified steps, 
// 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Attempts to move the current Tetris block by the specified vertical (x_step) 
//  and horizontal (y_step) steps. If the move is valid (no collisions), it 
//  updates the block's position and the game state/render state.
//
// INPUT PARAMETERS:
//  x_step: The horizontal movement step (positive for down, negative for up).
//  y_step: The vertical movement step (positive for right, negative for left).
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  bool: Returns true if the block was successfully moved, 
//        false if the move was blocked by walls or other blocks.
// 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Updates the dirty grid to reflect the movement of the current Tetris block. 
//  It marks the previous position of the block for re-rendering and sets 
//  the current position of the block as selected for rendering.
//
// INPUT PARAMETERS:
//  previous_pos: The previous position of the Tetris block before the move.
//  current_pos: The current position of the Tetris block after the move.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void update_dirty_grid_move(Vector2 previous_pos, Vector2 current_pos) 
{
  for(uint8_t y = 0; y < current_block.size; y++)
  {
    for(uint8_t x = 0; x < current_block.size; x++)
    {
      if(current_block.blocks[x][y] != TETRIS_NO_BLOCK)
      {
        dirty_grid[x + previous_pos.x][y + previous_pos.y] = UPDATE_GRID;
      }
    }
  }

  for(uint8_t y = 0; y < current_block.size; y++)
  {
    for(uint8_t x = 0; x < current_block.size; x++)
    {
      if(current_block.blocks[x][y] != TETRIS_NO_BLOCK)
      {
        dirty_grid[x + current_pos.x][y + current_pos.y] = UPDATE_SELECTED_BLOCK;
      }
    }
  }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Updates the dirty grid to reflect the rotation of the current Tetris block. 
//  It marks the previous shape's position for re-rendering and sets 
//  the rotated shape's position as selected for rendering.
//
// INPUT PARAMETERS:
//  previous_shape: A pointer to the block's shape before rotation.
//  current_shape: A pointer to the block's shape after rotation.
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Updates the dirty grid to reflect the presence of blocks in the main game grid.
//  It marks all occupied positions in the block grid for re-rendering.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Renders the "game over" screen by filling the block grid with a specific 
//  value to indicate the game has ended. It also updates the dirty grid and 
//  re-renders the screen in a delayed manner to create a visual effect.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void render_game_over() 
{
  memset(block_grid, 1, sizeof(block_grid));
  for(uint8_t x = 0; x < TETRIS_HEIGHT; x++)
  {
    memset(dirty_grid[x], UPDATE_GRID, sizeof(dirty_grid[0]));
    render_screen();
    msec_delay(TETRIS_FILL_DELAY);
  }
}

//-----------------------------------------------------------------------------
// DESCRIPTION:
//  Renders the Tetris game screen by drawing rectangles for each block on the 
//  grid. It updates the screen based on the dirty grid, which indicates which 
//  blocks need to be re-rendered. Different colors are used for the game blocks 
//  and the selected block.
//
// INPUT PARAMETERS:
//  none
//
// OUTPUT PARAMETERS:
//  none
//
// RETURN:
//  none
// 
//-----------------------------------------------------------------------------
void render_screen()
{
  const color565_t BLOCK_COLOR[] = {
    RGB_to_color(0xFF, 0xFF, 0xFF),
    RGB_to_color(0xFF, 0x00, 0x00),
  };

  for (uint8_t y = 0; y < TETRIS_HEIGHT; y++) 
  {
    for (uint8_t x = 0; x < TETRIS_WIDTH; x++)
    {
      switch (dirty_grid[x][y]) {
        case UPDATE_GRID:
          draw_rectangle((x * (TETRIS_BLOCK_SIZE + TETRIS_PADDING)) + TETRIS_X_OFFSET, 
                (y * (TETRIS_BLOCK_SIZE + TETRIS_PADDING)) + TETRIS_Y_OFFSET, 
                TETRIS_BLOCK_SIZE, TETRIS_BLOCK_SIZE, 
                BLOCK_COLOR[block_grid[x][y]]);
          break;
        case UPDATE_SELECTED_BLOCK:
            draw_rectangle((x * (TETRIS_BLOCK_SIZE + TETRIS_PADDING)) + TETRIS_X_OFFSET, 
                (y * (TETRIS_BLOCK_SIZE + TETRIS_PADDING)) + TETRIS_Y_OFFSET, 
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
