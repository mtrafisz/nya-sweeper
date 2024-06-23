#ifndef _CONFIG_H
#define _CONFIG_H

// CONFIGURABLE

// controls scale of sprites, and through it main window resolution
#define SIZE_SCALE 1.0f

// size of the game board
#define board_cells_y 10
#define board_cells_x 10

// number of mines on the board
#define mines (board_cells_x * board_cells_y) / 5

// frequency of time tick sound effect
#define tick_interval 1.0f

// RELATED CALCULATIONS - NOT FOR CONFIGURATION

#define TILE_SIZE_PX (36 * SIZE_SCALE)
#define CELL_SIZE_PX (32 * SIZE_SCALE)
#define BTN_EFX_SIZE_PX (28 * SIZE_SCALE)

#define window_cells_x (board_cells_x + 2)
#define window_cells_y (board_cells_y + 6)

#define window_size_px_x (window_cells_x * TILE_SIZE_PX)
#define window_size_px_y (window_cells_y * TILE_SIZE_PX)

#endif
