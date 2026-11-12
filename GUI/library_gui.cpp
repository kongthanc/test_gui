#include "library_gui.h"
#include "raylib.h"
#include "windows_text_input.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include <algorithm>
#include "ui_font.h"
#include "ui_layout.h"

namespace {
    const int WINDOW_W = 1220;
    const int WINDOW_H = 750;
    const int SIDEBAR_W = 280;
    const int CONTENT_X = 305;
    const int CONTENT_W = 890;
    const int ROWS_PER_PAGE = 6;
    const int BOOKS_PER_PAGE = 4;
    const int SUGGESTIONS_PER_PAGE = 4;

    Color page_background = {245, 247, 251, 255};
    Color panel_background = {255, 255, 255, 255};
    Color sidebar_background = {235, 239, 246, 255};

    

    void draw_panel() {
        float extra_x = ui_layout::offset().x / ui_layout::scale();
        float extra_y = ui_layout::offset().y / ui_layout::scale();
        DrawRectangleRounded(
            {(float)CONTENT_X, 102.0f, (float)CONTENT_W, 578.0f},
            0.03f, 8, panel_background);

        // Kéo dãn khung trắng tràn ra lấp đầy viền
        DrawRectangleRounded(
            {(float)CONTENT_X, 102.0f, (float)CONTENT_W + extra_x, 578.0f + extra_y},
            0.03f, 8, panel_background);
    }

    void draw_table_header(const char* const* headers, const int* columns,
                           int count, int y) {
        DrawRectangle(CONTENT_X + 15, y, CONTENT_W - 30, 38, DARKBLUE);
        for (int i = 0; i < count; ++i) {
            DrawText(headers[i], columns[i], y + 9, 18, WHITE);
        }
    }

    void draw_paging_hint(int page, int total, int y,
                          int rows_per_page = ROWS_PER_PAGE) {
        int total_pages = total == 0 ? 1 :
            (total + rows_per_page - 1) / rows_per_page;
        DrawText(TextFormat("Trang %d/%d", page + 1, total_pages),
                 CONTENT_X + 395, y, 19, DARKGRAY);
    }

    std::string display_role(const std::string& role) {
        if (role == "manager") return "Quản lý";
        if (role == "admin") return "Quản trị viên";
        return "Độc giả";
    }

    std::string display_status(const std::string& status) {
        if (status == "Da tra") return "Đã trả";
        if (status == "Dang muon") return "Đang mượn";
        if (status == "Qua han") return "Quá hạn";
        if (status == "Cho duyet") return "Chờ duyệt";
        if (status == "Tu choi") return "Bị từ chối";
        return status;
    }

    std::string display_session(const std::string& session) {
        if (session == "sang") return "Sáng";
        if (session == "chieu") return "Chiều";
        return session;
    }

    std::string normalize_session(const char* value) {
        if (value == nullptr) return "";
        std::string session(value);
        if (session == "sáng" || session == "Sáng" ||
            session == "SANG" || session == "Sang" || session == "sang") {
            return "sang";
        }
        if (session == "chiều" || session == "Chiều" ||
            session == "CHIEU" || session == "Chieu" || session == "chieu") {
            return "chieu";
        }
        return "";
    }

    std::string format_date(const date& value) {
        return std::to_string(value.get_day()) + "/" +
               std::to_string(value.get_month()) + "/" +
               std::to_string(value.get_year());
    }

    std::string display_shelf(const std::string& shelf) {
        if (shelf.empty()) return "Chưa xếp kệ";
        return shelf;
    }

    void append_utf8(char* destination, int capacity, const char* source) {
        if (destination == nullptr || source == nullptr || capacity <= 1) return;

        int length = (int)std::strlen(destination);
        int i = 0;
        while (source[i] != '\0') {
            unsigned char first = (unsigned char)source[i];
            int count = 1;

            if (first < 0x80) count = 1;
            else if ((first & 0xE0) == 0xC0) count = 2;
            else if ((first & 0xF0) == 0xE0) count = 3;
            else if ((first & 0xF8) == 0xF0) count = 4;
            else { ++i; continue; }

            bool valid = true;
            for (int j = 1; j < count; ++j) {
                if (source[i + j] == '\0' ||
                    (((unsigned char)source[i + j]) & 0xC0) != 0x80) {
                    valid = false;
                    break;
                }
            }
            if (!valid) { ++i; continue; }

            // Bỏ qua ký tự điều khiển, nhưng vẫn giữ dấu cách.
            if (count == 1 && first < 32) {
                ++i;
                continue;
            }

            if (length + count >= capacity) break;
            std::memcpy(destination + length, source + i, count);
            length += count;
            destination[length] = '\0';
            i += count;
        }
    }

    void append_codepoint(char* destination, int capacity, int codepoint) {
        if (codepoint < 32 || codepoint > 0x10FFFF ||
            (codepoint >= 0xD800 && codepoint <= 0xDFFF)) return;

        int byte_count = 0;
        const char* encoded = CodepointToUTF8(codepoint, &byte_count);
        if (encoded != nullptr && byte_count > 0) {
            int length = (int)std::strlen(destination);
            if (length + byte_count < capacity) {
                std::memcpy(destination + length, encoded, byte_count);
                destination[length + byte_count] = '\0';
            }
        }
    }

    void erase_last_utf8_character(char* value) {
        int length = (int)std::strlen(value);
        if (length == 0) return;

        --length;
        while (length > 0 &&
               (((unsigned char)value[length]) & 0xC0) == 0x80) {
            --length;
        }
        value[length] = '\0';
    }
}

// Hàm cắt chuỗi UTF-8 an toàn cho tiếng Việt (đặt ở đầu file library_gui.cpp)
static std::string truncate_utf8(const std::string& str, size_t max_chars) {
    size_t char_count = 0;
    size_t byte_idx = 0;
    size_t len = str.length();

    while (byte_idx < len && char_count < max_chars) {
        unsigned char c = static_cast<unsigned char>(str[byte_idx]);
        if ((c & 0x80) == 0) byte_idx += 1;          // ASCII (1 byte)
        else if ((c & 0xE0) == 0xC0) byte_idx += 2;   // UTF-8 (2 bytes)
        else if ((c & 0xF0) == 0xE0) byte_idx += 3;   // UTF-8 (3 bytes tiếng Việt)
        else if ((c & 0xF8) == 0xF0) byte_idx += 4;   // UTF-8 (4 bytes)
        else byte_idx += 1;
        char_count++;
    }

    if (byte_idx < len) {
        return str.substr(0, byte_idx) + "...";
    }
    return str;
}

library_gui::library_gui(library_controller& controller)
    : controller(controller) {
    reset();
}

void library_gui::reset() {
    current_page = DASHBOARD;
    history.clear();
    logout_requested = false;
    list_page = 0;
    suggestion_page = 0;
    active_field = nullptr;
    active_field_capacity = 0;
    message.clear();
    message_is_error = false;
    showing_related = false;
    showing_pending_requests = false;
    selected_book_id.clear();
    confirm_open = false;
    confirm_just_opened = false;
    input_locked = false;
    confirm_input_enabled = false;
    confirm_title.clear();
    confirm_text.clear();
    confirm_ok_label.clear();
    confirm_ok_color = BLUE;
    confirm_action = nullptr;
    clear_fields();
    confirm_error.clear();

    // Dọn dẹp bổ sung cho Popup Chi tiết sách
    show_book_detail_popup = false;
    detail_book_id.clear();
    detail_borrow_page = 1;
    cached_currently_borrowed_count = 0;
}

bool library_gui::wants_logout() const {
    return logout_requested;
}

void library_gui::clear_fields() {
    field_a[0] = '\0';
    field_b[0] = '\0';
    field_c[0] = '\0';
    field_d[0] = '\0';
    field_e[0] = '\0';
    field_f[0] = '\0';
    field_g[0] = '\0';
    field_h[0] = '\0';
    active_field = nullptr;
    active_field_capacity = 0;
    list_page = 0;
    suggestion_page = 0;
    searched_user_id.clear();
}

void library_gui::switch_page(page next_page) {
    current_page = next_page;
    message.clear();
    message_is_error = false;
    showing_related = false;
    showing_pending_requests = false;
    clear_fields();
}

// Đi vào trang con (thêm/sửa/xóa...): nhớ trang hiện tại để nút "Quay lại" trở về đúng bước trước.
void library_gui::open_page(page next_page) {
    if (next_page == current_page) return;
    history.push_back(current_page);
    switch_page(next_page);
}

// Chọn mục từ menu bên trái: bắt đầu một luồng mới, "Quay lại" sẽ về Tổng quan.
void library_gui::open_root_page(page next_page) {
    history.clear();
    if (next_page != DASHBOARD) history.push_back(DASHBOARD);
    switch_page(next_page);
}

void library_gui::go_back() {
    if (history.empty()) {
        switch_page(DASHBOARD);
        return;
    }
    page previous = history.back();
    history.pop_back();
    switch_page(previous);
}

void library_gui::set_message(const std::string& text, bool is_error) {
    message = text;
    message_is_error = is_error;
}

void library_gui::ask_confirm(const std::string& title, const std::string& text,
                              const std::string& ok_label, Color ok_color,
                              std::function<void()> action) {
    confirm_open = true;
    confirm_just_opened = true;   // bo qua cú click vừa mở hộp thoại
    confirm_title = title;
    confirm_text = text;
    confirm_ok_label = ok_label;
    confirm_ok_color = ok_color;
    confirm_input_enabled = false;
    confirm_action = action;
}

void library_gui::ask_confirm_input(
    const std::string& title,
    const std::string& text,
    const std::string& ok_label,
    Color ok_color,
    std::function<void()> action) {

    field_g[0] = '\0';
    active_field = nullptr;
    active_field_capacity = 0;

    ask_confirm(title, text, ok_label, ok_color, action);
    confirm_input_enabled = true;
}

void library_gui::draw_confirm_dialog() {
    if (!confirm_open) return;

    // Hộp thoại tự xử lý nút của nó, nên mở khóa đầu vào cho nó.
    // Frame vừa mở thì vẫn khóa để cú click mở hộp thoại không bị tính là "Xác nhận".
    input_locked = confirm_just_opened;
    confirm_just_opened = false;

    // Thu thập bàn phím sau khi mở khóa đầu vào của hộp thoại.
    collect_text_input();

    DrawRectangle(0, 0, WINDOW_W, WINDOW_H, {0, 0, 0, 130});

    Rectangle box = {
        340, 200, 540,
        confirm_input_enabled ? 380.0f : 290.0f
    };

    DrawRectangleRounded(box, 0.05f, 10, WHITE);
    DrawRectangleRoundedLines(box, 0.05f, 10, DARKBLUE);

    DrawText(confirm_title.c_str(), (int)box.x + 28,
             (int)box.y + 22, 26, DARKBLUE);

    DrawRectangle((int)box.x + 28, (int)box.y + 60,
                  (int)box.width - 56, 2, LIGHTGRAY);

    // Tách nội dung theo dấu xuống dòng.
    std::string line;
    int line_index = 0;

    for (size_t i = 0; i <= confirm_text.size(); ++i) {
        if (i == confirm_text.size() || confirm_text[i] == '\n') {
            DrawText(line.c_str(), (int)box.x + 28,
                     (int)box.y + 78 + line_index * 30, 20, DARKGRAY);
            line.clear();
            ++line_index;
        } else {
            line += confirm_text[i];
        }
    }

    if (confirm_input_enabled) {
        // Đặt ô nhập bên dưới toàn bộ nội dung, tránh đè lên thông tin yêu cầu.
        draw_input(
            {box.x + 28, box.y + 220, box.width - 56, 44},
            "Lý do từ chối",
            field_g,
            sizeof(field_g)
        );

        // Hiển thị lỗi nhập liệu ngay trong hộp thoại.
        if (!confirm_error.empty()) {
            DrawText(confirm_error.c_str(),
                     (int)box.x + 28, (int)box.y + 275,
                     16, RED);
        }
    }

    float button_y = box.y + (confirm_input_enabled ? 310 : 220);

    bool cancelled = draw_button(
        {box.x + 60, button_y, 190, 48}, "Hủy", GRAY);

    bool confirmed = draw_button(
        {box.x + 290, button_y, 190, 48},
        confirm_ok_label.c_str(), confirm_ok_color
    );

    if (confirmed) {
        if (confirm_input_enabled && is_blank(field_g)) {
            // Không đóng hộp thoại nếu chưa nhập lý do.
            confirm_error = "Vui lòng nhập lý do từ chối.";
            return;
        }

        std::function<void()> action = confirm_action;

        confirm_open = false;
        confirm_input_enabled = false;
        confirm_error.clear();
        confirm_action = nullptr;

        if (action) action();

    } else if (cancelled) {
        confirm_open = false;
        confirm_input_enabled = false;
        confirm_error.clear();
        confirm_action = nullptr;
        active_field = nullptr;
        active_field_capacity = 0;

        set_message("Đã hủy thao tác.");
    }
}

