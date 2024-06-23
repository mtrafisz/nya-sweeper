#include "sweeper.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

int count_adj_bombs(board_cell_t** board, size_t cx, size_t cy) {
    int count = 0;
    for (int y = -1; y <= 1; ++y) {
        for (int x = -1; x <= 1; ++x) {
            if (x == 0 && y == 0) {
                continue;
            }
            if (cx + x < 0 || cx + x >= board_cells_x) {
                continue;
            }
            if (cy + y < 0 || cy + y >= board_cells_y) {
                continue;
            }
            if (board[cy + y][cx + x].cell_value == CELL_BOMB) {
                ++count;
            }
        }
    }
    return count;
}

int count_flagged_cells(sweeper_t* sweeper) {
    int count = 0;
    for (size_t y = 0; y < board_cells_y; ++y) {
        for (size_t x = 0; x < board_cells_x; ++x) {
            if (sweeper->board[y][x].state == CELL_FLAGGED) {
                ++count;
            }
        }
    }
    return count;
}

bool is_every_bomb_flagged(sweeper_t* sweeper) {
    for (size_t y = 0; y < board_cells_y; ++y) {
        for (size_t x = 0; x < board_cells_x; ++x) {
            if (sweeper->board[y][x].cell_value == CELL_BOMB) {
                if (sweeper->board[y][x].state != CELL_FLAGGED) {
                    return false;
                }
            }
        }
    }
    return true;
}

void dbg_print_board(board_cell_t** board) {
    for (size_t y = 0; y < board_cells_y; ++y) {
        for (size_t x = 0; x < board_cells_x; ++x) {
            printf("%d ", board[y][x].cell_value);
        }
        printf("\n");
    }
}

void sweeper_propagate_click(sweeper_t *sweeper, size_t x, size_t y) {
    if (x < 0 || x >= board_cells_x || y < 0 || y >= board_cells_y) {
        return;
    }
    if (sweeper->board[y][x].state == CELL_UNCOVERED || sweeper->board[y][x].state == CELL_FLAGGED) {
        return;
    }
    if (sweeper->board[y][x].cell_value == CELL_BOMB) {
        sweeper->game_state = GAME_LOST;
        return;
    }
    sweeper->board[y][x].state = CELL_UNCOVERED;
    if (sweeper->board[y][x].cell_value == 0) {
        sweeper_propagate_click(sweeper, x - 1, y - 1);
        sweeper_propagate_click(sweeper, x, y - 1);
        sweeper_propagate_click(sweeper, x + 1, y - 1);
        sweeper_propagate_click(sweeper, x - 1, y);
        sweeper_propagate_click(sweeper, x + 1, y);
        sweeper_propagate_click(sweeper, x - 1, y + 1);
        sweeper_propagate_click(sweeper, x, y + 1);
        sweeper_propagate_click(sweeper, x + 1, y + 1);
    }
}

int count_cells_left(board_cell_t** board) {
    int count = 0;
    for (size_t y = 0; y < board_cells_y; ++y) {
        for (size_t x = 0; x < board_cells_x; ++x) {
            if (board[y][x].state == CELL_UNTOUCHED || board[y][x].state == CELL_FLAGGED) {
                ++count;
            }
        }
    }

    return count;
}

sweeper_t sweeper_init(void) {
    sweeper_t sweeper = {0};
    sweeper.board = malloc(sizeof(board_cell_t*) * board_cells_y);
    for (size_t i = 0; i < board_cells_y; ++i) {
        sweeper.board[i] = malloc(sizeof(board_cell_t) * board_cells_x);
    }
    resources_load(&sweeper.resources);
    
    return sweeper;
}

