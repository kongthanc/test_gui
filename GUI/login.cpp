#include "login.h"
#include "raylib.h"
#include "windows_text_input.h"
#include <cstring>
#include "ui_font.h"
#include "ui_layout.h"

namespace {
    // Nhận bất kỳ ký tự nào người dùng gõ vào (kể cả tiếng Việt có dấu)
    void append_codepoint(char* destination, int capacity, int codepoint) {
        if (destination == nullptr || capacity <= 1) return;
        if (codepoint < 32 || codepoint > 0x10FFFF ||
            (codepoint >= 0xD800 && codepoint <= 0xDFFF)) return;

        int byte_count = 0;
        const char* encoded = CodepointToUTF8(codepoint, &byte_count);
        if (encoded != nullptr && byte_count > 0) {
            int length = (int)strlen(destination);
            if (length + byte_count < capacity) {
                memcpy(destination + length, encoded, byte_count);
                destination[length + byte_count] = '\0';
            }
        }
    }

    // Hỗ trợ dán (Ctrl+V) giữ nguyên chuỗi
    void append_utf8(char* destination, int capacity, const char* source) {
        if (destination == nullptr || source == nullptr || capacity <= 1) return;
        int length = (int)strlen(destination);
        int i = 0;
        while (source[i] != '\0') {
            unsigned char first = (unsigned char)source[i];
            int count = 1;
            if (first < 0x80) count = 1;
            else if ((first & 0xE0) == 0xC0) count = 2;
            else if ((first & 0xF0) == 0xE0) count = 3;
            else if ((first & 0xF8) == 0xF0) count = 4;
            else { ++i; continue; }

            if (count == 1 && first < 32) { ++i; continue; }
            if (length + count >= capacity) break;

            memcpy(destination + length, source + i, count);
            length += count;
            destination[length] = '\0';
            i += count;
        }
    }

    // Xóa đúng 1 ký tự UTF-8 ở cuối khi bấm Backspace
    void erase_last_utf8_character(char* value) {
        if (value == nullptr) return;
        int length = (int)strlen(value);
        if (length == 0) return;

        --length;
        while (length > 0 && (((unsigned char)value[length]) & 0xC0) == 0x80) {
            --length;
        }
        value[length] = '\0';
    }

    // Đếm đúng số lượng chữ cái UTF-8 để hiển thị dấu '*' cho mật khẩu
    size_t count_utf8_chars(const char* str) {
        if (str == nullptr) return 0;
        size_t count = 0;
        for (size_t i = 0; str[i] != '\0'; ++i) {
            if ((((unsigned char)str[i]) & 0xC0) != 0x80) {
                ++count;
            }
        }
        return count;
    }
}

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
        if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
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

    bool control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
    if (control && IsKeyPressed(KEY_V)) {
        const char* clipboard = GetClipboardText();
        if (clipboard != nullptr) {
            if (focus_id) append_utf8(id, sizeof(id), clipboard);
            else if (focus_password) append_utf8(password, sizeof(password), clipboard);
        }
        return;
    }

    windows_text_input::input_event event;
    while (windows_text_input::pop(event)) {
        if (event.type == windows_text_input::event_type::backspace) {
            if (focus_id) erase_last_utf8_character(id);
            else if (focus_password) erase_last_utf8_character(password);
        } else if (event.type == windows_text_input::event_type::character) {
            if (focus_id) append_codepoint(id, sizeof(id), event.codepoint);
            else if (focus_password) append_codepoint(password, sizeof(password), event.codepoint);
        }
    }
}