bool library_gui::is_blank(const char* text) const {
    if (text == nullptr) return true;
    while (*text != '\0') {
        if (*text != ' ' && *text != '\t') return false;
        ++text;
    }
    return true;
}

bool library_gui::read_integer(const char* text, int& value,
                               int min_value, int max_value) const {
    if (is_blank(text)) return false;

    char* end = nullptr;
    long parsed = std::strtol(text, &end, 10);
    if (*end != '\0' || parsed < min_value || parsed > max_value) {
        return false;
    }

    value = (int)parsed;
    return true;
}

bool library_gui::read_date(const char* text, date& value) const {
    int day = 0;
    int month = 0;
    int year = 0;
    char extra = '\0';

    if (std::sscanf(text, "%d/%d/%d%c", &day, &month, &year, &extra) != 3) {
        return false;
    }

    try {
        // Cố gắng tạo đối tượng date, nếu sai ngày (ví dụ 29/02/2026) 
        // date.cpp sẽ "throw" lỗi ra ngoài
        date parsed(day, month, year);
        value = parsed;
        return true;
    } catch (const std::invalid_argument& e) {
        // "Hứng" lỗi tại đây để chương trình KHÔNG BỊ CRASH
        // Báo về cho GUI biết là ngày nhập không hợp lệ (trả về false)
        return false;
    }
}

void library_gui::collect_text_input() {
    if (input_locked || active_field == nullptr || active_field_capacity <= 1) return;

    bool control = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);

    // Dán UTF-8 trực tiếp từ clipboard.
    if (control && IsKeyPressed(KEY_V)) {
        const char* clipboard = GetClipboardText();
        if (clipboard != nullptr) {
            append_utf8(active_field, active_field_capacity, clipboard);
        }
        return;
    }

    // Xử lý sự kiện theo đúng thứ tự Windows gửi, tránh mất thứ tự Telex.
    windows_text_input::input_event event;
    while (windows_text_input::pop(event)) {
        if (event.type == windows_text_input::event_type::backspace) {
            erase_last_utf8_character(active_field);
        } else if (event.type == windows_text_input::event_type::character) {
            append_codepoint(active_field, active_field_capacity, event.codepoint);
        }
    }
}

void library_gui::draw_input(Rectangle box, const char* label,
                             char* value, int capacity) {
    DrawText(label, (int)box.x, (int)box.y - 24, 18, DARKGRAY);

    bool focused = active_field == value;
    DrawRectangleRounded(box, 0.08f, 6, WHITE);
    DrawRectangleRoundedLines(box, 0.08f, 6, focused ? BLUE : LIGHTGRAY);
    
    // Đồng bộ tọa độ Camera (World) sang tọa độ màn hình (Screen) cho ScissorMode
    Vector2 top_left = GetWorldToScreen2D(Vector2{box.x + 12, box.y}, ui_layout::camera());
    Vector2 bottom_right = GetWorldToScreen2D(Vector2{box.x + box.width - 12, box.y + box.height}, ui_layout::camera());
    
    // Bắt đầu chế độ cắt với tọa độ màn hình vật lý chính xác
    BeginScissorMode((int)top_left.x, (int)top_left.y, 
                     (int)(bottom_right.x - top_left.x), (int)(bottom_right.y - top_left.y));
    
    // Tính toán chiều rộng chữ để đẩy sang trái nếu chữ quá dài (giúp xem được chữ đang gõ)
    int text_width = MeasureText(value, 20);
    int max_visible_width = (int)box.width - 24;
    int text_x = (int)box.x + 12;
    
    if (focused && text_width > max_visible_width) {
        text_x = (int)box.x + 12 - (text_width - max_visible_width);
    }

    DrawText(value, text_x, (int)box.y + 11, 20, DARKGRAY);
    
    // Kết thúc chế độ cắt
    EndScissorMode();

    if (!input_locked &&
        CheckCollisionPointRec(ui_layout::mouse_position(), box) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        active_field = value;
        active_field_capacity = capacity;
    }
}

bool library_gui::draw_button(Rectangle box, const char* label, Color color) {
    Vector2 mouse = ui_layout::mouse_position();
    bool hovered = !input_locked && CheckCollisionPointRec(mouse, box);
    Color fill = hovered ? Color{(unsigned char)(color.r > 25 ? color.r - 25 : 0),
                                 (unsigned char)(color.g > 25 ? color.g - 25 : 0),
                                 (unsigned char)(color.b > 25 ? color.b - 25 : 0),
                                 255}
                         : color;
    DrawRectangleRounded(box, 0.12f, 6, fill);

    int font_size = 19;
    int text_width = MeasureText(label, font_size);
    DrawText(label, (int)(box.x + (box.width - text_width) / 2),
             (int)(box.y + (box.height - font_size) / 2), font_size, WHITE);

    return hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
}

void library_gui::draw_message() {
    if (message.empty()) return;
    DrawText(message.c_str(), CONTENT_X + 20, 695, 20,
             message_is_error ? RED : DARKGREEN);
}

void library_gui::draw_back_button() {
    float extra_x = ui_layout::offset().x / ui_layout::scale();
    if (current_page != DASHBOARD &&
        draw_button({CONTENT_X + CONTENT_W + extra_x - 150, 115, 130, 42}, "Quay lại", GRAY)) {
        go_back();
    }
}

void library_gui::draw_shell(const std::string& role) {
    float extra_x = ui_layout::offset().x / ui_layout::scale();
    float extra_y = ui_layout::offset().y / ui_layout::scale();

    ClearBackground(page_background);
    
    // 1. Header tràn 2 bên, chữ kéo sát mép trái
    DrawRectangle(-extra_x, -extra_y, WINDOW_W + extra_x * 2, 82 + extra_y, DARKBLUE);
    DrawText("PBL2 - QUẢN LÝ THƯ VIỆN", 28 - extra_x, 25, 28, WHITE);

    std::string role_name = display_role(role);
    int role_width = MeasureText(role_name.c_str(), 22);
    int role_x = WINDOW_W + extra_x - 28 - role_width; // Tên và Chuông neo sát mép phải

    if (role == "user") {
        user *current_user = controller.get_current_user();
        int unread = current_user == nullptr ? 0 : controller.get_unread_notification_count(current_user->get_id());

        Rectangle bell_box = { (float)(role_x - 72), 14, 48, 52 };
        Vector2 mouse = ui_layout::mouse_position();
        bool hovered = !input_locked && CheckCollisionPointRec(mouse, bell_box);

        DrawRectangleRounded(bell_box, 0.2f, 6, hovered ? BLUE : DARKBLUE);

        DrawCircle(role_x - 48, 34, 10, WHITE);
        DrawRectangle(role_x - 58, 34, 20, 12, WHITE);
        DrawRectangle(role_x - 60, 44, 24, 4, WHITE);
        DrawCircle(role_x - 48, 51, 3, WHITE);

        if (unread > 0) {
            DrawCircle(role_x - 31, 19, 10, RED);
            std::string count_text = unread > 99 ? "99+" : std::to_string(unread);
            int text_width = MeasureText(count_text.c_str(), 11);
            DrawText(count_text.c_str(), role_x - 31 - text_width / 2, 14, 11, WHITE);
        }

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            open_root_page(USER_NOTIFICATIONS);
        }
    }

    DrawText(role_name.c_str(), role_x, 29, 22, WHITE);

    // 2. Sidebar nền xám neo sát mép trái
    DrawRectangle(-extra_x, 82, SIDEBAR_W, WINDOW_H - 82 + extra_y, sidebar_background);
    DrawText("CHỨC NĂNG", 28 - extra_x, 110, 21, DARKGRAY);

    // 3. Khung trắng chính kéo giãn lấp đầy khoảng trống bên phải
    DrawRectangleRounded(
        {(float)CONTENT_X, 102.0f, (float)CONTENT_W + extra_x, 578.0f + extra_y},
        0.03f, 8, panel_background);
}

void library_gui::draw_sidebar(const std::string& role) {
    float extra_x = ui_layout::offset().x / ui_layout::scale();

    struct menu_item {
        const char* label;
        page destination;
    };

    std::vector<menu_item> menu;

    if (role == "manager") {
        menu = { {"Tổng quan", DASHBOARD}, {"Quản lý sách", BOOKS}, {"Phiếu mượn", BORROWS}, {"Yêu cầu mượn", REQUESTS}, {"Ca trực", SHIFTS}, {"Tính lương", SALARY}, {"Người dùng", USERS}, {"Vị trí kệ", SHELF} };
    } else if (role == "admin") {
        menu = { {"Tổng quan", DASHBOARD}, {"Quản lý sách", BOOKS}, {"Phiếu mượn", BORROWS}, {"Yêu cầu mượn", REQUESTS}, {"Lọc phiếu mượn", BORROW_FILTER}, {"Ca trực của tôi", SHIFTS}, {"Lương của tôi", SALARY}, {"Vị trí kệ", SHELF} };
    } else {
        menu = { {"Tổng quan", DASHBOARD}, {"Tìm kiếm sách", USER_SEARCH}, {"Đăng ký mượn", USER_REQUEST}, {"Vị trí kệ sách", SHELF}, {"Sách đang mượn", USER_ACTIVE_BORROWS}, {"Lịch sử mượn", USER_HISTORY} };
    }

    Vector2 mouse = ui_layout::mouse_position();
    for (int i = 0; i < (int)menu.size(); ++i) {
        // Dời tọa độ x của nút bấm trừ đi extra_x
        Rectangle item = {18 - extra_x, (float)(150 + i * 62), 244, 48};
        bool hovered = !input_locked && CheckCollisionPointRec(mouse, item);
        bool selected = current_page == menu[i].destination;

        if (selected) {
            DrawRectangleRounded(item, 0.12f, 6, BLUE);
        } else if (hovered) {
            DrawRectangleRounded(item, 0.12f, 6, {215, 225, 240, 255});
        }

        // Dời chữ bên trong nút bấm sang trái
        DrawText(menu[i].label, 32 - extra_x, 164 + i * 62, 20, selected ? WHITE : DARKGRAY);

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            open_root_page(menu[i].destination);
        }
    }

    // Dời nút Đăng xuất sang trái
    if (draw_button({28 - extra_x, 665, 224, 50}, "ĐĂNG XUẤT", RED)) {
        ask_confirm("Xác nhận đăng xuất",
                    "Bạn có chắc chắn muốn đăng xuất\nkhỏi hệ thống không?",
                    "Đăng xuất", RED,
                    [this]() { logout_requested = true; });
    }
}