void sweeper_reset(sweeper_t *sweeper) {
    for (size_t y = 0; y < board_cells_y; ++y) {
        for (size_t x = 0; x < board_cells_x; ++x) {
            sweeper->board[y][x].cell_value = 0;
            sweeper->board[y][x].state = CELL_UNTOUCHED;
        }
    }

    // generate mines
    for (size_t i = 0; i < mines; ++i) {
        size_t x, y;
        do {
            x = GetRandomValue(0, board_cells_x - 1);
            y = GetRandomValue(0, board_cells_y - 1);
        } while (sweeper->board[y][x].cell_value == CELL_BOMB);
        sweeper->board[y][x].cell_value = CELL_BOMB;
    }

    // calculate numbers
    for (size_t y = 0; y < board_cells_y; ++y) {
        for (size_t x = 0; x < board_cells_x; ++x) {
            if (sweeper->board[y][x].cell_value == CELL_BOMB) {
                continue;
            }
            sweeper->board[y][x].cell_value = count_adj_bombs(sweeper->board, x, y);
        }
    }

    sweeper->game_state = GAME_NOT_STARTED;
    sweeper->highlighted_cell = (Vector2) {-1, -1};
    sweeper->clicked_cell = (Vector2) {-1, -1};
    sweeper->first_click = true;
    sweeper->time_elapsed = 0;
    sweeper->tick_timer = 0;
}

