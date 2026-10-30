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
        message = "Vui lòng nhập đầy đủ thông tin!";
        return;
    }

    if (controller.login(id, password)) {
        logged_in = true;
        role = controller.get_current_role();
        message = "Đăng nhập thành công!";
    } else {
        message = "Sai mã tài khoản hoặc mật khẩu!";
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
        DrawText("ĐĂNG NHẬP THÀNH CÔNG",
                 410, 240, 34, DARKGREEN);

        DrawText(TextFormat("Vai trò: %s", role == "manager" ? "Quản lý" : role == "admin" ? "Quản trị viên" : "Độc giả"),
                 490, 310, 28, DARKBLUE);

        DrawText("Nhấn ENTER hoặc ESC để đăng xuất",
                 400, 400, 22, GRAY);
    } else {
        DrawRectangleRounded(
            {360, 95, 500, 555}, 0.06f, 12, WHITE);

        int title_w = MeasureText("QUẢN LÝ THƯ VIỆN", 32);
        DrawText("QUẢN LÝ THƯ VIỆN",
                 360 + (500 - title_w) / 2, 135, 32, DARKBLUE);

        int sub_w = MeasureText("ĐĂNG NHẬP HỆ THỐNG", 21);
        DrawText("ĐĂNG NHẬP HỆ THỐNG",
                 360 + (500 - sub_w) / 2, 185, 21, GRAY);

        draw_input_box(
            415, 270, 390, 56,
            "Mã tài khoản", id, focus_id);

        std::string displayed_password;

        if (show_password) {
            displayed_password = password;
        } else {
            displayed_password.assign(strlen(password), '*');
        }

        draw_input_box(
            415, 375, 390, 56,
            "Mật khẩu", displayed_password.c_str(),
            focus_password);

        Rectangle eye = {740, 380, 55, 46};

        DrawText(show_password ? "ẨN" : "HIỆN",
                 745, 393, 18, DARKBLUE);

        Rectangle button = {415, 465, 390, 54};

        DrawRectangleRounded(button, 0.15f, 8, BLUE);
        int btn_w = MeasureText("ĐĂNG NHẬP", 22);
        DrawText("ĐĂNG NHẬP", 415 + (390 - btn_w) / 2, 481, 22, WHITE);

        DrawText("TAB: chuyển ô | ENTER: đăng nhập",
                 415, 540, 18, GRAY);

        if (!message.empty()) {
            Color color = message == "Đăng nhập thành công!"
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