void library_gui::draw_dashboard(const std::string& role) {
    DrawText("Tổng quan", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    user* current_user = controller.get_current_user();
    std::string name = current_user == nullptr ? "" : current_user->get_name();

    DrawText(TextFormat("Xin chào, %s!", name.c_str()),
             CONTENT_X + 35, 195, 28, DARKBLUE);

    if (role == "manager") {
        DrawText("Bạn có quyền quản lý sách, phiếu mượn, ca trực và tài khoản.",
                 CONTENT_X + 35, 250, 21, DARKGRAY);
    } else if (role == "admin") {
        DrawText("Bạn có quyền quản lý sách, phiếu mượn và ca trực cá nhân.",
                 CONTENT_X + 35, 250, 21, DARKGRAY);
    } else {
        DrawText("Dùng menu bên trái để tìm sách và quản lý yêu cầu mượn.",
                 CONTENT_X + 35, 250, 21, DARKGRAY);
    }
}

void library_gui::draw_books(const std::string& role, bool user_search) {
    DrawText(user_search ? "Tìm kiếm sách" : "Quản lý sách",
             CONTENT_X + 25, 118, 28, DARKBLUE);
    draw_back_button();

    // Ô tìm kiếm
    draw_input({CONTENT_X + 15, 168, 230, 38}, "Tên sách", field_a, sizeof(field_a));
    draw_input({CONTENT_X + 255, 168, 220, 38}, "Tên tác giả", field_b, sizeof(field_b));
    draw_input({CONTENT_X + 485, 168, 150, 38}, "Thể loại", field_c, sizeof(field_c));

    if (draw_button({CONTENT_X + 645, 168, 90, 38}, "Tìm")) {
        list_page = 0;
        selected_book_id.clear();
    }
    if (draw_button({CONTENT_X + 745, 168, 100, 38}, "Tất cả", GRAY)) {
        field_a[0] = '\0';
        field_b[0] = '\0';
        field_c[0] = '\0';
        list_page = 0;
        selected_book_id.clear();
    }

    if (!user_search) {
        if (draw_button({CONTENT_X + 15, 220, 125, 36}, "Thêm sách")) {
            open_page(BOOK_ADD);
            return;
        }
        if (draw_button({CONTENT_X + 150, 220, 125, 36}, "Sửa sách", ORANGE)) {
            open_page(BOOK_EDIT);
            return;
        }
        if (draw_button({CONTENT_X + 285, 220, 125, 36}, "Xóa sách", RED)) {
            open_page(BOOK_DELETE);
            return;
        }
        if (draw_button({CONTENT_X + 420, 220, 110, 36}, "Hoàn tác", PURPLE)) {
            std::string msg;
            bool ok = controller.undo_last_book_action(msg);
            set_message(msg, !ok);
            list_page = 0;
        }
    }

    linked_list<book> books = controller.filter_books(field_a, field_b, field_c);
    int total = books.get_size();
    
    const int ROWS_PER_PAGE_BOOK = 7;
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE_BOOK - 1) / ROWS_PER_PAGE_BOOK;
    if (list_page >= page_count) list_page = page_count - 1;

    int table_start_y = user_search ? 230 : 292; 
    DrawText("Kết quả tìm kiếm", CONTENT_X + 20, table_start_y - 24, 18, DARKGRAY);

    const char *headers[] = {"Mã", "Tên sách", "Tác giả", "Thể loại", "Còn", "Thao tác"};
    const int columns[] = {CONTENT_X + 25, CONTENT_X + 85, CONTENT_X + 355, CONTENT_X + 540, CONTENT_X + 650, CONTENT_X + 730};
    draw_table_header(headers, columns, 6, table_start_y);

    node<book>* current = books.get_head();
    int skip = list_page * ROWS_PER_PAGE_BOOK;
    while (current != nullptr && skip-- > 0) current = current->get_next();

    int row = 0;
    while (current != nullptr && row < ROWS_PER_PAGE_BOOK) {
        book item = current->get_data();
        int y = table_start_y + 38 + row * 37;
        Rectangle row_box = {CONTENT_X + 15.0f, (float)y, CONTENT_W - 30.0f, 34.0f};
        bool selected = item.get_id() == selected_book_id;
        DrawRectangleRec(row_box, selected ? Color{205, 225, 250, 255}
                                           : (row % 2 == 0 ? Color{245, 248, 252, 255}
                                                           : Color{232, 239, 248, 255}));

        DrawText(item.get_id().c_str(), columns[0], y + 8, 16, DARKGRAY);
        
        // Cắt UTF-8 an toàn cho tên sách và tác giả
        std::string title_disp = truncate_utf8(item.get_title(), 26);
        DrawText(title_disp.c_str(), columns[1], y + 8, 15, DARKGRAY);

        std::string author_disp = truncate_utf8(item.get_author(), 20);
        DrawText(author_disp.c_str(), columns[2], y + 8, 15, DARKGRAY);

        DrawText(item.get_category().c_str(), columns[3], y + 8, 15, DARKGRAY);
        DrawText(TextFormat("%d", item.get_quantity()), columns[4] + 8, y + 8, 16, DARKGREEN);

        bool can_borrow = user_search && (item.get_quantity() > 0);
        if (draw_button({(float)columns[5], (float)y + 3, 105, 28}, 
                        user_search ? "Mượn sách" : "Chi tiết", 
                        can_borrow || !user_search ? BLUE : GRAY) && (can_borrow || !user_search)) {
            if (user_search) {
                std::string book_id = item.get_id();
                open_page(USER_REQUEST);
                std::strncpy(field_b, book_id.c_str(), sizeof(field_b) - 1);
                field_b[sizeof(field_b) - 1] = '\0';
                return;
            } else {
                open_book_detail_popup(item.get_id());
            }
        }

        if (CheckCollisionPointRec(ui_layout::mouse_position(), row_box) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            selected_book_id = item.get_id();
        }
        current = current->get_next();
        ++row;
    }

    if (total == 0) {
        DrawText("Không tìm thấy sách phù hợp.", CONTENT_X + 20, table_start_y + 45, 18, RED);
    }

    int paging_y = 595; 

    if (draw_button({CONTENT_X + 230, (float)paging_y, 100, 34}, "Trước", GRAY) && list_page > 0) --list_page;
    if (draw_button({CONTENT_X + 550, (float)paging_y, 100, 34}, "Sau", GRAY) && list_page + 1 < page_count) ++list_page;
    draw_paging_hint(list_page, total, paging_y + 7, ROWS_PER_PAGE_BOOK);
}

void library_gui::draw_book_form(bool editing) {
    DrawText(editing ? "Sửa thông tin sách" : "Thêm sách mới",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (!editing) {
        draw_input({340, 195, 360, 44}, "Mã sách", field_a, sizeof(field_a));
        draw_input({740, 195, 400, 44}, "Tên sách", field_b, sizeof(field_b));
        draw_input({340, 280, 360, 44}, "Tác giả", field_c, sizeof(field_c));
        draw_input({740, 280, 400, 44}, "Thể loại", field_d, sizeof(field_d));
        draw_input({340, 365, 360, 44}, "Số lượng (0 - 10000)", field_e, sizeof(field_e));
        draw_input({740, 365, 400, 44}, "Năm xuất bản", field_f, sizeof(field_f));
        
        // Form lấy vị trí kệ
        draw_input({340, 450, 800, 44}, "Vị trí kệ sách (Ví dụ: Ke A1)", field_g, sizeof(field_g));

        if (draw_button({530, 540, 220, 48}, "Lưu sách")) {
            int quantity = 0;
            int year = 0;
            if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
                is_blank(field_d) || is_blank(field_g) ||
                !read_integer(field_e, quantity, 0, 10000) ||
                !read_integer(field_f, year, 1000, 9999)) {
                set_message("Nhập đầy đủ thông tin (cả kệ sách) và số hợp lệ.", true);
            } else if (controller.get_book_by_id(field_a) != nullptr) {
                set_message("Không thêm được: mã sách đã tồn tại.", true);
            } else {
                std::string id = field_a;
                std::string title = field_b;
                std::string author = field_c;
                std::string category = field_d;
                std::string shelf = field_g;
                ask_confirm("Xác nhận thêm sách",
                            "Thêm sách mới vào thư viện?\n"
                            "Mã sách: " + id + "\n"
                            "Tên sách: " + title,
                            "Thêm sách", BLUE,
                            [this, id, title, author, category, quantity, year, shelf]() {
                    // TRUYỀN ĐỦ 7 THAM SỐ (CÓ SHELF) ĐỂ HẾT BÁO LỖI
                    if (controller.add_new_book(id, title, author, category, quantity, year, shelf)) {
                        set_message("Đã thêm sách mới và lưu vị trí kệ.");
                        clear_fields();
                    } else {
                        set_message("Không thêm được: mã sách đã tồn tại.", true);
                    }
                });
            }
        }
        return;
    }

    // --- PHẦN EDITING (SỬA SÁCH) ---
    draw_input({340, 185, 360, 44}, "Mã sách cũ", field_a, sizeof(field_a));
    draw_input({740, 185, 400, 44}, "Mã sách mới", field_b, sizeof(field_b));
    draw_input({340, 260, 360, 44}, "Tên sách", field_c, sizeof(field_c));
    draw_input({740, 260, 400, 44}, "Tác giả", field_d, sizeof(field_d));
    draw_input({340, 335, 360, 44}, "Thể loại", field_e, sizeof(field_e));
    draw_input({740, 335, 400, 44}, "Số lượng (0 - 10000)", field_f, sizeof(field_f));
    
    // Đã thay thế biến static bằng field_h
    draw_input({340, 410, 360, 44}, "Năm xuất bản", field_h, sizeof(field_h));
    draw_input({740, 410, 400, 44}, "Vị trí kệ sách", field_g, sizeof(field_g));

    if (draw_button({530, 500, 220, 48}, "Cập nhật sách", ORANGE)) {
        int quantity = 0;
        int year = 0;
        if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
            is_blank(field_d) || is_blank(field_e) || is_blank(field_g) ||
            !read_integer(field_f, quantity, 0, 10000) ||
            !read_integer(field_h, year, 1000, 9999)) { // Sửa thành field_h
            set_message("Nhập đầy đủ thông tin và số hợp lệ.", true);
        } else if (controller.get_book_by_id(field_a) == nullptr) {
            set_message("Không tìm thấy mã sách cũ.", true);
        } else {
            std::string old_id = field_a;
            std::string new_id = field_b;
            if (old_id != new_id) {
                if (controller.is_book_borrowed(old_id)) {
                    set_message("Không thể đổi mã sách khi sách đang có người mượn.", true);
                    return;
                }
                if (controller.has_pending_request_for_book(old_id)) {
                    set_message("Không thể đổi mã sách khi còn yêu cầu mượn chờ duyệt.", true);
                    return;
                }
                if (controller.get_book_by_id(new_id) != nullptr) {
                    set_message("Mã sách mới đã tồn tại.", true);
                    return;
                }
            }
            std::string title = field_c;
            std::string author = field_d;
            std::string category = field_e;
            std::string shelf = field_g;
            ask_confirm("Xác nhận cập nhật sách",
                        "Cập nhật thông tin sách " + old_id + "?\n"
                        "Mã sách mới: " + new_id,
                        "Cập nhật", ORANGE,
                        [this, old_id, new_id, title, author, category, quantity, year, shelf]() {
                book replacement(new_id, title, author, category, quantity, year);
                if (controller.update_book_info(old_id, replacement)) {
                    set_message("Đã cập nhật thông tin sách.");
                    clear_fields(); // Dòng này sẽ xóa cả field_h
                } else {
                    set_message("Không thể cập nhật sách. Vui lòng kiểm tra lại mã sách và dữ liệu.", true);
                }
            });
        }
    }
}

