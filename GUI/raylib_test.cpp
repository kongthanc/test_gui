#include "raylib.h"

int main() {
    InitWindow(800, 450, "PBL2 GUI Test");
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Raylib is working!", 250, 200, 30, DARKBLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}