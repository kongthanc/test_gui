#include "library_gui.h"
#include "raylib.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <vector>
#include "ui_font.h"
#include "ui_layout.h"

namespace {
    const int WINDOW_W = 1220;
    const int WINDOW_H = 750;
    const int SIDEBAR_W = 280;
    const int CONTENT_X = 305;
    const int CONTENT_W = 890;
    const int ROWS_PER_PAGE = 6;

    Color page_background = {245, 247, 251, 255};
    Color panel_background = {255, 255, 255, 255};
    Color sidebar_background = {235, 239, 246, 255};

    void draw_panel() {
        DrawRectangleRounded(
            {(float)CONTENT_X, 102.0f, (float)CONTENT_W, 578.0f},
            0.03f, 8, panel_background);
    }

    void draw_table_header(const char* const* headers, const int* columns,
                           int count, int y) {
        DrawRectangle(CONTENT_X + 15, y, CONTENT_W - 30, 38, DARKBLUE);
        for (int i = 0; i < count; ++i) {
            DrawText(headers[i], columns[i], y + 9, 18, WHITE);
        }
    }

    void draw_paging_hint(int page, int total, int y) {
        int total_pages = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
        DrawText(TextFormat("Trang %d/%d", page + 1, total_pages),
                 CONTENT_X + 395, y, 19, DARKGRAY);
    }

    // ---- Cac ham doi chuoi luu trong file du lieu (khong dau) sang tieng Viet co dau.
    // ---- Chi dung de HIEN THI, du lieu goc va logic nghiep vu khong thay doi.
    std::string display_status(const std::string& status) {
        if (status == "Dang muon") return "Đang mượn";
        if (status == "Da tra") return "Đã trả";
        if (status == "Qua han") return "Quá hạn";
        if (status == "Cho duyet") return "Chờ duyệt";
        return status;
    }

    std::string display_session(const std::string& session) {
        if (session == "sang") return "Sáng";
        if (session == "chieu") return "Chiều";
        return session;
    }

    std::string display_role(const std::string& role) {
        if (role == "manager") return "Quản lý";
        if (role == "admin") return "Quản trị viên";
        if (role == "user") return "Độc giả";
        return role;
    }

    std::string display_shelf(const std::string& shelf) {
        if (shelf == "Chua xep ke") return "Chưa xếp kệ";
        return shelf;
    }

    std::string format_date(const date& d) {
        return std::to_string(d.get_day()) + "/" +
               std::to_string(d.get_month()) + "/" +
               std::to_string(d.get_year());
    }

    // Chap nhan "sang"/"sáng" va "chieu"/"chiều"; tra ve gia tri luu trong he thong.
    std::string normalize_session(const char* text) {
        std::string s = text == nullptr ? "" : text;
        size_t start = 0;
        while (start < s.size() && s[start] == ' ') ++start;
        size_t end = s.size();
        while (end > start && s[end - 1] == ' ') --end;
        s = s.substr(start, end - start);

        std::string lower;
        for (size_t i = 0; i < s.size(); ++i) {
            char c = s[i];
            lower += (c >= 'A' && c <= 'Z') ? (char)(c + 32) : c;
        }
        if (lower == "sang" || lower == "sáng") return "sang";
        if (lower == "chieu" || lower == "chiều") return "chieu";
        return "";
    }
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
    active_field = nullptr;
    active_field_capacity = 0;
    message.clear();
    message_is_error = false;
    showing_related = false;
    confirm_open = false;
    confirm_just_opened = false;
    input_locked = false;
    confirm_title.clear();
    confirm_text.clear();
    confirm_ok_label.clear();
    confirm_ok_color = BLUE;
    confirm_action = nullptr;
    clear_fields();
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
    active_field = nullptr;
    active_field_capacity = 0;
    list_page = 0;
}

void library_gui::switch_page(page next_page) {
    current_page = next_page;
    message.clear();
    message_is_error = false;
    showing_related = false;
    clear_fields();
}

// Di vao trang con (them/sua/xoa...): nho trang hien tai de nut "Quay lai" tro ve dung buoc truoc.
void library_gui::open_page(page next_page) {
    if (next_page == current_page) return;
    history.push_back(current_page);
    switch_page(next_page);
}

