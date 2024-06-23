#ifndef _SWEEPER_H
#define _SWEEPER_H

#include "raylib.h"
#include "../config.h"
#include "resources.h"

enum _board_cell_state_e {
    CELL_UNTOUCHED,
    CELL_UNCOVERED,
    CELL_FLAGGED,
};

enum _game_state_e {
    GAME_NOT_STARTED,
    GAME_PLAYING,
    GAME_WON,
    GAME_LOST,
};

#define CELL_BOMB 9

typedef struct _board_cell_s {
    int cell_value;  // 0-8 = number of bombs around, 9 = bomb
    enum _board_cell_state_e state;
} board_cell_t;

typedef struct _sweeper_s {
    board_cell_t** board;
    struct _resources_s resources;
    Vector2 highlighted_cell;
    Vector2 clicked_cell;
    enum _game_state_e game_state;
    bool first_click;
    bool reset_button_down;
    float time_elapsed;
    float tick_timer;
} sweeper_t;

sweeper_t sweeper_init(void);
void sweeper_destroy(sweeper_t *sweeper);   // unload resources, cleanup, etc

void sweeper_reset(sweeper_t *sweeper);     // set up a new game

void sweeper_draw(sweeper_t *sweeper);
void sweeper_update(sweeper_t *sweeper, float delta_time);

#endif