void library_gui::draw_book_delete() {
    DrawText("Xóa sách", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();
    draw_input({360, 220, 420, 46}, "Mã sách cần xóa", field_a, sizeof(field_a));

    if (draw_button({460, 315, 200, 48}, "Xóa sách", RED)) {
        if (is_blank(field_a)) {
            set_message("Nhập mã sách cần xóa.", true);
            return;
        }
        node<book>* found = controller.get_book_by_id(field_a);
        if (found == nullptr) {
            set_message("Không tìm thấy mã sách.", true);
            return;
        }

        std::string id = field_a;
        if (controller.has_pending_request_for_book(id)) {
            set_message("Không thể xóa sách vì còn yêu cầu mượn đang chờ duyệt.", true);
            return;
        }
        if (controller.is_book_borrowed(id)) {
            set_message("Không thể xóa sách vì đang có người mượn hoặc chưa trả.", true);
            return;
        }
        std::string title = found->get_data().get_title();
        ask_confirm("Xác nhận xóa sách",
                    "Bạn có chắc chắn muốn xóa sách này?\n"
                    "Mã sách: " + id + "\n"
                    "Tên sách: " + title,
                    "Xóa sách", RED,
                    [this, id]() {
            if (controller.delete_book(id)) {
                set_message("Đã xóa sách.");
                clear_fields();
            } else {
                set_message("Không thể xóa sách. Vui lòng kiểm tra lại dữ liệu.", true);
            }
        });
    }
}
void library_gui::draw_borrows() {
    DrawText("Quản lý phiếu mượn", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    DrawText("Chọn nghiệp vụ:", 340, 195, 22, DARKGRAY);
    // Thu gọn nút để nhét thêm nút Hoàn Tác
    if (draw_button({340, 240, 175, 50}, "Lập phiếu")) {
        open_page(BORROW_CREATE);
        return;
    }
    if (draw_button({545, 240, 175, 50}, "Xác nhận trả", ORANGE)) {
        open_page(BORROW_RETURN);
        return;
    }
    if (draw_button({750, 240, 175, 50}, "Xóa phiếu", RED)) {
        open_page(BORROW_DELETE);
        return;
    }
    if (draw_button({955, 240, 150, 50}, "Hoàn tác", PURPLE)) {
        std::string msg;
        bool ok = controller.undo_borrow_action(msg);
        set_message(msg, !ok);
    }

    DrawText("Mỗi phiếu cần có mã phiếu, mã sách, mã độc giả,", 340, 345, 21, DARKGRAY);
    DrawText("ngày mượn và thời hạn 7, 14 hoặc 30 ngày.", 340, 380, 21, DARKGRAY);
}

void library_gui::draw_borrow_create() {
    DrawText("Lập phiếu mượn sách", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({340, 195, 360, 44}, "Mã phiếu", field_a, sizeof(field_a));
    draw_input({740, 195, 360, 44}, "Mã sách", field_b, sizeof(field_b));
    draw_input({340, 280, 360, 44}, "Mã độc giả", field_c, sizeof(field_c));
    draw_input({740, 280, 360, 44}, "Ngày mượn (DD/MM/YYYY)", field_d, sizeof(field_d));
    draw_input({340, 365, 360, 44}, "Thời hạn: 7, 14 hoặc 30 ngày",
               field_e, sizeof(field_e));

    if (!draw_button({530, 465, 210, 48}, "Tạo phiếu")) return;

    date borrow_date;
    int duration = 0;
    if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
        !read_date(field_d, borrow_date) ||
        !read_integer(field_e, duration, 1, 30) ||
        (duration != 7 && duration != 14 && duration != 30)) {
        set_message("Kiểm tra thông tin, ngày và thời hạn mượn.", true);
        return;
    }

    std::string borrow_id = field_a;
    std::string book_id = field_b;
    std::string user_id = field_c;

    if (controller.get_user_by_id(user_id) == nullptr) {
        set_message("Không tìm thấy mã độc giả.", true);
        return;
    }
    if (controller.has_overdue_borrows(user_id)) {
        set_message("Độc giả còn sách quá hạn chưa trả, không thể mượn.", true);
        return;
    }
    node<book>* book_node = controller.get_book_by_id(book_id);
    if (book_node == nullptr) {
        set_message("Không tìm thấy mã sách.", true);
        return;
    }

    std::string title = book_node->get_data().get_title();
    date due_date = borrow_date.add_days(duration);
    std::string due_text = format_date(due_date);

    ask_confirm("Xác nhận lập phiếu mượn",
                "Tạo phiếu mượn với thông tin sau?\n"
                "Mã phiếu: " + borrow_id + "\n"
                "Sách: " + book_id + " - " + title + "\n"
                "Độc giả: " + user_id + "\n"
                "Hạn trả: " + due_text,
                "Tạo phiếu", BLUE,
                [this, borrow_id, book_id, user_id, borrow_date, due_date, due_text]() {
        std::string msg;
        if (controller.borrow_book(borrow_id, book_id, user_id, borrow_date, due_date, msg)) {
            set_message("Đã tạo phiếu. Hạn trả: " + due_text);
            clear_fields();
        } else {
            set_message(msg, true); // Hiển thị nguyên nhân chi tiết
        }
    });
}

void library_gui::draw_borrow_return(bool deleting) {
    DrawText(deleting ? "Xóa phiếu mượn" : "Xác nhận trả sách",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({360, 220, 420, 46}, "Mã phiếu mượn", field_a, sizeof(field_a));
    const char* label = deleting ? "Xóa phiếu" : "Xác nhận trả";
    Color color = deleting ? RED : ORANGE;

    if (!draw_button({460, 315, 200, 48}, label, color)) return;

    if (is_blank(field_a)) {
        set_message("Nhập mã phiếu mượn.", true);
        return;
    }

    std::string id = field_a;
    if (deleting) {
        ask_confirm("Xác nhận xóa phiếu mượn",
                    "Bạn có chắc chắn muốn xóa phiếu mượn\n" + id + " không?",
                    "Xóa phiếu", RED,
                    [this, id]() {
            if (controller.delete_borrow(id)) {
                set_message("Đã xóa phiếu mượn.");
                clear_fields();
            } else {
                set_message("Không xóa được. Phiếu phải tồn tại và sách đã trả.", true);
            }
        });
    } else {
        ask_confirm("Xác nhận trả sách",
                    "Xác nhận độc giả đã trả sách\ncho phiếu mượn " + id + "?",
                    "Xác nhận trả", ORANGE,
                    [this, id]() {
            if (controller.return_book(id)) {
                set_message("Đã xác nhận trả sách.");
                clear_fields();
            } else {
                set_message("Không tìm thấy phiếu hoặc sách đã được trả.", true);
            }
        });
    }
}

void library_gui::draw_requests() {
    DrawText("Duyệt yêu cầu mượn sách", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();
    if (!controller.has_pending_requests()) {
        DrawText("Không có yêu cầu nào trong hàng đợi.", 340, 235, 22, DARKGRAY);
        return;
    }

    borrow request = controller.peek_next_request();
    DrawText(TextFormat("Số yêu cầu đang chờ: %d", controller.get_pending_request_count()),
             340, 190, 23, DARKBLUE);
    DrawText(TextFormat("Mã phiếu: %s", request.get_id().c_str()),
             360, 250, 21, DARKGRAY);
    DrawText(TextFormat("Mã sách: %s", request.get_book_id().c_str()),
             360, 290, 21, DARKGRAY);
    DrawText(TextFormat("Mã độc giả: %s", request.get_user_id().c_str()),
             360, 330, 21, DARKGRAY);
    DrawText(TextFormat("Ngày mượn: %d/%d/%d",
                        request.get_borrow_date().get_day(),
                        request.get_borrow_date().get_month(),
                        request.get_borrow_date().get_year()),
             360, 370, 21, DARKGRAY);
    DrawText(TextFormat("Hạn trả: %d/%d/%d",
                        request.get_due_date().get_day(),
                        request.get_due_date().get_month(),
                        request.get_due_date().get_year()),
             360, 410, 21, DARKGRAY);

    std::string summary = "Mã phiếu: " + request.get_id() + "\n"
                          "Mã sách: " + request.get_book_id() + "\n"
                          "Mã độc giả: " + request.get_user_id();

    // Nút duyệt: xác nhận trước khi xử lý yêu cầu.
    if (draw_button({360, 525, 195, 48}, "Duyệt", DARKGREEN)) {
        ask_confirm("Xác nhận duyệt yêu cầu",
                    "Duyệt yêu cầu mượn sách này?\n" + summary,
                    "Duyệt", DARKGREEN,
                    [this]() {
            if (controller.approve_next_request()) {
                set_message("Đã duyệt yêu cầu và tạo phiếu mượn.");
            } else {
                set_message(
                    "Duyệt thất bại. Yêu cầu vẫn ở đầu hàng đợi. "
                    "Kiểm tra sách, tài khoản và số lượng; "
                    "nếu yêu cầu không còn hợp lệ, hãy từ chối.",
                    true);
            }
        });
        return;
    }

    // Nhấn từ chối mới mở hộp thoại nhập lý do.
    if (draw_button({585, 525, 195, 48}, "Từ chối", RED)) {
        ask_confirm_input(
            "Xác nhận từ chối yêu cầu",
            "Từ chối yêu cầu mượn sách này?\n" + summary,
            "Từ chối",
            RED,
            [this]() {
                // Lấy lý do đã nhập trong hộp thoại.
                std::string reason = field_g;

                if (controller.reject_next_request(reason)) {
                    set_message("Đã từ chối yêu cầu và gửi thông báo.");
                } else {
                    set_message("Không thể từ chối yêu cầu.", true);
                }
            }
        );
        return;
    }
}

void library_gui::draw_borrow_filter() {
    DrawText("Lọc phiếu mượn theo ngày", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({330, 185, 310, 44}, "Từ ngày (DD/MM/YYYY)", field_a, sizeof(field_a));
    draw_input({670, 185, 310, 44}, "Đến ngày (DD/MM/YYYY)", field_b, sizeof(field_b));
    if (draw_button({1010, 185, 110, 44}, "Lọc")) list_page = 0;

    date start_date;
    date end_date;
    if (is_blank(field_a) || is_blank(field_b)) {
        DrawText("Nhập khoảng ngày để hiển thị danh sách phiếu mượn.",
                 330, 275, 20, DARKGRAY);
        return;
    }
    if (!read_date(field_a, start_date) || !read_date(field_b, end_date) ||
        end_date < start_date) {
        DrawText("Khoảng ngày không hợp lệ.", 330, 275, 20, RED);
        return;
    }

    linked_list<borrow> records =
        controller.filter_borrows_by_date(start_date, end_date);
    int total = records.get_size();
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (list_page >= page_count) list_page = page_count - 1;

    const char* headers[] = {"Mã phiếu", "Mã sách", "Mã độc giả", "Ngày mượn", "Hạn trả", "Trạng thái"};
    const int columns[] = {330, 460, 595, 740, 885, 1030};
    draw_table_header(headers, columns, 6, 250);

    node<borrow>* current = records.get_head();
    int skip = list_page * ROWS_PER_PAGE;
    while (current != nullptr && skip-- > 0) current = current->get_next();
    int row = 0;
    while (current != nullptr && row < ROWS_PER_PAGE) {
        borrow item = current->get_data();
        int y = 290 + row * 44;
        DrawRectangle(CONTENT_X + 15, y, CONTENT_W - 30, 42,
                      row % 2 == 0 ? Color{245, 248, 252, 255}
                                   : Color{232, 239, 248, 255});
        DrawText(item.get_id().c_str(), 330, y + 11, 17, DARKGRAY);
        DrawText(item.get_book_id().c_str(), 460, y + 11, 17, DARKGRAY);
        DrawText(item.get_user_id().c_str(), 595, y + 11, 17, DARKGRAY);
        DrawText(TextFormat("%d/%d/%d", item.get_borrow_date().get_day(),
                            item.get_borrow_date().get_month(),
                            item.get_borrow_date().get_year()),
                 740, y + 11, 17, DARKGRAY);
        DrawText(TextFormat("%d/%d/%d", item.get_due_date().get_day(),
                            item.get_due_date().get_month(),
                            item.get_due_date().get_year()),
                 885, y + 11, 17, DARKGRAY);
        DrawText(display_status(item.get_status()).c_str(), 1030, y + 11, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }
    if (total == 0) DrawText("Không có phiếu mượn trong khoảng ngày này.",
                             330, 315, 20, RED);

    if (draw_button({CONTENT_X + 230, 595, 110, 40}, "Trước", GRAY) &&
        list_page > 0) --list_page;
    if (draw_button({CONTENT_X + 550, 595, 110, 40}, "Sau", GRAY) &&
        list_page + 1 < page_count) ++list_page;
    draw_paging_hint(list_page, total, 605);
}

void library_gui::draw_shifts(const std::string& role) {
    DrawText(role == "manager" ? "Quản lý ca trực" : "Ca trực của tôi",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (draw_button({330, 175, 170, 44}, "Đăng ký ca")) {
        open_page(SHIFT_REGISTER);
        return;
    }
    if (draw_button({525, 175, 170, 44}, "Rút khỏi ca", ORANGE)) {
        open_page(SHIFT_CANCEL);
        return;
    }
    if (role == "manager" &&
        draw_button({720, 175, 170, 44}, "Xóa ca trực", RED)) {
        open_page(SHIFT_DELETE);
        return;
    }

    linked_list<shift> shifts = role == "manager"
        ? controller.get_all_shifts()
        : controller.get_my_shifts(controller.get_current_user()->get_id());

    int total = shifts.get_size();
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (list_page >= page_count) list_page = page_count - 1;

    const char* headers[] = {"Ngày", "Ca", "Số người", "Danh sách admin"};
    const int columns[] = {335, 535, 680, 840};
    draw_table_header(headers, columns, 4, 240);

    node<shift>* current = shifts.get_head();
    int skip = list_page * ROWS_PER_PAGE;
    while (current != nullptr && skip-- > 0) current = current->get_next();
    int row = 0;
    while (current != nullptr && row < ROWS_PER_PAGE) {
        shift item = current->get_data();
        date day = item.get_shift_date();
        int y = 280 + row * 44;
        DrawRectangle(CONTENT_X + 15, y, CONTENT_W - 30, 42,
                      row % 2 == 0 ? Color{245, 248, 252, 255}
                                   : Color{232, 239, 248, 255});
        DrawText(TextFormat("%d/%d/%d", day.get_day(), day.get_month(), day.get_year()),
                 335, y + 11, 17, DARKGRAY);
        DrawText(display_session(item.get_session()).c_str(), 535, y + 11, 17, DARKGRAY);
        DrawText(TextFormat("%d/4", item.get_current_count()),
                 680, y + 11, 17, DARKGRAY);
        DrawText(item.get_all_admins_string().c_str(), 840, y + 11, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }
    if (total == 0) {
        DrawText("Chưa có ca trực phù hợp.", 335, 310, 20, DARKGRAY);
    }

    if (draw_button({CONTENT_X + 230, 595, 110, 40}, "Trước", GRAY) &&
        list_page > 0) --list_page;
    if (draw_button({CONTENT_X + 550, 595, 110, 40}, "Sau", GRAY) &&
        list_page + 1 < page_count) ++list_page;
    draw_paging_hint(list_page, total, 605);
}

void library_gui::draw_shift_form(bool cancelling, bool deleting) {
    const char* title = deleting ? "Xóa ca trực" :
                        cancelling ? "Rút khỏi ca trực" : "Đăng ký ca trực";
    DrawText(title, CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({340, 200, 360, 44}, "Ngày (DD/MM/YYYY)", field_a, sizeof(field_a));
    draw_input({740, 200, 360, 44}, "Ca: sáng hoặc chiều", field_b, sizeof(field_b));

    bool manager = controller.get_current_role() == "manager";
    if (!deleting && manager) {
        draw_input({340, 290, 360, 44}, "Mã admin/manager", field_c, sizeof(field_c));
    } else if (!deleting) {
        DrawText(TextFormat("Nhân viên: %s",
                            controller.get_current_user()->get_id().c_str()),
                 340, 300, 21, DARKGRAY);
    }

    const char* action = deleting ? "Xóa ca" : cancelling ? "Rút khỏi ca" : "Đăng ký";
    Color action_color = deleting ? RED : cancelling ? ORANGE : BLUE;
    if (!draw_button({530, 400, 210, 48}, action, action_color)) return;

    date shift_date;
    std::string session = normalize_session(field_b);
    if (!read_date(field_a, shift_date) || session.empty()) {
        set_message("Ngày hoặc ca trực không hợp lệ.", true);
        return;
    }

    std::string date_text = format_date(shift_date);
    std::string session_text = display_session(session);

    if (deleting) {
        ask_confirm("Xác nhận xóa ca trực",
                    "Xóa toàn bộ ca trực này?\n"
                    "Ngày: " + date_text + "\n"
                    "Ca: " + session_text,
                    "Xóa ca", RED,
                    [this, shift_date, session]() {
            if (controller.delete_shift(shift_date, session)) {
                set_message("Đã xóa toàn bộ ca trực.");
                clear_fields();
            } else {
                set_message("Không tìm thấy ca trực.", true);
            }
        });
        return;
    }

    std::string person_id;
    if (manager) {
        person_id = field_c;
    } else {
        person_id = controller.get_current_user()->get_id();
    }
    if (is_blank(person_id.c_str())) {
        set_message("Nhập mã nhân viên.", true);
        return;
    }

    ask_confirm(cancelling ? "Xác nhận rút khỏi ca" : "Xác nhận đăng ký ca",
                std::string(cancelling ? "Rút nhân viên khỏi ca trực?\n"
                                       : "Đăng ký nhân viên vào ca trực?\n") +
                "Nhân viên: " + person_id + "\n"
                "Ngày: " + date_text + "\n"
                "Ca: " + session_text,
                cancelling ? "Rút khỏi ca" : "Đăng ký",
                cancelling ? ORANGE : BLUE,
                [this, cancelling, shift_date, session, person_id]() {
        bool success = cancelling
            ? controller.cancel_shift(shift_date, session, person_id)
            : controller.register_shift(shift_date, session, person_id);
        if (success) {
            set_message(cancelling ? "Đã rút khỏi ca trực." : "Đã đăng ký ca trực.");
            clear_fields();
        } else {
            set_message(cancelling
                        ? "Không tìm thấy nhân viên trong ca trực."
                        : "Không đăng ký được: ca đầy, trùng ca hoặc sai vai trò.",
                        true);
        }
    });
}

void library_gui::draw_salary(const std::string& role) {
    DrawText(role == "manager" ? "Tính lương ca trực" : "Lương ca trực của tôi",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (role == "manager") {
        draw_input({340, 205, 300, 44}, "Mã admin/manager", field_a, sizeof(field_a));
        draw_input({675, 205, 210, 44}, "Tháng, 0 = tất cả", field_b, sizeof(field_b));
        draw_input({920, 205, 210, 44}, "Năm, 0 = tất cả", field_c, sizeof(field_c));
    } else {
        draw_input({380, 205, 280, 44}, "Tháng, 0 = tất cả", field_a, sizeof(field_a));
        draw_input({710, 205, 280, 44}, "Năm, 0 = tất cả", field_b, sizeof(field_b));
    }

    if (draw_button({530, 320, 210, 48}, "Tính lương", DARKGREEN)) {
        std::string id = role == "manager" ? std::string(field_a)
                                           : controller.get_current_user()->get_id();
        const char* month_text = role == "manager" ? field_b : field_a;
        const char* year_text = role == "manager" ? field_c : field_b;
        int month = 0;
        int year = 0;

        bool month_ok = is_blank(month_text) || read_integer(month_text, month, 0, 12);
        bool year_ok = is_blank(year_text) || read_integer(year_text, year, 0, 2026);
        if (is_blank(id.c_str()) || !month_ok || !year_ok) {
            set_message("Nhập mã, tháng và năm hợp lệ.", true);
        } else {
            long long total = controller.get_admin_salary(id, month, year);
            set_message(TextFormat("Tổng lương: %lld VNĐ (%lld buổi)",
                                   total, total / 200000));
        }
    }
}

void library_gui::draw_users() {
    DrawText("Quản lý người dùng", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (draw_button({340, 185, 165, 48}, "Thêm tài khoản")) {
        open_page(USER_ADD);
        return;
    }
    if (draw_button({520, 185, 165, 48}, "Sửa tài khoản", ORANGE)) {
        open_page(USER_EDIT);
        return;
    }
    if (draw_button({700, 185, 165, 48}, "Xóa tài khoản", RED)) {
        open_page(USER_DELETE);
        return;
    }
    if (draw_button({880, 185, 130, 48}, "Hoàn tác", PURPLE)) {
        std::string msg;
        bool ok = controller.undo_user_action(msg);
        set_message(msg, !ok);
        searched_user_id.clear(); // Xóa thông tin tra cứu hiện tại để tránh hiển thị user vừa bị xóa/đổi
    }

    draw_input({340, 295, 380, 44}, "Mã tài khoản cần tra cứu", field_a, sizeof(field_a));
    if (draw_button({750, 295, 150, 44}, "Tra cứu")) {
        if (is_blank(field_a)) {
            set_message("Nhập mã tài khoản.", true);
            searched_user_id.clear();
        } else {
            node<user>* found = controller.get_user_by_id(field_a);
            if (found == nullptr) {
                set_message("Không tìm thấy tài khoản.", true);
                searched_user_id.clear();
            } else {
                searched_user_id = field_a;
                set_message(""); // Xóa câu thông báo chung
                list_page = 0;   // Reset trang danh sách
            }
        }
    }

    // Nếu tìm thấy tài khoản, vẽ thông tin và bảng danh sách tùy theo vai trò
    if (!searched_user_id.empty()) {
        node<user>* found = controller.get_user_by_id(searched_user_id);
        if (found != nullptr) {
            user item = found->get_data();
            
            // ================== CẢNH BÁO TÀI KHOẢN QUÁ HẠN ==================
            // Kiểm tra xem độc giả có đang giữ sách quá hạn không
            bool is_overdue = (item.get_role() == "user") && controller.has_overdue_borrows(searched_user_id);
            Color user_info_color = is_overdue ? RED : DARKGREEN;
            std::string warning_tag = is_overdue ? " [CẢNH BÁO: CÓ SÁCH QUÁ HẠN]" : "";
            
            DrawText(TextFormat("Kết quả: %s - %s (%s)%s", 
                                item.get_id().c_str(), 
                                item.get_name().c_str(), 
                                display_role(item.get_role()).c_str(),
                                warning_tag.c_str()), 
                     340, 370, 20, user_info_color);
            
            // ================== HIỂN THỊ CA TRỰC CHO ADMIN / MANAGER ==================
            if (item.get_role() == "admin" || item.get_role() == "manager") {
                linked_list<shift> shifts = controller.get_my_shifts(searched_user_id);
                int total = shifts.get_size();
                
                if (total == 0) {
                    DrawText("Nhân viên này chưa đăng ký ca trực nào.", 340, 410, 18, DARKGRAY);
                } else {
                    DrawText("Lịch trực của nhân viên:", 340, 410, 18, DARKGRAY);
                    
                    const char* headers[] = {"Ngày trực", "Ca trực", "Số người", "Danh sách admin"};
                    const int columns[] = {340, 520, 680, 840};
                    draw_table_header(headers, columns, 4, 440);
                    
                    int rows_per_user_page = 3;
                    int page_count = total == 0 ? 1 : (total + rows_per_user_page - 1) / rows_per_user_page;
                    if (list_page >= page_count) list_page = page_count - 1;
                    
                    node<shift>* current = shifts.get_head();
                    int skip = list_page * rows_per_user_page;
                    while (current != nullptr && skip-- > 0) current = current->get_next();
                    
                    int row = 0;
                    while (current != nullptr && row < rows_per_user_page) {
                        shift s_item = current->get_data();
                        int y = 480 + row * 40;
                        DrawRectangle(CONTENT_X + 15, (float)y, CONTENT_W - 30, 38, row % 2 == 0 ? Color{245, 248, 252, 255} : Color{232, 239, 248, 255});
                        
                        date day = s_item.get_shift_date();
                        DrawText(TextFormat("%d/%d/%d", day.get_day(), day.get_month(), day.get_year()), columns[0], y + 10, 16, DARKGRAY);
                        DrawText(display_session(s_item.get_session()).c_str(), columns[1], y + 10, 16, DARKGRAY);
                        DrawText(TextFormat("%d/4", s_item.get_current_count()), columns[2], y + 10, 16, DARKGRAY);
                        
                        // Cắt bớt danh sách admin nếu quá dài để tránh tràn cột
                        std::string admins = s_item.get_all_admins_string();
                        if (admins.length() > 30) admins = admins.substr(0, 27) + "...";
                        DrawText(admins.c_str(), columns[3], y + 10, 16, DARKGRAY);
                        
                        current = current->get_next();
                        ++row;
                    }
                    
                    // Phân trang
                    if (draw_button({CONTENT_X + 230, 615, 110, 40}, "Trước", GRAY) && list_page > 0) --list_page;
                    if (draw_button({CONTENT_X + 550, 615, 110, 40}, "Sau", GRAY) && list_page + 1 < page_count) ++list_page;
                    draw_paging_hint(list_page, total, 625, rows_per_user_page);
                }
            } 
            // ================== HIỂN THỊ SÁCH ĐANG MƯỢN CHO USER ==================
            else {
                // Lấy danh sách phiếu mượn đang active (Đang mượn hoặc Quá hạn)
                linked_list<borrow> records = controller.get_my_borrows(searched_user_id, true);
                int total = records.get_size();
                
                if (total == 0) {
                    DrawText("Độc giả này hiện không mượn sách nào.", 340, 410, 18, DARKGRAY);
                } else {
                    DrawText("Sách đang mượn / Quá hạn:", 340, 410, 18, DARKGRAY);
                    
                    const char* headers[] = {"Mã phiếu", "Mã sách", "Ngày mượn", "Hạn trả", "Trạng thái"};
                    const int columns[] = {340, 470, 650, 810, 950};
                    draw_table_header(headers, columns, 5, 440);
                    
                    int rows_per_user_page = 3;
                    int page_count = total == 0 ? 1 : (total + rows_per_user_page - 1) / rows_per_user_page;
                    if (list_page >= page_count) list_page = page_count - 1;
                    
                    node<borrow>* current = records.get_head();
                    int skip = list_page * rows_per_user_page;
                    while (current != nullptr && skip-- > 0) current = current->get_next();
                    
                    int row = 0;
                    while (current != nullptr && row < rows_per_user_page) {
                        borrow b_item = current->get_data();
                        int y = 480 + row * 40;
                        DrawRectangle(CONTENT_X + 15, (float)y, CONTENT_W - 30, 38, row % 2 == 0 ? Color{245, 248, 252, 255} : Color{232, 239, 248, 255});
                        
                        DrawText(b_item.get_id().c_str(), columns[0], y + 10, 16, DARKGRAY);
                        DrawText(b_item.get_book_id().c_str(), columns[1], y + 10, 16, DARKGRAY);
                        DrawText(format_date(b_item.get_borrow_date()).c_str(), columns[2], y + 10, 16, DARKGRAY);
                        DrawText(format_date(b_item.get_due_date()).c_str(), columns[3], y + 10, 16, DARKGRAY);
                        
                        // In màu đỏ nếu sách quá hạn
                        Color status_color = b_item.get_status() == "Qua han" ? RED : DARKGRAY;
                        DrawText(display_status(b_item.get_status()).c_str(), columns[4], y + 10, 16, status_color);
                        
                        current = current->get_next();
                        ++row;
                    }
                    
                    // Phân trang
                    if (draw_button({CONTENT_X + 230, 615, 110, 40}, "Trước", GRAY) && list_page > 0) --list_page;
                    if (draw_button({CONTENT_X + 550, 615, 110, 40}, "Sau", GRAY) && list_page + 1 < page_count) ++list_page;
                    draw_paging_hint(list_page, total, 625, rows_per_user_page);
                }
            }
        }
    }
}

void library_gui::draw_user_form(bool editing) {
    DrawText(editing ? "Cập nhật tài khoản" : "Thêm tài khoản mới",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (editing) {
        draw_input({340, 185, 360, 44}, "Mã tài khoản cũ", field_a, sizeof(field_a));
        draw_input({740, 185, 360, 44}, "Mã tài khoản mới", field_b, sizeof(field_b));
        draw_input({340, 270, 360, 44}, "Họ và tên", field_c, sizeof(field_c));
        draw_input({740, 270, 360, 44}, "Mật khẩu", field_d, sizeof(field_d));
        draw_input({340, 355, 360, 44}, "Vai trò: manager, admin, user",
                   field_e, sizeof(field_e));
    } else {
        draw_input({340, 195, 360, 44}, "Mã tài khoản", field_a, sizeof(field_a));
        draw_input({740, 195, 360, 44}, "Họ và tên", field_b, sizeof(field_b));
        draw_input({340, 280, 360, 44}, "Mật khẩu", field_c, sizeof(field_c));
        draw_input({740, 280, 360, 44}, "Vai trò: manager, admin, user",
                   field_d, sizeof(field_d));
    }

    if (!draw_button({530, 465, 220, 48},
                     editing ? "Cập nhật" : "Thêm tài khoản")) {
        return;
    }

    std::string old_id = field_a;
    std::string id = editing ? field_b : field_a;
    std::string name = editing ? field_c : field_b;
    std::string password = editing ? field_d : field_c;
    std::string role = editing ? field_e : field_d;
    bool supported_role = role == "manager" || role == "admin" || role == "user";

    if (is_blank(old_id.c_str()) || is_blank(id.c_str()) || is_blank(name.c_str()) ||
        is_blank(password.c_str()) || !supported_role) {
        set_message("Nhập đầy đủ thông tin và vai trò hợp lệ.", true);
        return;
    }

    std::string summary = "Mã tài khoản: " + id + "\n"
                          "Họ và tên: " + name + "\n"
                          "Vai trò: " + display_role(role);

    if (editing) {
        if (old_id != id) {
            if (controller.has_pending_request_for_user(old_id)) {
                set_message("Không thể đổi mã tài khoản khi còn yêu cầu mượn chờ duyệt.", true);
                return;
            }
            if (controller.get_user_by_id(id) != nullptr) {
                set_message("Mã tài khoản mới đã tồn tại.", true);
                return;
            }
        }
        ask_confirm("Xác nhận cập nhật tài khoản",
                    "Cập nhật tài khoản " + old_id + "?\n" + summary,
                    "Cập nhật", ORANGE,
                    [this, old_id, id, name, password, role]() {
            if (controller.update_user_info(old_id, user(id, name, password, role))) {
                set_message("Đã cập nhật tài khoản.");
                clear_fields();
            } else {
                set_message("Không thể cập nhật tài khoản. Vui lòng kiểm tra lại mã và dữ liệu.", true);
            }
        });
    } else {
        ask_confirm("Xác nhận thêm tài khoản",
                    "Thêm tài khoản mới?\n" + summary,
                    "Thêm tài khoản", BLUE,
                    [this, id, name, password, role]() {
            if (controller.add_new_user(id, name, password, role)) {
                set_message("Đã thêm tài khoản.");
                clear_fields();
            } else {
                set_message("Mã tài khoản đã tồn tại.", true);
            }
        });
    }
}

void library_gui::draw_user_delete() {
    DrawText("Xóa tài khoản", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();
    draw_input({360, 220, 420, 46}, "Mã tài khoản cần xóa",
               field_a, sizeof(field_a));

    if (!draw_button({460, 315, 210, 48}, "Xóa tài khoản", RED)) return;

    if (is_blank(field_a)) {
        set_message("Nhập mã tài khoản cần xóa.", true);
        return;
    }
    node<user>* found = controller.get_user_by_id(field_a);
    if (found == nullptr) {
        set_message("Không tìm thấy tài khoản.", true);
        return;
    }
    user* current_user = controller.get_current_user();
    if (current_user != nullptr && current_user->get_id() == field_a) {
        set_message("Không thể xóa tài khoản đang đăng nhập.", true);
        return;
    }
    if (controller.has_pending_request_for_user(field_a)) {
        set_message("Không thể xóa tài khoản vì còn yêu cầu mượn đang chờ duyệt.", true);
        return;
    }
    if (controller.has_overdue_borrows(field_a)) {
        set_message("Không thể xóa tài khoản vì còn sách quá hạn chưa trả.", true);
        return;
    }
    if (controller.has_active_borrows(field_a)) {
        set_message("Không thể xóa tài khoản vì còn sách đang mượn chưa trả.", true);
        return;
    }
    if (controller.has_user_shifts(field_a)) {
        set_message("Không thể xóa tài khoản vì tài khoản vẫn có ca trực.", true);
        return;
    }

    std::string id = field_a;
    std::string name = found->get_data().get_name();
    ask_confirm("Xác nhận xóa tài khoản",
                "Bạn có chắc chắn muốn xóa tài khoản này?\n"
                "Mã tài khoản: " + id + "\n"
                "Họ và tên: " + name,
                "Xóa tài khoản", RED,
                [this, id]() {
        if (controller.delete_user(id)) {
            set_message("Đã xóa tài khoản.");
            clear_fields();
        } else {
            set_message("Không thể xóa tài khoản. Vui lòng kiểm tra lại dữ liệu.", true);
        }
    });
}

void library_gui::draw_shelf() {
    DrawText("Tra cứu vị trí kệ sách", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({360, 220, 420, 46}, "Mã sách", field_a, sizeof(field_a));
    if (draw_button({480, 315, 180, 48}, "Tra cứu")) {
        if (is_blank(field_a)) {
            set_message("Nhập mã sách.", true);
        } else {
            node<book>* found = controller.get_book_by_id(field_a);
            if (found == nullptr) {
                set_message("Không tìm thấy sách.", true);
            } else {
                book item = found->get_data();
                set_message("Sách: " + item.get_title() +
                            " | Thể loại: " + item.get_category() +
                            " | Kệ: " + display_shelf(controller.get_book_shelf(item.get_id())));
            }
        }
    }
}

void library_gui::draw_user_request() {
    user* current_user = controller.get_current_user();
    if (current_user == nullptr) {
        DrawText("Vui lòng đăng nhập lại.", CONTENT_X + 25, 150, 20, RED);
        return;
    }

    linked_list<borrow> pending_requests =
        controller.get_my_pending_requests(current_user->get_id());
    int pending_count = pending_requests.get_size();

    if (showing_pending_requests) {
        DrawText("Danh sách yêu cầu đang chờ duyệt", CONTENT_X + 25, 120,
                 27, DARKBLUE);
        if (draw_button({CONTENT_X + CONTENT_W - 235, 112, 210, 40},
                        "Quay lại đăng ký", GRAY)) {
            showing_pending_requests = false;
            list_page = 0;
            return;
        }

        const char* headers[] = {"Mã yêu cầu", "Tên sách", "Ngày mượn", "Hạn trả"};
        const int columns[] = {330, 485, 850, 1010};
        draw_table_header(headers, columns, 4, 185);

        int total = pending_count;
        int page_count = total == 0 ? 1 :
            (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
        if (list_page >= page_count) list_page = page_count - 1;

        node<borrow>* current = pending_requests.get_head();
        int skip = list_page * ROWS_PER_PAGE;
        while (current != nullptr && skip-- > 0) {
            current = current->get_next();
        }

        int row = 0;
        while (current != nullptr && row < ROWS_PER_PAGE) {
            borrow item = current->get_data();
            node<book>* found = controller.get_book_by_id(item.get_book_id());
            std::string title = found == nullptr ? item.get_book_id()
                                                  : found->get_data().get_title();
            int y = 225 + row * 46;
            DrawRectangle(CONTENT_X + 15, y, CONTENT_W - 30, 44,
                          row % 2 == 0 ? Color{245, 248, 252, 255}
                                       : Color{232, 239, 248, 255});
            DrawText(item.get_id().c_str(), columns[0], y + 12, 17, DARKGRAY);
            DrawText(title.c_str(), columns[1], y + 12, 17, DARKGRAY);
            DrawText(format_date(item.get_borrow_date()).c_str(),
                     columns[2], y + 12, 17, DARKGRAY);
            DrawText(format_date(item.get_due_date()).c_str(),
                     columns[3], y + 12, 17, DARKGRAY);
            current = current->get_next();
            ++row;
        }

        if (total == 0) {
            DrawText("Bạn hiện không có yêu cầu mượn nào đang chờ duyệt.",
                     330, 260, 19, DARKGRAY);
        }
        if (draw_button({CONTENT_X + 230, 595, 110, 40}, "Trước", GRAY) &&
            list_page > 0) --list_page;
        if (draw_button({CONTENT_X + 550, 595, 110, 40}, "Sau", GRAY) &&
            list_page + 1 < page_count) ++list_page;
        draw_paging_hint(list_page, total, 605);
        return;
    }

    DrawText("Đăng ký mượn sách", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();
    std::string request_button_label =
        "Danh sách yêu cầu (" + std::to_string(pending_count) + ")";
    if (draw_button({CONTENT_X + 485, 115, 240, 40},
                    request_button_label.c_str(), BLUE)) {
        showing_pending_requests = true;
        list_page = 0;
        return;
    }

    draw_input({340, 195, 360, 44}, "Mã sách", field_b, sizeof(field_b));
    draw_input({740, 195, 360, 44}, "Ngày mượn (DD/MM/YYYY)", field_c, sizeof(field_c));
    draw_input({340, 280, 360, 44}, "Thời hạn: 7, 14 hoặc 30 ngày",
               field_d, sizeof(field_d));
    if (!draw_button({530, 390, 210, 48}, "Gửi yêu cầu")) return;

    date borrow_date;
    int duration = 0;
    if (is_blank(field_b) || !read_date(field_c, borrow_date) || !read_integer(field_d, duration, 1, 30) ||
        (duration != 7 && duration != 14 && duration != 30))
    {
        set_message("Kiểm tra thông tin, ngày và thời hạn mượn.", true);
        return;
    }

    std::string user_id = current_user->get_id();
    if (controller.has_overdue_borrows(user_id)) {
        set_message("Bạn còn sách quá hạn chưa trả, "
                    "không thể đăng ký mượn sách.", true);
        return;
    }
    std::string book_id = field_b;
    node<book>* book_node = controller.get_book_by_id(book_id);
    if (book_node == nullptr) {
        set_message("Không tìm thấy mã sách.", true);
        return;
    }

    std::string title = book_node->get_data().get_title();
    date due_date = borrow_date.add_days(duration);

    ask_confirm("Xác nhận đăng ký mượn sách",
                "Gửi yêu cầu mượn sách này?\n"
                "Sách: " + book_id + " - " + title + "\n"
                "Ngày mượn: " + format_date(borrow_date) + "\n"
                "Hạn trả: " + format_date(due_date),
                "Gửi yêu cầu", BLUE,
                [this, book_id, user_id, borrow_date, due_date]() {
        std::string out_msg; // Tạo biến chứa kết quả lỗi
        if (controller.register_book_request(book_id, user_id, borrow_date, due_date, out_msg)) {
            set_message(out_msg);
            clear_fields();
        } else {
            set_message(out_msg, true); // Báo đúng lý do lỗi (Trùng sách, quá hạn, v.v.)
        }
    });
}

void library_gui::draw_my_borrows(bool active_only) {
    DrawText(active_only ? "Sách đang mượn" : "Lịch sử mượn sách",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    linked_list<borrow> records = controller.get_my_borrows(
        controller.get_current_user()->get_id(), active_only);
    int total = records.get_size();
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (list_page >= page_count) list_page = page_count - 1;

    const char* headers[] = {"Mã phiếu", "Tên sách", "Ngày mượn", "Hạn trả", "Trạng thái"};
    const int columns[] = {330, 465, 765, 910, 1045};
    draw_table_header(headers, columns, 5, 185);

    node<borrow>* current = records.get_head();
    int skip = list_page * ROWS_PER_PAGE;
    while (current != nullptr && skip-- > 0) current = current->get_next();
    int row = 0;
    while (current != nullptr && row < ROWS_PER_PAGE) {
        borrow item = current->get_data();
        node<book>* book_node = controller.get_book_by_id(item.get_book_id());
        std::string title = book_node == nullptr ? item.get_book_id()
                                                 : book_node->get_data().get_title();
        int y = 225 + row * 46;
        DrawRectangle(CONTENT_X + 15, y, CONTENT_W - 30, 44,
                      row % 2 == 0 ? Color{245, 248, 252, 255}
                                   : Color{232, 239, 248, 255});
        DrawText(item.get_id().c_str(), 330, y + 12, 17, DARKGRAY);
        DrawText(title.c_str(), 465, y + 12, 17, DARKGRAY);
        DrawText(TextFormat("%d/%d/%d", item.get_borrow_date().get_day(),
                            item.get_borrow_date().get_month(),
                            item.get_borrow_date().get_year()),
                 765, y + 12, 17, DARKGRAY);
        DrawText(TextFormat("%d/%d/%d", item.get_due_date().get_day(),
                            item.get_due_date().get_month(),
                            item.get_due_date().get_year()),
                 910, y + 12, 17, DARKGRAY);
        DrawText(display_status(item.get_status()).c_str(), 1045, y + 12, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }

    if (total == 0) {
        DrawText(active_only ? "Bạn không có sách đang mượn."
                             : "Bạn chưa có lịch sử mượn sách.",
                 330, 260, 20, DARKGRAY);
    }
    if (draw_button({CONTENT_X + 230, 595, 110, 40}, "Trước", GRAY) &&
        list_page > 0) --list_page;
    if (draw_button({CONTENT_X + 550, 595, 110, 40}, "Sau", GRAY) &&
        list_page + 1 < page_count) ++list_page;
    draw_paging_hint(list_page, total, 605);
}

void library_gui::draw_notifications() {
    DrawText("Thông báo", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    user* current_user = controller.get_current_user();
    if (current_user == nullptr) return;

    std::vector<notification> records =
        controller.get_my_notifications(current_user->get_id());

    int total = (int)records.size();
    int unread =
        controller.get_unread_notification_count(current_user->get_id());

    DrawText(
        TextFormat("Tổng: %d thông báo | Chưa đọc: %d", total, unread),
        330, 175, 19, DARKGRAY
    );

    int page_count = total == 0 ? 1 : (total + 4) / 5;

    if (list_page >= page_count) {
        list_page = page_count - 1;
    }

    int start = list_page * 5;
    int end = std::min(start + 5, total);

    for (int i = start; i < end; ++i) {
        // Hiển thị thông báo mới nhất trước.
        notification item = records[total - 1 - i];

        int y = 215 + (i - start) * 76;

        Rectangle card = {325, (float)y, 850, 66};

        DrawRectangleRounded(
            card, 0.08f, 6,
            item.get_is_read()
                ? Color{245, 247, 251, 255}
                : Color{225, 238, 255, 255}
        );

        DrawRectangleRoundedLines(card, 0.08f, 6, LIGHTGRAY);

        DrawCircle(
            345, y + 20, 5,
            item.get_is_read() ? GRAY : BLUE
        );

        DrawText(
            item.get_title().c_str(),
            360, y + 8, 18, DARKBLUE
        );

        // Rút gọn nội dung trên danh sách, chỉ xem đầy đủ khi mở chi tiết.
        std::string content = item.get_content();

        if (content.size() > 95) {
            content = content.substr(0, 92) + "...";
        }

        DrawText(
            content.c_str(),
            360, y + 34, 15, DARKGRAY
        );

        // Tăng kích thước và căn giữa thời gian theo chiều dọc của thẻ.
        DrawText(
            item.get_created_at().c_str(),
            1000, y + 23, 18, GRAY
        );

        Vector2 mouse = ui_layout::mouse_position();

        // Nhấn vào thông báo để mở nội dung chi tiết.
        if (!notification_detail_open &&
            !input_locked &&
            CheckCollisionPointRec(mouse, card) &&
            IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {

            notification_detail_title = item.get_title();
            notification_detail_content = item.get_content();
            notification_detail_time = item.get_created_at();

            notification_detail_scroll = 0;
            notification_detail_open = true;

            // Đánh dấu đã đọc khi người dùng mở thông báo.
            if (!item.get_is_read()) {
                controller.mark_notification_read(item.get_id());
                set_message("Đã đánh dấu thông báo là đã đọc.");
            }
        }
    }

    if (total == 0) {
        DrawText(
            "Bạn chưa có thông báo nào.",
            350, 260, 20, DARKGRAY
        );
    }

    // Phân trang danh sách thông báo.
    if (draw_button({CONTENT_X + 230, 615, 110, 40}, "Trước", GRAY)
        && list_page > 0 && !notification_detail_open) {
        --list_page;
    }

    if (draw_button({CONTENT_X + 550, 615, 110, 40}, "Sau", GRAY)
        && list_page + 1 < page_count && !notification_detail_open) {
        ++list_page;
    }

    draw_paging_hint(list_page, total, 625, 5);

    // Hiển thị hộp thoại chi tiết thông báo.
    if (notification_detail_open)
    {
        float screen_w = (float)WINDOW_W;
        float screen_h = (float)WINDOW_H;

        // Giới hạn kích thước hộp thoại, không để tràn khỏi màn hình.
        float box_w = std::min(820.0f, screen_w * 0.72f);
        float box_h = std::min(560.0f, screen_h * 0.75f);

        // Căn giữa hộp thoại theo kích thước cửa sổ.
        Rectangle box = {
            (screen_w - box_w) / 2,
            (screen_h - box_h) / 2,
            box_w,
            box_h
        };

        DrawRectangle(0, 0, (int)screen_w, (int)screen_h,
                      {0, 0, 0, 130});

        DrawRectangleRounded(box, 0.05f, 10, WHITE);
        DrawRectangleRoundedLines(box, 0.05f, 10, DARKBLUE);

        // Tiêu đề thông báo.
        DrawText(
            notification_detail_title.c_str(),
            (int)box.x + 30, (int)box.y + 25,
            27, DARKBLUE);

        DrawRectangle(
            (int)box.x + 30, (int)box.y + 75,
            (int)box.width - 60, 2, LIGHTGRAY);

        // Thời gian thông báo.
        DrawText(
            notification_detail_time.c_str(),
            (int)box.x + 30, (int)box.y + 95,
            18, GRAY);

        DrawText(
            "Nội dung:",
            (int)box.x + 30, (int)box.y + 135,
            22, DARKBLUE);

        // Tách phần lý do thành một dòng riêng khi hiển thị.
        std::string display_content = notification_detail_content;

        size_t reason_pos = display_content.find("Lý do:");

        if (reason_pos != std::string::npos && reason_pos > 0)
        {
            if (display_content[reason_pos - 1] == ' ')
            {
                display_content.replace(reason_pos - 1, 1, "\n");
            }
        }

        // Khu vực hiển thị nội dung có thể cuộn.
        Rectangle view = {
            box.x + 30,
            box.y + 170,
            box.width - 60,
            box.height - 270};

        // Chia nội dung thành các dòng dựa theo chiều rộng hiển thị.
        std::vector<std::string> lines;
        std::string paragraph;

        for (size_t i = 0; i <= display_content.size(); ++i)
        {
            if (i == display_content.size() ||
                display_content[i] == '\n')
            {

                std::string current_line;
                size_t pos = 0;

                while (pos < paragraph.size())
                {
                    while (pos < paragraph.size() &&
                           paragraph[pos] == ' ')
                    {
                        ++pos;
                    }

                    if (pos >= paragraph.size())
                        break;

                    size_t end_word = paragraph.find(' ', pos);

                    std::string word = paragraph.substr(
                        pos,
                        end_word == std::string::npos
                            ? std::string::npos
                            : end_word - pos);

                    std::string candidate = current_line.empty()
                                                ? word
                                                : current_line + " " + word;

                    if (MeasureText(candidate.c_str(), 21) >
                            (int)view.width - 20 &&
                        !current_line.empty())
                    {

                        lines.push_back(current_line);
                        current_line = word;
                    }
                    else
                    {
                        current_line = candidate;
                    }

                    if (end_word == std::string::npos)
                        break;

                    pos = end_word + 1;
                }

                lines.push_back(current_line);
                paragraph.clear();
            }
            else
            {
                paragraph += display_content[i];
            }
        }

        // Tính giới hạn cuộn theo tổng chiều cao nội dung.
        float line_height = 32;
        float content_height = (float)lines.size() * line_height;

        float max_scroll = std::max(
            0.0f,
            content_height - view.height);

        Vector2 mouse = ui_layout::mouse_position();

        // Chỉ cuộn khi chuột nằm trong khu vực nội dung.
        if (CheckCollisionPointRec(mouse, view))
        {
            notification_detail_scroll -= GetMouseWheelMove() * 40;
        }

        notification_detail_scroll = std::max(
            0.0f,
            std::min(notification_detail_scroll, max_scroll));

        // Giới hạn vùng vẽ để chữ không tràn khỏi hộp thoại.
        BeginScissorMode(
            (int)view.x, (int)view.y,
            (int)view.width, (int)view.height);

        for (int i = 0; i < (int)lines.size(); ++i)
        {
            DrawText(
                lines[i].c_str(),
                (int)view.x,
                (int)(view.y + 5 + i * line_height - notification_detail_scroll),
                21, DARKGRAY);
        }

        EndScissorMode();

        DrawText(
            "Cuộn chuột để xem nội dung còn lại",
            (int)view.x,
            (int)(box.y + box.height - 75),
            15, GRAY);

        // Nút đóng luôn nằm ở giữa phía dưới hộp thoại.
        if (draw_button(
                {box.x + (box.width - 200) / 2,
                 box.y + box.height - 55,
                 200, 42},
                "Đóng", GRAY))
        {

            notification_detail_open = false;
            notification_detail_scroll = 0;
        }
    }
}

void library_gui::draw_book_detail_popup() {
    if (!show_book_detail_popup) return;

    input_locked = false;

    node<book>* b_node = controller.get_book_by_id(detail_book_id);
    if (b_node == nullptr) {
        show_book_detail_popup = false;
        return;
    }
    book b = b_node->get_data();

    // Phủ lớp nền đen mờ
    DrawRectangle(0, 0, WINDOW_W, WINDOW_H, Color{0, 0, 0, 140});

    // Khung Popup chính
    Rectangle box = { (WINDOW_W - 740) / 2.0f, (WINDOW_H - 520) / 2.0f, 740, 520 };
    DrawRectangleRounded(box, 0.04f, 8, WHITE);
    DrawRectangleRoundedLines(box, 0.04f, 8, DARKBLUE);

    DrawText("THÔNG TIN CHI TIẾT SÁCH", (int)box.x + 28, (int)box.y + 20, 24, DARKBLUE);

    int y_info = (int)box.y + 58;

    // Dòng 1: Mã sách - Thể loại - Năm xuất bản - Kệ sách
    DrawText(TextFormat("Mã sách: %s", b.get_id().c_str()), (int)box.x + 28, y_info, 17, DARKGRAY);
    DrawText(TextFormat("Thể loại: %s", b.get_category().c_str()), (int)box.x + 200, y_info, 17, DARKGRAY);
    DrawText(TextFormat("Năm xuất bản: %d", b.get_year()), (int)box.x + 370, y_info, 17, DARKGRAY);
    DrawText(TextFormat("Kệ sách: %s", display_shelf(controller.get_book_shelf(b.get_id())).c_str()), (int)box.x + 570, y_info, 17, DARKBLUE);

    // Dòng 2: Tên sách (Dành riêng trọn 1 dòng rộng 680px, hiển thị tối đa 55 ký tự thoải mái tên dài)
    y_info += 26;
    std::string popup_title = truncate_utf8(b.get_title(), 55);
    DrawText(TextFormat("Tên sách: %s", popup_title.c_str()), (int)box.x + 28, y_info, 18, DARKBLUE);

    // Dòng 3: Tác giả - Số lượng còn - Đang mượn
    y_info += 26;
    std::string popup_author = truncate_utf8(b.get_author(), 30);
    DrawText(TextFormat("Tác giả: %s", popup_author.c_str()), (int)box.x + 28, y_info, 17, DARKGRAY);
    DrawText(TextFormat("Số lượng còn: %d", b.get_quantity()), (int)box.x + 370, y_info, 17, DARKGREEN);
    DrawText(TextFormat("Đang mượn: %d", cached_currently_borrowed_count), (int)box.x + 570, y_info, 17, RED);

    // Vạch phân cách
    int y_sep = y_info + 32;
    DrawRectangle((int)box.x + 28, y_sep, (int)box.width - 56, 2, LIGHTGRAY);

    DrawText("DANH SÁCH ĐỘC GIẢ ĐANG MƯỢN SÁCH NÀY:", (int)box.x + 28, y_sep + 10, 18, DARKBLUE);

    int table_y = y_sep + 38;
    int table_x = (int)box.x + 28;
    int table_w = (int)box.width - 56;

    // Header bảng
    DrawRectangle(table_x, table_y, table_w, 34, DARKBLUE);
    DrawText("Mã phiếu", table_x + 12, table_y + 8, 16, WHITE);
    DrawText("Độc giả", table_x + 120, table_y + 8, 16, WHITE);
    DrawText("Ngày mượn", table_x + 250, table_y + 8, 16, WHITE);
    DrawText("Hạn trả", table_x + 390, table_y + 8, 16, WHITE);
    DrawText("Trạng thái", table_x + 530, table_y + 8, 16, WHITE);

    int items_per_page = 4;
    int total_pages = cached_currently_borrowed_count == 0 ? 1 : (cached_currently_borrowed_count + items_per_page - 1) / items_per_page;
    if (detail_borrow_page < 1) detail_borrow_page = 1;
    if (detail_borrow_page > total_pages) detail_borrow_page = total_pages;

    if (cached_currently_borrowed_count == 0) {
        DrawText("Hiện chưa có độc giả nào đang mượn cuốn sách này.", table_x + 15, table_y + 45, 17, DARKGRAY);
    } else {
        node<borrow>* curr = cached_detail_borrows.get_head();
        int skip = (detail_borrow_page - 1) * items_per_page;
        int matches_seen = 0;
        int row = 0;

        while (curr != nullptr && row < items_per_page) {
            borrow br = curr->get_data();
            std::string st = br.get_status();
            if (st == "Dang muon" || st == "Qua han") {
                if (matches_seen < skip) {
                    matches_seen++;
                } else {
                    int row_y = table_y + 34 + row * 35;
                    DrawRectangle(table_x, row_y, table_w, 33, row % 2 == 0 ? Color{245, 248, 252, 255} : Color{232, 239, 248, 255});

                    DrawText(br.get_id().c_str(), table_x + 12, row_y + 7, 16, DARKGRAY);
                    DrawText(br.get_user_id().c_str(), table_x + 120, row_y + 7, 16, DARKGRAY);
                    DrawText(format_date(br.get_borrow_date()).c_str(), table_x + 250, row_y + 7, 16, DARKGRAY);
                    DrawText(format_date(br.get_due_date()).c_str(), table_x + 390, row_y + 7, 16, DARKGRAY);

                    Color st_color = br.get_status() == "Qua han" ? RED : DARKGREEN;
                    DrawText(display_status(br.get_status()).c_str(), table_x + 530, row_y + 7, 16, st_color);

                    row++;
                }
            }
            curr = curr->get_next();
        }
    }

    int nav_y = (int)box.y + (int)box.height - 52;

    // Phân trang
    if (total_pages > 1) {
        if (draw_button({(float)table_x, (float)nav_y, 90, 36}, "Trước", GRAY) && detail_borrow_page > 1) {
            detail_borrow_page--;
        }
        DrawText(TextFormat("Trang %d/%d", detail_borrow_page, total_pages), table_x + 105, nav_y + 8, 17, DARKGRAY);
        if (draw_button({(float)table_x + 200, (float)nav_y, 90, 36}, "Sau", GRAY) && detail_borrow_page < total_pages) {
            detail_borrow_page++;
        }
    }

    // Nút Đóng
    if (draw_button({box.x + box.width - 150, (float)nav_y, 120, 38}, "Đóng", RED)) {
        show_book_detail_popup = false;
    }
}

void library_gui::open_book_detail_popup(const std::string& book_id) {
    selected_book_id = book_id;
    detail_book_id = book_id;
    detail_borrow_page = 1;
    show_book_detail_popup = true;

    // Truy vấn dữ liệu từ Controller đúng 1 lần duy nhất khi click mở Popup
    cached_detail_borrows = controller.get_borrows_by_book(detail_book_id);
    cached_currently_borrowed_count = 0;

    node<borrow>* temp = cached_detail_borrows.get_head();
    while (temp != nullptr) {
        std::string st = temp->get_data().get_status();
        if (st == "Dang muon" || st == "Qua han") {
            cached_currently_borrowed_count++;
        }
        temp = temp->get_next();
    }
}

void library_gui::draw(const std::string& role) {
    BeginDrawing();
    ClearBackground(page_background);

    BeginMode2D(ui_layout::camera());

    // Khi hộp thoại xác nhận đang mở, khóa mọi thao tác ở phía sau.
    input_locked = confirm_open || notification_detail_open || show_book_detail_popup;

    collect_text_input();
    draw_shell(role);
    draw_sidebar(role);

    switch (current_page) {
        case DASHBOARD:           draw_dashboard(role); break;
        case BOOKS:               draw_books(role, false); break;
        case USER_SEARCH:         draw_books(role, true); break;
        case BOOK_ADD:            draw_book_form(false); break;
        case BOOK_EDIT:           draw_book_form(true); break;
        case BOOK_DELETE:         draw_book_delete(); break;
        case BORROWS:             draw_borrows(); break;
        case BORROW_CREATE:       draw_borrow_create(); break;
        case BORROW_RETURN:       draw_borrow_return(false); break;
        case BORROW_DELETE:       draw_borrow_return(true); break;
        case REQUESTS:            draw_requests(); break;
        case BORROW_FILTER:       draw_borrow_filter(); break;
        case SHIFTS:              draw_shifts(role); break;
        case SHIFT_REGISTER:      draw_shift_form(false, false); break;
        case SHIFT_CANCEL:        draw_shift_form(true, false); break;
        case SHIFT_DELETE:        draw_shift_form(false, true); break;
        case SALARY:              draw_salary(role); break;
        case USERS:               draw_users(); break;
        case USER_ADD:            draw_user_form(false); break;
        case USER_EDIT:           draw_user_form(true); break;
        case USER_DELETE:         draw_user_delete(); break;
        case SHELF:               draw_shelf(); break;
        case USER_REQUEST:        draw_user_request(); break;
        case USER_ACTIVE_BORROWS: draw_my_borrows(true); break;
        case USER_HISTORY:        draw_my_borrows(false); break;
        case USER_NOTIFICATIONS: draw_notifications(); break;
    }

    draw_message();
    draw_confirm_dialog();
    draw_book_detail_popup();

    EndMode2D();
    EndDrawing();
}