#include "raylib.h"
#include "GUI/login.h"
#include "GUI/library_gui.h"
#include "GUI/ui_font.h"
#include "GUI/windows_text_input.h"
#include <vector>

Font app_font;

int main() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(1220, 750, "PBL2 - Quản lý thư viện");
#ifdef _WIN32
    windows_text_input::install();
#endif
    SetWindowMinSize(900, 600);
    SetTargetFPS(60);

    // Nạp đầy đủ ký tự tiếng Việt. Nếu truyền nullptr, raylib chỉ nạp ASCII
    // nên các chữ có dấu sẽ bị hiện thành dấu '?'.
    std::vector<int> codepoints;
    for (int c = 32; c <= 126; ++c) codepoints.push_back(c);          // ASCII
    for (int c = 0xA0; c <= 0x17F; ++c) codepoints.push_back(c);      // Latin-1, Latin mở rộng A (A, D, I, U...)
    codepoints.push_back(0x1A0); codepoints.push_back(0x1A1);         // Ơ, ơ có râu
    codepoints.push_back(0x1AF); codepoints.push_back(0x1B0);         // Ư, ư có râu
    for (int c = 0x1EA0; c <= 0x1EF9; ++c) codepoints.push_back(c);   // Latin mở rộng bổ sung (dấu thanh)
    codepoints.push_back(0x2013); codepoints.push_back(0x2014);       // Dấu gạch ngang
    codepoints.push_back(0x2026);                                     // Dấu ba chấm

    app_font = LoadFontEx("C:/Windows/Fonts/segoeui.ttf", 64,
                          codepoints.data(), (int)codepoints.size());
    bool custom_font = app_font.texture.id != 0;
    if (!custom_font) app_font = GetFontDefault();   // Dự phòng nếu không tìm thấy font
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
#ifdef _WIN32
    windows_text_input::uninstall();
#endif
    CloseWindow();
    return 0;
}