// Chon muc tu menu ben trai: bat dau mot luong moi, "Quay lai" se ve Tong quan.
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
    confirm_action = action;
}

void library_gui::draw_confirm_dialog() {
    if (!confirm_open) return;

    // Hop thoai tu xu ly nut cua no, nen mo khoa dau vao cho no.
    // Frame vua mo thi van khoa de cu click mo hop thoai khong bi tinh la "Xac nhan".
    input_locked = confirm_just_opened;
    confirm_just_opened = false;

    DrawRectangle(0, 0, WINDOW_W, WINDOW_H, {0, 0, 0, 130});

    Rectangle box = {340, 220, 540, 290};
    DrawRectangleRounded(box, 0.05f, 10, WHITE);
    DrawRectangleRoundedLines(box, 0.05f, 10, DARKBLUE);

    DrawText(confirm_title.c_str(), (int)box.x + 28, (int)box.y + 22, 26, DARKBLUE);
    DrawRectangle((int)box.x + 28, (int)box.y + 60, (int)box.width - 56, 2, LIGHTGRAY);

    // Tach noi dung theo dau xuong dong.
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

    bool cancelled = draw_button({box.x + 60, box.y + 220, 190, 48}, "Hủy", GRAY);
    bool confirmed = draw_button({box.x + 290, box.y + 220, 190, 48},
                                 confirm_ok_label.c_str(), confirm_ok_color);

    if (confirmed) {
        std::function<void()> action = confirm_action;
        confirm_open = false;
        confirm_action = nullptr;
        if (action) action();
    } else if (cancelled) {
        confirm_open = false;
        confirm_action = nullptr;
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

    date parsed(day, month, year);
    if (parsed.get_day() != day ||
        parsed.get_month() != month ||
        parsed.get_year() != year) {
        return false;
    }

    value = parsed;
    return true;
}

void library_gui::collect_text_input() {
    if (input_locked || active_field == nullptr || active_field_capacity <= 1) return;

    int key = GetCharPressed();
    while (key > 0) {
        // ASCII + cac ky tu tieng Viet (Latin-1, Latin mo rong, Latin mo rong bo sung).
        bool accepted = (key >= 32 && key <= 126) ||
                        (key >= 0xC0 && key <= 0x1EF9);
        if (accepted) {
            int size = 0;
            const char* encoded = CodepointToUTF8(key, &size);
            int length = (int)std::strlen(active_field);
            if (size > 0 && length + size < active_field_capacity) {
                std::memcpy(active_field + length, encoded, size);
                active_field[length + size] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int length = (int)std::strlen(active_field);
        if (length > 0) {
            // Xoa tron mot ky tu UTF-8 (co the gom nhieu byte).
            --length;
            while (length > 0 &&
                   (((unsigned char)active_field[length]) & 0xC0) == 0x80) {
                --length;
            }
            active_field[length] = '\0';
        }
    }
}

void library_gui::draw_input(Rectangle box, const char* label,
                             char* value, int capacity) {
    DrawText(label, (int)box.x, (int)box.y - 24, 18, DARKGRAY);

    bool focused = active_field == value;
    DrawRectangleRounded(box, 0.08f, 6, WHITE);
    DrawRectangleRoundedLines(box, 0.08f, 6, focused ? BLUE : LIGHTGRAY);
    DrawText(value, (int)box.x + 12, (int)box.y + 11, 20, DARKGRAY);

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
    if (current_page != DASHBOARD &&
        draw_button({CONTENT_X + CONTENT_W - 150, 115, 130, 42}, "Quay lại", GRAY)) {
        go_back();
    }
}

void library_gui::draw_shell(const std::string& role) {
    ClearBackground(page_background);
    DrawRectangle(0, 0, WINDOW_W, 82, DARKBLUE);
    DrawText("PBL2 - QUẢN LÝ THƯ VIỆN", 28, 25, 28, WHITE);

    std::string role_name = display_role(role);
    int role_width = MeasureText(role_name.c_str(), 22);
    DrawText(role_name.c_str(), WINDOW_W - 28 - role_width, 29, 22, WHITE);

    DrawRectangle(0, 82, SIDEBAR_W, WINDOW_H - 82, sidebar_background);
    DrawText("CHỨC NĂNG", 28, 110, 21, DARKGRAY);
    draw_panel();
}

void library_gui::draw_sidebar(const std::string& role) {
    struct menu_item {
        const char* label;
        page destination;
    };

    std::vector<menu_item> menu;

    if (role == "manager") {
        menu = {
            {"Tổng quan", DASHBOARD},
            {"Quản lý sách", BOOKS},
            {"Phiếu mượn", BORROWS},
            {"Yêu cầu mượn", REQUESTS},
            {"Ca trực", SHIFTS},
            {"Tính lương", SALARY},
            {"Người dùng", USERS},
            {"Vị trí kệ", SHELF}
        };
    } else if (role == "admin") {
        menu = {
            {"Tổng quan", DASHBOARD},
            {"Quản lý sách", BOOKS},
            {"Phiếu mượn", BORROWS},
            {"Yêu cầu mượn", REQUESTS},
            {"Lọc phiếu mượn", BORROW_FILTER},
            {"Ca trực của tôi", SHIFTS},
            {"Lương của tôi", SALARY},
            {"Vị trí kệ", SHELF}
        };
    } else {
        menu = {
            {"Tổng quan", DASHBOARD},
            {"Tìm kiếm sách", USER_SEARCH},
            {"Đăng ký mượn", USER_REQUEST},
            {"Vị trí kệ sách", SHELF},
            {"Sách đang mượn", USER_ACTIVE_BORROWS},
            {"Lịch sử mượn", USER_HISTORY}
        };
    }

    Vector2 mouse = ui_layout::mouse_position();
    for (int i = 0; i < (int)menu.size(); ++i) {
        Rectangle item = {18, (float)(150 + i * 62), 244, 48};
        bool hovered = !input_locked && CheckCollisionPointRec(mouse, item);
        bool selected = current_page == menu[i].destination;

        if (selected) {
            DrawRectangleRounded(item, 0.12f, 6, BLUE);
        } else if (hovered) {
            DrawRectangleRounded(item, 0.12f, 6, {215, 225, 240, 255});
        }

        DrawText(menu[i].label, 32, 164 + i * 62, 20,
                 selected ? WHITE : DARKGRAY);

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            open_root_page(menu[i].destination);
        }
    }

    if (draw_button({28, 665, 224, 50}, "ĐĂNG XUẤT", RED)) {
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
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({CONTENT_X + 20, 185, 340, 44},
               "Từ khóa: tên sách, tác giả hoặc thể loại",
               field_a, sizeof(field_a));
    if (draw_button({CONTENT_X + 375, 185, 95, 44}, "Tìm")) {
        list_page = 0;
        showing_related = false;
    }
    if (draw_button({CONTENT_X + 485, 185, 110, 44}, "Tất cả", GRAY)) {
        field_a[0] = '\0';
        list_page = 0;
        showing_related = false;
    }

    if (!user_search) {
        if (draw_button({CONTENT_X + 615, 185, 145, 44}, "Thêm sách")) {
            open_page(BOOK_ADD);
            return;
        }
    }

    draw_input({CONTENT_X + 20, 255, 340, 44},
               "Mã sách để xem gợi ý cùng thể loại",
               field_b, sizeof(field_b));
    if (draw_button({CONTENT_X + 375, 255, 110, 44}, "Gợi ý", DARKGREEN)) {
        if (controller.get_book_by_id(field_b) == nullptr) {
            set_message("Không tìm thấy mã sách để gợi ý.", true);
            showing_related = false;
        } else {
            showing_related = true;
            list_page = 0;
        }
    }
    if (!user_search) {
        if (draw_button({CONTENT_X + 515, 255, 140, 44}, "Sửa sách", ORANGE)) {
            open_page(BOOK_EDIT);
            return;
        }
        if (draw_button({CONTENT_X + 670, 255, 140, 44}, "Xóa sách", RED)) {
            open_page(BOOK_DELETE);
            return;
        }
    }

    linked_list<book> books;
    std::string result_label = "Kết quả tìm kiếm";
    if (showing_related) {
        node<book>* selected = controller.get_book_by_id(field_b);
        if (selected != nullptr) {
            book source = selected->get_data();
            books = controller.get_related_books(source.get_category(), source.get_id());
            result_label = "Sách gợi ý cùng thể loại: " + source.get_category();
        } else {
            showing_related = false;
            books = controller.search_books(field_a);
        }
    } else {
        books = controller.search_books(field_a);
    }
    int total = books.get_size();
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (list_page >= page_count) list_page = page_count - 1;

    const char* headers[] = {"Mã", "Tên sách", "Tác giả", "Thể loại", "SL", "Năm", "Kệ"};
    const int columns[] = {330, 405, 675, 865, 1005, 1060, 1125};
    DrawText(result_label.c_str(), 325, 312, 18, DARKGRAY);
    draw_table_header(headers, columns, 7, 340);

    node<book>* current = books.get_head();
    int skip = list_page * ROWS_PER_PAGE;
    while (current != nullptr && skip-- > 0) current = current->get_next();

    int row = 0;
    while (current != nullptr && row < ROWS_PER_PAGE) {
        book item = current->get_data();
        int y = 380 + row * 38;
        DrawRectangle(CONTENT_X + 15, y, CONTENT_W - 30, 36,
                      row % 2 == 0 ? Color{245, 248, 252, 255}
                                   : Color{232, 239, 248, 255});
        DrawText(item.get_id().c_str(), 330, y + 9, 17, DARKGRAY);
        DrawText(item.get_title().c_str(), 405, y + 9, 17, DARKGRAY);
        DrawText(item.get_author().c_str(), 675, y + 9, 17, DARKGRAY);
        DrawText(item.get_category().c_str(), 865, y + 9, 17, DARKGRAY);
        DrawText(TextFormat("%d", item.get_quantity()), 1005, y + 9, 17, DARKGRAY);
        DrawText(TextFormat("%d", item.get_year()), 1060, y + 9, 17, DARKGRAY);
        DrawText(display_shelf(controller.get_book_shelf(item.get_id())).c_str(),
                 1125, y + 9, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }

    if (total == 0) {
        DrawText("Không tìm thấy sách phù hợp.", 330, 405, 20, RED);
    }

    if (draw_button({CONTENT_X + 230, 620, 110, 40}, "Trước", GRAY) &&
        list_page > 0) {
        --list_page;
    }
    if (draw_button({CONTENT_X + 550, 620, 110, 40}, "Sau", GRAY) &&
        list_page + 1 < page_count) {
        ++list_page;
    }
    draw_paging_hint(list_page, total, 630);
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

        if (draw_button({530, 470, 220, 48}, "Lưu sách")) {
            int quantity = 0;
            int year = 0;
            if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
                is_blank(field_d) ||
                !read_integer(field_e, quantity, 0, 10000) ||
                !read_integer(field_f, year, 1000, 9999)) {
                set_message("Nhập đầy đủ thông tin và số hợp lệ.", true);
            } else if (controller.get_book_by_id(field_a) != nullptr) {
                set_message("Không thêm được: mã sách đã tồn tại.", true);
            } else {
                std::string id = field_a;
                std::string title = field_b;
                std::string author = field_c;
                std::string category = field_d;
                ask_confirm("Xác nhận thêm sách",
                            "Thêm sách mới vào thư viện?\n"
                            "Mã sách: " + id + "\n"
                            "Tên sách: " + title + "\n"
                            "Số lượng: " + std::to_string(quantity),
                            "Thêm sách", BLUE,
                            [this, id, title, author, category, quantity, year]() {
                    if (controller.add_new_book(id, title, author, category,
                                                quantity, year)) {
                        set_message("Đã thêm sách mới.");
                        clear_fields();
                    } else {
                        set_message("Không thêm được: mã sách đã tồn tại.", true);
                    }
                });
            }
        }
        return;
    }

    draw_input({340, 195, 360, 44}, "Mã sách cũ", field_a, sizeof(field_a));
    draw_input({740, 195, 400, 44}, "Mã sách mới", field_b, sizeof(field_b));
    draw_input({340, 280, 360, 44}, "Tên sách", field_c, sizeof(field_c));
    draw_input({740, 280, 400, 44}, "Tác giả", field_d, sizeof(field_d));
    draw_input({340, 365, 360, 44}, "Thể loại", field_e, sizeof(field_e));
    draw_input({740, 365, 400, 44}, "Số lượng (0 - 10000)", field_f, sizeof(field_f));
    draw_input({340, 450, 360, 44}, "Năm xuất bản", field_g, sizeof(field_g));

    if (draw_button({530, 545, 220, 48}, "Cập nhật sách", ORANGE)) {
        int quantity = 0;
        int year = 0;
        if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
            is_blank(field_d) || is_blank(field_e) ||
            !read_integer(field_f, quantity, 0, 10000) ||
            !read_integer(field_g, year, 1000, 9999)) {
            set_message("Nhập đầy đủ thông tin và số hợp lệ.", true);
        } else if (controller.get_book_by_id(field_a) == nullptr) {
            set_message("Không tìm thấy mã sách cũ.", true);
        } else {
            std::string old_id = field_a;
            std::string new_id = field_b;
            std::string title = field_c;
            std::string author = field_d;
            std::string category = field_e;
            ask_confirm("Xác nhận cập nhật sách",
                        "Cập nhật thông tin sách " + old_id + "?\n"
                        "Mã sách mới: " + new_id + "\n"
                        "Tên sách: " + title + "\n"
                        "Số lượng: " + std::to_string(quantity),
                        "Cập nhật", ORANGE,
                        [this, old_id, new_id, title, author, category, quantity, year]() {
                book replacement(new_id, title, author, category, quantity, year);
                if (controller.update_book_info(old_id, replacement)) {
                    set_message("Đã cập nhật thông tin sách.");
                    clear_fields();
                } else {
                    set_message("Cập nhật thất bại: kiểm tra mã sách cũ và mã sách mới.", true);
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
                set_message("Sách đang có người mượn, không thể xóa.", true);
            }
        });
    }
}

void library_gui::draw_borrows() {
    DrawText("Quản lý phiếu mượn", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    DrawText("Chọn nghiệp vụ:", 340, 195, 22, DARKGRAY);
    if (draw_button({340, 240, 210, 50}, "Lập phiếu mượn")) {
        open_page(BORROW_CREATE);
        return;
    }
    if (draw_button({580, 240, 200, 50}, "Xác nhận trả", ORANGE)) {
        open_page(BORROW_RETURN);
        return;
    }
    if (draw_button({810, 240, 200, 50}, "Xóa phiếu", RED)) {
        open_page(BORROW_DELETE);
        return;
    }

    DrawText("Mỗi phiếu cần có mã phiếu, mã sách, mã độc giả,",
             340, 345, 21, DARKGRAY);
    DrawText("ngày mượn và thời hạn 7, 14 hoặc 30 ngày.",
             340, 380, 21, DARKGRAY);
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
        if (controller.borrow_book(borrow_id, book_id, user_id,
                                   borrow_date, due_date)) {
            set_message("Đã tạo phiếu. Hạn trả: " + due_text);
            clear_fields();
        } else {
            set_message("Tạo phiếu thất bại. Kiểm tra mã phiếu, "
                        "số lượng sách hoặc thông tin mượn.", true);
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
    DrawText(TextFormat("Số yêu cầu đang chờ: %d",
                        controller.get_pending_request_count()),
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

    if (draw_button({360, 485, 195, 48}, "Duyệt", DARKGREEN)) {
        ask_confirm("Xác nhận duyệt yêu cầu",
                    "Duyệt yêu cầu mượn sách này?\n" + summary,
                    "Duyệt", DARKGREEN,
                    [this]() {
            if (controller.approve_next_request()) {
                set_message("Đã duyệt yêu cầu và tạo phiếu mượn.");
            } else {
                set_message("Duyệt thất bại: hết sách hoặc trùng mã phiếu.", true);
            }
        });
        return;
    }
    if (draw_button({585, 485, 195, 48}, "Từ chối", RED)) {
        ask_confirm("Xác nhận từ chối yêu cầu",
                    "Từ chối yêu cầu mượn sách này?\n" + summary,
                    "Từ chối", RED,
                    [this]() {
            controller.reject_next_request();
            set_message("Đã từ chối yêu cầu đầu hàng đợi.");
        });
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

    if (draw_button({340, 185, 175, 48}, "Thêm tài khoản")) {
        open_page(USER_ADD);
        return;
    }
    if (draw_button({545, 185, 175, 48}, "Sửa tài khoản", ORANGE)) {
        open_page(USER_EDIT);
        return;
    }
    if (draw_button({750, 185, 175, 48}, "Xóa tài khoản", RED)) {
        open_page(USER_DELETE);
        return;
    }

    draw_input({340, 295, 380, 44}, "Mã tài khoản cần tra cứu",
               field_a, sizeof(field_a));
    if (draw_button({750, 295, 150, 44}, "Tra cứu")) {
        if (is_blank(field_a)) {
            set_message("Nhập mã tài khoản.", true);
        } else {
            node<user>* found = controller.get_user_by_id(field_a);
            if (found == nullptr) {
                set_message("Không tìm thấy tài khoản.", true);
            } else {
                user item = found->get_data();
                set_message("Mã: " + item.get_id() + " | Tên: " +
                            item.get_name() + " | Vai trò: " +
                            display_role(item.get_role()));
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
        ask_confirm("Xác nhận cập nhật tài khoản",
                    "Cập nhật tài khoản " + old_id + "?\n" + summary,
                    "Cập nhật", ORANGE,
                    [this, old_id, id, name, password, role]() {
            if (controller.update_user_info(old_id, user(id, name, password, role))) {
                set_message("Đã cập nhật tài khoản.");
                clear_fields();
            } else {
                set_message("Cập nhật thất bại.", true);
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
    if (controller.has_active_borrows(field_a)) {
        set_message("Không thể xóa tài khoản đang mượn sách "
                    "hoặc còn sách quá hạn chưa trả.", true);
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
            set_message("Xóa tài khoản thất bại.", true);
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
    DrawText("Đăng ký mượn sách", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({340, 195, 360, 44}, "Mã yêu cầu / mã phiếu", field_a, sizeof(field_a));
    draw_input({740, 195, 360, 44}, "Mã sách", field_b, sizeof(field_b));
    draw_input({340, 280, 360, 44}, "Ngày mượn (DD/MM/YYYY)", field_c, sizeof(field_c));
    draw_input({740, 280, 360, 44}, "Thời hạn: 7, 14 hoặc 30 ngày",
               field_d, sizeof(field_d));

    if (!draw_button({530, 390, 210, 48}, "Gửi yêu cầu")) return;

    date borrow_date;
    int duration = 0;
    if (is_blank(field_a) || is_blank(field_b) ||
        !read_date(field_c, borrow_date) ||
        !read_integer(field_d, duration, 1, 30) ||
        (duration != 7 && duration != 14 && duration != 30)) {
        set_message("Kiểm tra thông tin, ngày và thời hạn mượn.", true);
        return;
    }

    user* current_user = controller.get_current_user();
    if (current_user == nullptr) {
        set_message("Vui lòng đăng nhập lại.", true);
        return;
    }
    std::string user_id = current_user->get_id();
    if (controller.has_overdue_borrows(user_id)) {
        set_message("Bạn còn sách quá hạn chưa trả, "
                    "không thể đăng ký mượn sách.", true);
        return;
    }
    std::string request_id = field_a;
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
                [this, request_id, book_id, user_id, borrow_date, due_date]() {
        if (controller.register_book_request(request_id, book_id, user_id,
                                             borrow_date, due_date)) {
            set_message("Đã đưa yêu cầu vào hàng đợi.");
            clear_fields();
        } else {
            set_message("Gửi yêu cầu thất bại. Kiểm tra mã yêu cầu, "
                        "số lượng sách hoặc lịch sử mượn.", true);
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

void library_gui::draw(const std::string& role) {
    BeginDrawing();
    ClearBackground(page_background);

    BeginMode2D(ui_layout::camera());

    // Khi hop thoai xac nhan dang mo, khoa moi thao tac o phia sau.
    input_locked = confirm_open;

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
    }

    draw_message();
    draw_confirm_dialog();

    EndMode2D();
    EndDrawing();
}