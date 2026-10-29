#include "raylib.h"
#include "GUI/login.h"
#include "GUI/library_gui.h"
#include "GUI/ui_font.h"

Font app_font;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1220, 750, "PBL2 - Quan ly thu vien");
    SetWindowMinSize(900, 600);
    SetTargetFPS(60);

    app_font = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 64, nullptr, 0);
    SetTextureFilter(app_font.texture, TEXTURE_FILTER_BILINEAR);

    library_controller controller;
    login_screen login(controller);
    library_gui app(controller);

    while (!WindowShouldClose()) {
        if (!login.is_logged_in()) {
            app.reset();
            login.draw();
            continue;
        }

        app.draw(login.get_role());

        if (app.wants_logout()) {
            login.reset();
            app.reset();
        }
    }

    UnloadFont(app_font);
    CloseWindow();
    return 0;
}