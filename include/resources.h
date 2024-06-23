#ifndef _RESOURCES_H
#define _RESOURCES_H

#include "raylib.h"

typedef enum _spritesheet_sprites_e {
    SPRITE_TILE_EMPTY,
    SPRITE_TILE_BORDER_DARK,
    SPRITE_TILE_BORDER_DARK_THICK,
    SPRITE_TILE_BORDER_DARK_CORNER,
    SPRITE_TILE_BORDER_DARK_CORNER_THICK,
    SPRITE_TILE_BORDER_LIGHT,
    SPRITE_TILE_BORDER_LIGHT_THICK,
    SPRITE_TILE_BORDER_LIGHT_CORNER,
    SPRITE_TILE_BORDER_LIGHT_CORNER_THICK,
    SPRITE_TILE_BORDER_MIXED_CORNER,
    SPRITE_TILE_BORDER_MIXED_CORNER_THICK,
    SPRITE_NYAA,
    SPRITE_UOOOGH,
    SPRITE_NANYAJAAN,
    SPRITE_CELL,
    SPRITE_FLAG,
    SPRITE_BOMB,
} spritesheet_sprites_t;

typedef enum _sound_effects_e {
    SFX_TIME_TICK,
    SFX_GAME_OVER,
    SFX_GAME_WIN,
    SFX_UNUSED1,
    SFX_CELL_CLICK,
    
    SFX_COUNT
} sound_effects_t;

struct _resources_s {
    Texture2D spritesheet;
    Sound sound_effects[SFX_COUNT];
};

void resources_load(struct _resources_s *resources);
void resources_unload(struct _resources_s *resources);

Rectangle resources_get_sprite_rect(spritesheet_sprites_t sprite, bool flip_x, bool flip_y);

void resources_play_sound(struct _resources_s *resources, sound_effects_t sound);

#endif