void sweeper_draw(sweeper_t *sweeper) {
    // Draw tilemap:
    for (size_t y = 0; y < window_cells_y; y++) {
        for (size_t x = 0; x < window_cells_x; x++) {
            size_t tile_rect_idx = SPRITE_TILE_EMPTY;
            float transformation_deg = 0;   // how to rotate texture
            bool flip_x = false;

            /* I FREAKING LOOOOVE ELSE IFs*/
            /* OOOUUUUUUGHGHGHG*/
            // fr, is there better way xD?
            // TODO: tilemap in file (generate to file and read?)
            // that would do the same thing, but with more code...

            // Draw border
            if (x == 0 && y == 0) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT_CORNER_THICK;
            } else if (x == window_cells_x - 1 && y == 0) {
                tile_rect_idx = SPRITE_TILE_BORDER_MIXED_CORNER_THICK;
                transformation_deg = 90.0f;
            } else if (y == 0) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT_THICK;
            } else if (x == 0 && y == window_cells_y - 1) {
                tile_rect_idx = SPRITE_TILE_BORDER_MIXED_CORNER_THICK;
                transformation_deg = 180.0f;
                flip_x = true;
            } else if (x == 0) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT_THICK;
                transformation_deg = -90.0f;
            } else if (x == window_cells_x - 1 && y == window_cells_y - 1) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK_CORNER_THICK;
                transformation_deg = 180.0f;
            } else if (x == window_cells_x - 1) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK_THICK;
                transformation_deg = 90.0f;
            } else if (y == window_cells_y - 1) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK_THICK;
                transformation_deg = 180.0f;
            }
            // Draw the info window (whole width but 3 tiles high)
            else if (x == 1 && y == 1) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK_CORNER;
            } else if (x == window_cells_x - 2 && y == 1) {
                tile_rect_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                flip_x = true;
            } else if (y == 1) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK;
            } else if (x == 1 && y == 3) {
                tile_rect_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                transformation_deg = -90.0f;
            } else if (x == window_cells_x - 2 && y == 3) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT_CORNER;
                transformation_deg = 180.0f;
            } else if (y == 3) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT;
                transformation_deg = 180.0f;
            } else if (x == 1 && y == 2) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK;
                transformation_deg = -90.0f;
            } else if (x == window_cells_x - 2 && y == 2) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT;
                transformation_deg = 90.0f;
            }
            // Draw the board border
            else if (x == 1 && y == 5) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK_CORNER;
            } else if (x == window_cells_x - 2 && y == 5) {
                tile_rect_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                flip_x = true;
            } else if (y == 5) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK;
            } else if (x == 1 && y == window_cells_y - 2) {
                tile_rect_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                transformation_deg = -90.0f;
            } else if (x == window_cells_x - 2 && y == window_cells_y - 2) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT_CORNER;
                transformation_deg = 180.0f;
            } else if (y == window_cells_y - 2) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT;
                transformation_deg = 180.0f;
            } else if (x == 1 && y > 5 && y < window_cells_y - 2) {
                tile_rect_idx = SPRITE_TILE_BORDER_DARK;
                transformation_deg = -90.0f;
            } else if (x == window_cells_x - 2 && y > 5 && y < window_cells_y - 2) {
                tile_rect_idx = SPRITE_TILE_BORDER_LIGHT;
                transformation_deg = 90.0f;
            }

            DrawTexturePro(
                sweeper->resources.spritesheet,
                resources_get_sprite_rect(tile_rect_idx, flip_x, false),
                (Rectangle) {
                    .x = x * TILE_SIZE_PX + TILE_SIZE_PX / 2,
                    .y = y * TILE_SIZE_PX + TILE_SIZE_PX / 2,
                    .width = TILE_SIZE_PX,
                    .height = TILE_SIZE_PX,
                },
                (Vector2) {
                    .x = TILE_SIZE_PX / 2,
                    .y = TILE_SIZE_PX / 2,
                },
                transformation_deg,
                WHITE
            );
        }
    }

    float cell_width = (board_cells_x * TILE_SIZE_PX - 4 - board_cells_x) / board_cells_x;
    float cell_height = (board_cells_y * TILE_SIZE_PX - 4 - board_cells_y) / board_cells_y;
    float efx_width = cell_width / 1.3;
    float efx_height = cell_height / 1.3;

    // Draw fine lines under the cell board:

    for (size_t y = 0; y < board_cells_y; ++y) {
        DrawLine(
            TILE_SIZE_PX + 2,
            5 * TILE_SIZE_PX + y * cell_height + y + 1,
            TILE_SIZE_PX + board_cells_x * TILE_SIZE_PX - 2,
            5 * TILE_SIZE_PX + y * cell_height + y + 1,
            (Color) {18, 18, 18, 100}
        );
    }

    for (size_t x = 0; x < board_cells_x; ++x) {
        DrawLine(
            TILE_SIZE_PX + x * cell_width + x + 1,
            5 * TILE_SIZE_PX + 2,
            TILE_SIZE_PX + x * cell_width + x + 1,
            5 * TILE_SIZE_PX + board_cells_y * TILE_SIZE_PX - 2,
            (Color) {18, 18, 18, 100}
        );
    }

    // Draw board cells
    for (size_t y = 0; y < board_cells_y; ++y) {
        for (size_t x = 0; x < board_cells_x; ++x) {
            bool bomb = sweeper->board[y][x].cell_value == CELL_BOMB;
            bool flag = sweeper->board[y][x].state == CELL_FLAGGED;
            bool uncovered = sweeper->board[y][x].state == CELL_UNCOVERED;
            bool clicked = ((int)sweeper->clicked_cell.x == x && (int)sweeper->clicked_cell.y == y);
            bool hovered = ((int)sweeper->highlighted_cell.x == x && (int)sweeper->highlighted_cell.y == y);

            Rectangle cell_rect = {
                .x = x * cell_width + 2 + TILE_SIZE_PX + x,
                .y = y * cell_height + 2 + 5 * TILE_SIZE_PX + y,
                .width = cell_width,
                .height = cell_height,
            };

            // for bomb, flag, etc
            Rectangle efx_rect = {
                .x = x * cell_width + 2 + TILE_SIZE_PX + x + cell_width / 2 - efx_width / 2,
                .y = y * cell_height + 2 + 5 * TILE_SIZE_PX + y + cell_height / 2 - efx_height / 2,
                .width = efx_width,
                .height = efx_height,
            };

            if (!uncovered && !clicked) {
                DrawTexturePro(
                    sweeper->resources.spritesheet,
                    resources_get_sprite_rect(SPRITE_CELL, false, false),
                    cell_rect,
                    (Vector2) {0},
                    0,
                    WHITE
                );
            }

            if (bomb && uncovered) {
                DrawTexturePro(
                    sweeper->resources.spritesheet,
                    resources_get_sprite_rect(SPRITE_BOMB, false, false),
                    efx_rect,
                    (Vector2) {0},
                    0,
                    WHITE
                );
            }

            if (!uncovered && flag) {
                DrawTexturePro(
                    sweeper->resources.spritesheet,
                    resources_get_sprite_rect(SPRITE_FLAG, false, false),
                    efx_rect,
                    (Vector2) {0},
                    0,
                    WHITE
                );
            }

            if (uncovered && !bomb && sweeper->board[y][x].cell_value > 0) {
                Color cell_colors[] = {
                    (Color) {0, 0, 255, 255},       // 1 - blue
                    (Color) {0, 255, 0, 255},       // 2 - green
                    (Color) {255, 0, 0, 255},       // 3 - red
                    (Color) {165, 65, 170, 255},    // 4 - purple
                    (Color) {155, 35, 35, 255},     // 5 - brown
                    (Color) {60, 150, 150, 255},    // 6 - cyan
                    (Color) {0, 0, 0, 255},         // 7 - black
                    (Color) {128, 128, 128, 255},   // 8 - gray

                };

                DrawText(
                    TextFormat("%d", sweeper->board[y][x].cell_value),
                    cell_rect.x + cell_rect.width / 2 - MeasureText(TextFormat("%d", sweeper->board[y][x].cell_value), 20) / 2,
                    cell_rect.y + cell_rect.height / 2 - 20 / 2,
                    20,
                    cell_colors[sweeper->board[y][x].cell_value - 1]
                );
            }

            if (hovered) {
                DrawRectangleRec(
                    (Rectangle) {
                        .x = x * cell_width + 2 + TILE_SIZE_PX + x,
                        .y = y * cell_height + 2 + 5 * TILE_SIZE_PX + y,
                        .width = cell_width,
                        .height = cell_height,
                    },
                    (Color) {255, 255, 255, 50}
                );
            }
        }
    }

    // (2 * TILE_SIZE_PX / 2) WTF? IF I DON'T DO THIS, IT DOESN'T EVEN DRAW THE SPRITES ???

    Rectangle btn_reset[4] = {
        (Rectangle) {
            .x = TILE_SIZE_PX + 1 + (window_cells_x - 3) * TILE_SIZE_PX / 2,
            .y = TILE_SIZE_PX + 1 + (2 * TILE_SIZE_PX / 2),
            .width = TILE_SIZE_PX,
            .height = TILE_SIZE_PX,
        },
        (Rectangle) {
            .x = TILE_SIZE_PX + 1 + (window_cells_x - 3) * TILE_SIZE_PX / 2,
            .y = TILE_SIZE_PX + 1 + 4 * TILE_SIZE_PX / 2,
            .width = TILE_SIZE_PX,
            .height = TILE_SIZE_PX,
        },
        (Rectangle) {
            .x = TILE_SIZE_PX + 1 + (window_cells_x - 1) * TILE_SIZE_PX / 2,
            .y = TILE_SIZE_PX + 1 + (2 * TILE_SIZE_PX / 2),
            .width = TILE_SIZE_PX,
            .height = TILE_SIZE_PX,
        },
        (Rectangle) {
            .x = TILE_SIZE_PX + 1 + (window_cells_x - 1) * TILE_SIZE_PX / 2,
            .y = TILE_SIZE_PX + 1 + 4 * TILE_SIZE_PX / 2,
            .width = TILE_SIZE_PX,
            .height = TILE_SIZE_PX,
        },
    };

    // Draw reset button:
    for (size_t i = 0; i < 4; ++i) {
        bool flip;
        float rotation;
        size_t sprite_idx;
        if (!sweeper->reset_button_down) {
            switch (i) {
                case 0:
                    flip = false;
                    rotation = 0;
                    sprite_idx = SPRITE_TILE_BORDER_LIGHT_CORNER;
                    break;
                case 1:
                    flip = true;
                    rotation = 180;
                    sprite_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                    break;
                case 2:
                    flip = false;
                    rotation = 90;
                    sprite_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                    break;
                case 3:
                    flip = true;
                    rotation = 90;
                    sprite_idx = SPRITE_TILE_BORDER_DARK_CORNER;
                    break;
            }
        } else {
            switch (i) {
                case 0:
                    flip = false;
                    rotation = 0;
                    sprite_idx = SPRITE_TILE_BORDER_DARK_CORNER;
                    break;
                case 1:
                    flip = false;
                    rotation = -90;
                    sprite_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                    break;
                case 2:
                    flip = true;
                    rotation = 0;
                    sprite_idx = SPRITE_TILE_BORDER_MIXED_CORNER;
                    break;
                case 3:
                    flip = true;
                    rotation = 90;
                    sprite_idx = SPRITE_TILE_BORDER_LIGHT_CORNER;
                    break;
            }
        }

        DrawTexturePro(
            sweeper->resources.spritesheet,
            resources_get_sprite_rect(sprite_idx, flip, false),
            btn_reset[i],
            (Vector2) {
                .x = TILE_SIZE_PX / 2,
                .y = TILE_SIZE_PX / 2,
            },
            rotation,
            WHITE
        );
    }

    // Draw reset button icon:
    size_t icon_idx;
    switch (sweeper->game_state) {
        case GAME_PLAYING:
        case GAME_NOT_STARTED:
            icon_idx = SPRITE_NYAA;
            break;
        case GAME_WON:
            icon_idx = SPRITE_NANYAJAAN;
            break;
        case GAME_LOST:
            icon_idx = SPRITE_UOOOGH;
            break;
    }

    // in the middle of the button:
    DrawTexturePro(
        sweeper->resources.spritesheet,
        resources_get_sprite_rect(icon_idx, false, false),
        (Rectangle) {
            .x = TILE_SIZE_PX + 1 + (window_cells_x - 2) * TILE_SIZE_PX / 2,
            .y = TILE_SIZE_PX + 1 + 3 * TILE_SIZE_PX / 2,
            .width = TILE_SIZE_PX,
            .height = TILE_SIZE_PX,
        },
        (Vector2) {
            .x = TILE_SIZE_PX / 2,
            .y = TILE_SIZE_PX / 2,
        },
        0,
        WHITE
    );

    int flags_left = mines - count_flagged_cells(sweeper);
    const text_font_size = TILE_SIZE_PX * 12/9;

    char flag_left_str[4] = {0};
    char time_str[6] = {0}; 

    sprintf(flag_left_str, "%03d", flags_left);
    sprintf(time_str, "%03.0f", sweeper->time_elapsed);

    DrawText(flag_left_str, TILE_SIZE_PX * 3/2, TILE_SIZE_PX * 2, text_font_size, BLACK);
    DrawText("flags", TILE_SIZE_PX * 3/2, TILE_SIZE_PX * 1.5, text_font_size / 2, BLACK);
    DrawText(time_str, window_size_px_x - TILE_SIZE_PX * 3/2 - MeasureText("000", text_font_size), TILE_SIZE_PX * 2, text_font_size, BLACK);
    DrawText("time", window_size_px_x - TILE_SIZE_PX * 3/2 - MeasureText("000", text_font_size), TILE_SIZE_PX * 1.5, text_font_size / 2, BLACK);
}

