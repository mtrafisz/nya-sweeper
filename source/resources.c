#include "resources.h"

#include <stddef.h>

void resources_load(struct _resources_s *resources) {
    Image spritesheet_image = LoadImage(ASSETS_PATH "sprites/spritesheet.png");
    resources->spritesheet = LoadTextureFromImage(spritesheet_image);
    UnloadImage(spritesheet_image);

    for (int i = 0; i < SFX_COUNT; i++) {
        char path[64];
        sprintf(path, ASSETS_PATH "sfx/%d.wav", i+1);
        resources->sound_effects[i] = LoadSound(path);
    }
}

void resources_unload(struct _resources_s *resources) {
    UnloadTexture(resources->spritesheet);

    for (int i = 0; i < SFX_COUNT; i++) {
        UnloadSound(resources->sound_effects[i]);
    }
}

Rectangle resources_get_sprite_rect(spritesheet_sprites_t sprite, bool flip_x, bool flip_y) {
    Rectangle rects[] = {
        (Rectangle) {36 * 3, 28, 36, 36},                   // SPRITE_TILE_EMPTY,
        (Rectangle) {36 * 5, 28, 36, 36},                   // SPRITE_TILE_BORDER_DARK,
        (Rectangle) {36 * 9, 28, 36, 36},                   // SPRITE_TILE_BORDER_DARK_THICK,
        (Rectangle) {36 * 6, 28, 36, 36},                   // SPRITE_TILE_BORDER_DARK_CORNER,
        (Rectangle) {36 * 7, 28, 36, 36},                   // SPRITE_TILE_BORDER_DARK_CORNER_THICK,
        (Rectangle) {36 * 4, 28, 36, 36},                   // SPRITE_TILE_BORDER_LIGHT,
        (Rectangle) {36 * 1, 28, 36, 36},                   // SPRITE_TILE_BORDER_LIGHT_THICK,
        (Rectangle) {36 * 2, 28, 36, 36},                   // SPRITE_TILE_BORDER_LIGHT_CORNER,
        (Rectangle) {36 * 10, 28, 36, 36},                  // SPRITE_TILE_BORDER_LIGHT_CORNER_THICK,
        (Rectangle) {36 * 8, 28, 36, 36},                   // SPRITE_TILE_BORDER_MIXED_CORNER,
        (Rectangle) {36 * 0, 28, 36, 36},                   // SPRITE_TILE_BORDER_MIXED_CORNER_THICK,
        (Rectangle) {0, 0, 28, 28},                         // SPRITE_NYAA, NOT IN SPRITESHEET
        (Rectangle) {28 * 2, 0, 28, 28},                    // SPRITE_UOOOGH, NOT IN SPRITESHEET
        (Rectangle) {28, 0, 28, 28},                        // SPRITE_NANYAJAAN, NOT IN SPRITESHEET
        (Rectangle) {36 * 11, 32, 32, 32},                  // SPRITE_CELL,
        (Rectangle) {28 * 3, 10, 18, 18},                   // SPRITE_FLAG,
        (Rectangle) {36 * 11 + 32, 28 + 36 - 18, 18, 18},   // SPRITE_BOMB,
    };

    if (flip_x) {
        rects[sprite].width *= -1;
    }

    if (flip_y) {
        rects[sprite].height *= -1;
    }

    return rects[sprite];
}

void resources_play_sound(struct _resources_s *resources, sound_effects_t sound) {
    PlaySound(resources->sound_effects[sound]);
}