void login_screen::draw_input_box(
    int x, int y, int w, int h,
    const char* label, const char* value, bool focused) {

    DrawText(label, x, y - 30, 20, DARKGRAY);

    DrawRectangleRounded({(float)x, (float)y, (float)w, (float)h}, 0.12f, 8, WHITE);
    DrawRectangleRoundedLines({(float)x, (float)y, (float)w, (float)h}, 0.12f, 8, focused ? BLUE : LIGHTGRAY);

    Vector2 top_left = GetWorldToScreen2D(Vector2{(float)x + 16, (float)y}, ui_layout::camera());
    Vector2 bottom_right = GetWorldToScreen2D(Vector2{(float)x + w - 60, (float)y + h}, ui_layout::camera());
    
    BeginScissorMode((int)top_left.x, (int)top_left.y, 
                     (int)(bottom_right.x - top_left.x), (int)(bottom_right.y - top_left.y));

    int text_width = MeasureText(value, 22);
    int max_visible_width = w - 76; 
    int text_x = x + 16;
    
    if (focused && text_width > max_visible_width) {
        text_x = x + 16 - (text_width - max_visible_width);
    }

    DrawText(value, text_x, y + 17, 22, DARKGRAY);
    EndScissorMode();
}

void login_screen::draw() {
    handle_input();

    BeginDrawing();
    ClearBackground({245, 247, 251, 255});
    BeginMode2D(ui_layout::camera());

    if (logged_in) {
        DrawText("ĐĂNG NHẬP THÀNH CÔNG", 410, 240, 34, DARKGREEN);
        DrawText(TextFormat("Vai trò: %s", role == "manager" ? "Quản lý" : role == "admin" ? "Quản trị viên" : "Độc giả"),
                 490, 310, 28, DARKBLUE);
        DrawText("Nhấn ENTER hoặc ESC để đăng xuất", 400, 400, 22, GRAY);
    } else {
        DrawRectangleRounded({360, 95, 500, 555}, 0.06f, 12, WHITE);

        int title_w = MeasureText("QUẢN LÝ THƯ VIỆN", 32);
        DrawText("QUẢN LÝ THƯ VIỆN", 360 + (500 - title_w) / 2, 135, 32, DARKBLUE);

        int sub_w = MeasureText("ĐĂNG NHẬP HỆ THỐNG", 21);
        DrawText("ĐĂNG NHẬP HỆ THỐNG", 360 + (500 - sub_w) / 2, 185, 21, GRAY);

        draw_input_box(415, 270, 390, 56, "Mã tài khoản", id, focus_id);

        std::string displayed_password;
        if (show_password) {
            displayed_password = password;
        } else {
            displayed_password.assign(count_utf8_chars(password), '*');
        }
        draw_input_box(415, 375, 390, 56, "Mật khẩu", displayed_password.c_str(), focus_password);

        Rectangle eye = {740, 380, 55, 46};
        DrawText(show_password ? "ẨN" : "HIỆN", 745, 393, 18, DARKBLUE);

        Rectangle button = {415, 465, 390, 54};
        DrawRectangleRounded(button, 0.15f, 8, BLUE);
        
        int btn_w = MeasureText("ĐĂNG NHẬP", 22);
        DrawText("ĐĂNG NHẬP", 415 + (390 - btn_w) / 2, 481, 22, WHITE);

        DrawText("TAB: chuyển ô | ENTER: đăng nhập", 415, 540, 18, GRAY);

        if (!message.empty()) {
            Color color = message == "Đăng nhập thành công!" ? DARKGREEN : RED;
            DrawText(message.c_str(), 415, 580, 19, color);
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            Vector2 mouse = ui_layout::mouse_position();
            if (CheckCollisionPointRec(mouse, eye)) {
                show_password = !show_password;
            } else if (CheckCollisionPointRec(mouse, button)) {
                try_login();
            } else if (CheckCollisionPointRec(mouse, {415, 270, 390, 56})) {
                focus_id = true;
                focus_password = false;
            } else if (CheckCollisionPointRec(mouse, {415, 375, 390, 56})) {
                focus_id = false;
                focus_password = true;
            }
        }
    }

    EndMode2D();
    EndDrawing();
}

bool login_screen::is_logged_in() const { return logged_in; }
std::string login_screen::get_role() const { return role; }

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