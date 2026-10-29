#include "login.h"
#include "raylib.h"
#include <cstring>
#include "ui_font.h"
#include "ui_layout.h"

login_screen::login_screen(library_controller& c)
    : controller(c), focus_id(true), focus_password(false),
      show_password(false), logged_in(false) {
    id[0] = '\0';
    password[0] = '\0';
}

void login_screen::try_login() {
    if (id[0] == '\0' || password[0] == '\0') {
        message = "Vui long nhap day du thong tin!";
        return;
    }

    if (controller.login(id, password)) {
        logged_in = true;
        role = controller.get_current_role();
        message = "Dang nhap thanh cong!";
    } else {
        message = "Sai ma tai khoan hoac mat khau!";
    }
}

void login_screen::handle_input() {
    if (logged_in) {
        if (IsKeyPressed(KEY_ENTER) ||
            IsKeyPressed(KEY_ESCAPE)) {
            controller.logout();
            logged_in = false;
            role.clear();
            message.clear();
            id[0] = '\0';
            password[0] = '\0';
            focus_id = true;
            focus_password = false;
            show_password = false;
        }
        return;
    }

    if (IsKeyPressed(KEY_TAB)) {
        focus_id = !focus_id;
        focus_password = !focus_password;
    }

    if (IsKeyPressed(KEY_ENTER)) {
        try_login();
        return;
    }

    int key = GetCharPressed();

    while (key > 0) {
        if (key >= 32 && key <= 126) {
            if (focus_id) {
                int len = (int)strlen(id);
                if (len < (int)sizeof(id) - 1) {
                    id[len] = (char)key;
                    id[len + 1] = '\0';
                }
            } else if (focus_password) {
                int len = (int)strlen(password);
                if (len < (int)sizeof(password) - 1) {
                    password[len] = (char)key;
                    password[len + 1] = '\0';
                }
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (focus_id) {
            int len = (int)strlen(id);
            if (len > 0) id[len - 1] = '\0';
        } else if (focus_password) {
            int len = (int)strlen(password);
            if (len > 0) password[len - 1] = '\0';
        }
    }
}

void login_screen::draw_input_box(
    int x, int y, int w, int h,
    const char* label, const char* value, bool focused) {

    DrawText(label, x, y - 30, 20, DARKGRAY);

    DrawRectangleRounded(
        {(float)x, (float)y, (float)w, (float)h},
        0.12f, 8, WHITE);

    DrawRectangleRoundedLines(
        {(float)x, (float)y, (float)w, (float)h},
        0.12f, 8, focused ? BLUE : LIGHTGRAY);

    DrawText(value, x + 16, y + 17, 22, DARKGRAY);
}

void login_screen::draw() {
    handle_input();

    BeginDrawing();
    ClearBackground({245, 247, 251, 255});

    BeginMode2D(ui_layout::camera());

    if (logged_in) {
        DrawText("DANG NHAP THANH CONG",
                 410, 240, 34, DARKGREEN);

        DrawText(TextFormat("Vai tro: %s", role.c_str()),
                 490, 310, 28, DARKBLUE);

        DrawText("Nhan ENTER hoac ESC de dang xuat",
                 400, 400, 22, GRAY);
    } else {
        DrawRectangleRounded(
            {360, 95, 500, 555}, 0.06f, 12, WHITE);

        int title_w = MeasureText("QUAN LY THU VIEN", 32);
        DrawText("QUAN LY THU VIEN",
                 360 + (500 - title_w) / 2, 135, 32, DARKBLUE);

        int sub_w = MeasureText("DANG NHAP HE THONG", 21);
        DrawText("DANG NHAP HE THONG",
                 360 + (500 - sub_w) / 2, 185, 21, GRAY);

        draw_input_box(
            415, 270, 390, 56,
            "Ma tai khoan", id, focus_id);

        std::string displayed_password;

        if (show_password) {
            displayed_password = password;
        } else {
            displayed_password.assign(strlen(password), '*');
        }

        draw_input_box(
            415, 375, 390, 56,
            "Mat khau", displayed_password.c_str(),
            focus_password);

        Rectangle eye = {740, 380, 55, 46};

        DrawText(show_password ? "AN" : "HIEN",
                 745, 393, 18, DARKBLUE);

        Rectangle button = {415, 465, 390, 54};

        DrawRectangleRounded(button, 0.15f, 8, BLUE);
        int btn_w = MeasureText("DANG NHAP", 22);
        DrawText("DANG NHAP", 415 + (390 - btn_w) / 2, 481, 22, WHITE);

        DrawText("TAB: chuyen o | ENTER: dang nhap",
                 415, 540, 18, GRAY);

        if (!message.empty()) {
            Color color = message == "Dang nhap thanh cong!"
                          ? DARKGREEN : RED;
            DrawText(message.c_str(), 415, 580, 19, color);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = ui_layout::mouse_position();

            if (CheckCollisionPointRec(mouse, eye)) {
                show_password = !show_password;
            } else if (CheckCollisionPointRec(mouse, button)) {
                try_login();
            } else if (CheckCollisionPointRec(
                           mouse, {415, 270, 390, 56})) {
                focus_id = true;
                focus_password = false;
            } else if (CheckCollisionPointRec(
                           mouse, {415, 375, 390, 56})) {
                focus_id = false;
                focus_password = true;
            }
        }
    }

    EndMode2D();
    EndDrawing();
}

bool login_screen::is_logged_in() const {
    return logged_in;
}

std::string login_screen::get_role() const {
    return role;
}

void login_screen::reset() {
    controller.logout();
    logged_in = false;
    role = "";
    message = "";
    id[0] = '\0';
    password[0] = '\0';
    show_password = false;
    focus_id = true;
    focus_password = false;
}