void sweeper_update(sweeper_t *sweeper, float delta_time) {
    if (sweeper->game_state == GAME_PLAYING || sweeper->game_state == GAME_NOT_STARTED) {

    if (sweeper->game_state == GAME_PLAYING) {
        sweeper->time_elapsed += delta_time;
        sweeper->tick_timer += delta_time;

        if (sweeper->tick_timer >= tick_interval) {
            sweeper->tick_timer = 0;
            resources_play_sound(&sweeper->resources, SFX_TIME_TICK);
        }
    }

    Vector2 mouse_pos = GetMousePosition();
    int board_x = (int)mouse_pos.x - TILE_SIZE_PX - 2;
    int board_y = (int)mouse_pos.y - 5 * TILE_SIZE_PX - 2;
    float cell_width = (board_cells_x * TILE_SIZE_PX - 4 - board_cells_x) / board_cells_x;
    float cell_height = (board_cells_y * TILE_SIZE_PX - 4 - board_cells_y) / board_cells_y;

    int cell_x = board_x / (cell_width + 1);
    int cell_y = board_y / (cell_height + 1);

    if (cell_x < 0 || cell_x >= board_cells_x || cell_y < 0 || cell_y >= board_cells_y) {
        sweeper->highlighted_cell = (Vector2) {-1, -1};
        sweeper->clicked_cell = (Vector2) {-1, -1};
        goto update_btns;
    }

    bool is_clicked_cell = ((int)sweeper->clicked_cell.x == cell_x && (int)sweeper->clicked_cell.y == cell_y);

    if (!IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
        !IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) {
        sweeper->highlighted_cell = (Vector2) {cell_x, cell_y};
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && sweeper->board[cell_y][cell_x].state != CELL_FLAGGED) {
        sweeper->clicked_cell = (Vector2) {cell_x, cell_y};
        // resources_play_sound(&sweeper->resources, SFX_CELL_CLICK); sounds weird
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (is_clicked_cell) {
            sweeper->game_state = GAME_NOT_STARTED ? GAME_PLAYING : sweeper->game_state;
            // reset untill first click is a '0'
            if (sweeper->first_click) {
                int resets = 0;
                while (sweeper->board[cell_y][cell_x].cell_value != 0) {
                    sweeper_reset(sweeper);
                    resets++;
                }
                sweeper->game_state = GAME_PLAYING;
                sweeper->first_click = false;
                TraceLog(LOG_INFO, "Resets: %d", resets);   // 0 - 14 - it isn't so bad
            }

            sweeper_propagate_click(sweeper, cell_x, cell_y);

            if (sweeper->game_state != GAME_LOST) {
                if (count_cells_left(sweeper->board) == mines || is_every_bomb_flagged(sweeper)) {
                    sweeper->game_state = GAME_WON;
                    resources_play_sound(&sweeper->resources, SFX_GAME_WIN);
                }
            } else {
                for (size_t y = 0; y < board_cells_y; ++y) {
                    for (size_t x = 0; x < board_cells_x; ++x) {
                        sweeper->board[y][x].state = CELL_UNCOVERED;
                    }
                }
                resources_play_sound(&sweeper->resources, SFX_GAME_OVER);
            }

        } else {
            sweeper->clicked_cell = (Vector2) {-1, -1};
        }
    }

    if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        if (sweeper->board[cell_y][cell_x].state == CELL_UNTOUCHED) {
            sweeper->board[cell_y][cell_x].state = CELL_FLAGGED;
        } else if (sweeper->board[cell_y][cell_x].state == CELL_FLAGGED) {
            sweeper->board[cell_y][cell_x].state = CELL_UNTOUCHED;
        }
    }
    }

update_btns:
    Rectangle button_rect = {
        .x = TILE_SIZE_PX + 1 + (window_cells_x - 3) * TILE_SIZE_PX / 2,
        .y = TILE_SIZE_PX + TILE_SIZE_PX / 2,
        .width = TILE_SIZE_PX * 2,
        .height = TILE_SIZE_PX * 2,
    };

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
        CheckCollisionPointRec(GetMousePosition(), button_rect)) {
        sweeper->reset_button_down = true;
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        if (CheckCollisionPointRec(GetMousePosition(), button_rect)) {
            sweeper_reset(sweeper);
        }
        sweeper->reset_button_down = false;
    }

    return;
}

void sweeper_destroy(sweeper_t *sweeper) {
    resources_unload(&sweeper->resources);
    free(sweeper->board);
}
