#include "raylib.h"
#include "GUI/login.h"
#include "GUI/library_gui.h"
#include "GUI/ui_font.h"
#include <vector>

Font app_font;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1220, 750, "PBL2 - Quản lý thư viện");
    SetWindowMinSize(900, 600);
    SetTargetFPS(60);

    // Nap day du ky tu tieng Viet. Neu truyen nullptr, raylib chi nap ASCII
    // nen cac chu co dau se bi hien thanh dau '?'.
    std::vector<int> codepoints;
    for (int c = 32; c <= 126; ++c) codepoints.push_back(c);          // ASCII
    for (int c = 0xA0; c <= 0x17F; ++c) codepoints.push_back(c);      // Latin-1, Latin mo rong A (A, D, I, U...)
    codepoints.push_back(0x1A0); codepoints.push_back(0x1A1);         // O, o co rau
    codepoints.push_back(0x1AF); codepoints.push_back(0x1B0);         // U, u co rau
    for (int c = 0x1EA0; c <= 0x1EF9; ++c) codepoints.push_back(c);   // Latin mo rong bo sung (dau thanh)
    codepoints.push_back(0x2013); codepoints.push_back(0x2014);       // gach ngang
    codepoints.push_back(0x2026);                                     // dau ba cham

    app_font = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 64,
                          codepoints.data(), (int)codepoints.size());
    bool custom_font = app_font.texture.id != 0;
    if (!custom_font) app_font = GetFontDefault();   // du phong neu khong tim thay font
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

    if (custom_font) UnloadFont(app_font);
    CloseWindow();
    return 0;
}