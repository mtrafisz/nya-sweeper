#include "raylib.h"
#include "../config.h"
#include "sweeper.h"

int main(void) {
    InitWindow((int)window_size_px_x, (int)window_size_px_y, "Nya Sweeper");
    InitAudioDevice();

    sweeper_t sweeper = sweeper_init();
    sweeper_reset(&sweeper);

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_R)) {
            sweeper_reset(&sweeper);
        }

        sweeper_update(&sweeper, GetFrameTime());
        BeginDrawing();
        ClearBackground(RAYWHITE);
        sweeper_draw(&sweeper);
        EndDrawing();
    }

    sweeper_destroy(&sweeper);
    CloseWindow();
    CloseAudioDevice();
    return 0;
}
