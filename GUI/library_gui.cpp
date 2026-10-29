#include "library_gui.h"
#include "raylib.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
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
}

library_gui::library_gui(library_controller& controller)
    : controller(controller) {
    reset();
}

void library_gui::reset() {
    current_page = DASHBOARD;
    logout_requested = false;
    list_page = 0;
    active_field = nullptr;
    active_field_capacity = 0;
    message.clear();
    message_is_error = false;
    showing_related = false;
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

void library_gui::open_page(page next_page) {
    current_page = next_page;
    message.clear();
    message_is_error = false;
    showing_related = false;
    clear_fields();
}

void library_gui::set_message(const std::string& text, bool is_error) {
    message = text;
    message_is_error = is_error;
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
    if (active_field == nullptr || active_field_capacity <= 1) return;

    int key = GetCharPressed();
    while (key > 0) {
        if (key >= 32 && key <= 126) {
            int length = (int)std::strlen(active_field);
            if (length < active_field_capacity - 1) {
                active_field[length] = (char)key;
                active_field[length + 1] = '\0';
            }
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int length = (int)std::strlen(active_field);
        if (length > 0) active_field[length - 1] = '\0';
    }
}

void library_gui::draw_input(Rectangle box, const char* label,
                             char* value, int capacity) {
    DrawText(label, (int)box.x, (int)box.y - 24, 18, DARKGRAY);

    bool focused = active_field == value;
    DrawRectangleRounded(box, 0.08f, 6, WHITE);
    DrawRectangleRoundedLines(box, 0.08f, 6, focused ? BLUE : LIGHTGRAY);
    DrawText(value, (int)box.x + 12, (int)box.y + 11, 20, DARKGRAY);

    if (CheckCollisionPointRec(ui_layout::mouse_position(), box) &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        active_field = value;
        active_field_capacity = capacity;
    }
}

bool library_gui::draw_button(Rectangle box, const char* label, Color color) {
    Vector2 mouse = ui_layout::mouse_position();
    bool hovered = CheckCollisionPointRec(mouse, box);
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
        draw_button({CONTENT_X + CONTENT_W - 150, 115, 130, 42}, "Quay lai", GRAY)) {
        open_page(DASHBOARD);
    }
}

void library_gui::draw_shell(const std::string& role) {
    ClearBackground(page_background);
    DrawRectangle(0, 0, WINDOW_W, 82, DARKBLUE);
    DrawText("PBL2 - QUAN LY THU VIEN", 28, 25, 28, WHITE);

    const char* role_name = "Doc gia";
    if (role == "admin") role_name = "Quan tri vien";
    if (role == "manager") role_name = "Quan ly";
    DrawText(role_name, WINDOW_W - 195, 29, 22, WHITE);

    DrawRectangle(0, 82, SIDEBAR_W, WINDOW_H - 82, sidebar_background);
    DrawText("CHUC NANG", 28, 110, 21, DARKGRAY);
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
            {"Tong quan", DASHBOARD},
            {"Quan ly sach", BOOKS},
            {"Phieu muon", BORROWS},
            {"Yeu cau muon", REQUESTS},
            {"Ca truc", SHIFTS},
            {"Tinh luong", SALARY},
            {"Nguoi dung", USERS},
            {"Vi tri ke", SHELF}
        };
    } else if (role == "admin") {
        menu = {
            {"Tong quan", DASHBOARD},
            {"Quan ly sach", BOOKS},
            {"Phieu muon", BORROWS},
            {"Yeu cau muon", REQUESTS},
            {"Loc phieu muon", BORROW_FILTER},
            {"Ca truc cua toi", SHIFTS},
            {"Luong cua toi", SALARY},
            {"Vi tri ke", SHELF}
        };
    } else {
        menu = {
            {"Tong quan", DASHBOARD},
            {"Tim kiem sach", USER_SEARCH},
            {"Dang ky muon", USER_REQUEST},
            {"Vi tri ke sach", SHELF},
            {"Sach dang muon", USER_ACTIVE_BORROWS},
            {"Lich su muon", USER_HISTORY}
        };
    }

    Vector2 mouse = ui_layout::mouse_position();
    for (int i = 0; i < (int)menu.size(); ++i) {
        Rectangle item = {18, (float)(150 + i * 62), 244, 48};
        bool hovered = CheckCollisionPointRec(mouse, item);
        bool selected = current_page == menu[i].destination;

        if (selected) {
            DrawRectangleRounded(item, 0.12f, 6, BLUE);
        } else if (hovered) {
            DrawRectangleRounded(item, 0.12f, 6, {215, 225, 240, 255});
        }

        DrawText(menu[i].label, 32, 164 + i * 62, 20,
                 selected ? WHITE : DARKGRAY);

        if (hovered && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            open_page(menu[i].destination);
        }
    }

    if (draw_button({28, 665, 224, 50}, "DANG XUAT", RED)) {
        logout_requested = true;
    }
}

void library_gui::draw_dashboard(const std::string& role) {
    DrawText("Tong quan", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    user* current_user = controller.get_current_user();
    std::string name = current_user == nullptr ? "" : current_user->get_name();

    DrawText(TextFormat("Xin chao, %s!", name.c_str()),
             CONTENT_X + 35, 195, 28, DARKBLUE);

    if (role == "manager") {
        DrawText("Ban co quyen quan ly sach, phieu muon, ca truc va tai khoan.",
                 CONTENT_X + 35, 250, 21, DARKGRAY);
    } else if (role == "admin") {
        DrawText("Ban co quyen quan ly sach, phieu muon va ca truc ca nhan.",
                 CONTENT_X + 35, 250, 21, DARKGRAY);
    } else {
        DrawText("Dung menu ben trai de tim sach va quan ly yeu cau muon.",
                 CONTENT_X + 35, 250, 21, DARKGRAY);
    }
}

void library_gui::draw_books(const std::string& role, bool user_search) {
    DrawText(user_search ? "Tim kiem sach" : "Quan ly sach",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({CONTENT_X + 20, 185, 340, 44},
               "Tu khoa: ten sach, tac gia hoac the loai",
               field_a, sizeof(field_a));
    if (draw_button({CONTENT_X + 375, 185, 95, 44}, "Tim")) {
        list_page = 0;
        showing_related = false;
    }
    if (draw_button({CONTENT_X + 485, 185, 110, 44}, "Tat ca", GRAY)) {
        field_a[0] = '\0';
        list_page = 0;
        showing_related = false;
    }

    if (!user_search) {
        if (draw_button({CONTENT_X + 615, 185, 145, 44}, "Them sach")) {
            open_page(BOOK_ADD);
            return;
        }
    }

    draw_input({CONTENT_X + 20, 255, 340, 44},
               "Ma sach de xem goi y cung the loai",
               field_b, sizeof(field_b));
    if (draw_button({CONTENT_X + 375, 255, 110, 44}, "Goi y", DARKGREEN)) {
        if (controller.get_book_by_id(field_b) == nullptr) {
            set_message("Khong tim thay ma sach de goi y.", true);
            showing_related = false;
        } else {
            showing_related = true;
            list_page = 0;
        }
    }
    if (!user_search) {
        if (draw_button({CONTENT_X + 515, 255, 140, 44}, "Sua sach", ORANGE)) {
            open_page(BOOK_EDIT);
            return;
        }
        if (draw_button({CONTENT_X + 670, 255, 140, 44}, "Xoa sach", RED)) {
            open_page(BOOK_DELETE);
            return;
        }
    }

    linked_list<book> books;
    std::string result_label = "Ket qua tim kiem";
    if (showing_related) {
        node<book>* selected = controller.get_book_by_id(field_b);
        if (selected != nullptr) {
            book source = selected->get_data();
            books = controller.get_related_books(source.get_category(), source.get_id());
            result_label = "Sach goi y cung the loai: " + source.get_category();
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

    const char* headers[] = {"Ma", "Ten sach", "Tac gia", "The loai", "SL", "Nam", "Ke"};
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
        DrawText(controller.get_book_shelf(item.get_id()).c_str(),
                 1125, y + 9, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }

    if (total == 0) {
        DrawText("Khong tim thay sach phu hop.", 330, 405, 20, RED);
    }

    if (draw_button({CONTENT_X + 230, 620, 110, 40}, "Truoc", GRAY) &&
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
    DrawText(editing ? "Sua thong tin sach" : "Them sach moi",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (!editing) {
        draw_input({340, 195, 360, 44}, "Ma sach", field_a, sizeof(field_a));
        draw_input({740, 195, 400, 44}, "Ten sach", field_b, sizeof(field_b));
        draw_input({340, 280, 360, 44}, "Tac gia", field_c, sizeof(field_c));
        draw_input({740, 280, 400, 44}, "The loai", field_d, sizeof(field_d));
        draw_input({340, 365, 360, 44}, "So luong (0 - 10000)", field_e, sizeof(field_e));
        draw_input({740, 365, 400, 44}, "Nam xuat ban", field_f, sizeof(field_f));

        if (draw_button({530, 470, 220, 48}, "Luu sach")) {
            int quantity = 0;
            int year = 0;
            if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
                is_blank(field_d) ||
                !read_integer(field_e, quantity, 0, 10000) ||
                !read_integer(field_f, year, 1000, 9999)) {
                set_message("Nhap day du thong tin va so hop le.", true);
            } else if (controller.add_new_book(field_a, field_b, field_c, field_d,
                                               quantity, year)) {
                set_message("Da them sach moi.");
                clear_fields();
            } else {
                set_message("Khong them duoc: ma sach da ton tai.", true);
            }
        }
        return;
    }

    draw_input({340, 195, 360, 44}, "Ma sach cu", field_a, sizeof(field_a));
    draw_input({740, 195, 400, 44}, "Ma sach moi", field_b, sizeof(field_b));
    draw_input({340, 280, 360, 44}, "Ten sach", field_c, sizeof(field_c));
    draw_input({740, 280, 400, 44}, "Tac gia", field_d, sizeof(field_d));
    draw_input({340, 365, 360, 44}, "The loai", field_e, sizeof(field_e));
    draw_input({740, 365, 400, 44}, "So luong (0 - 10000)", field_f, sizeof(field_f));
    draw_input({340, 450, 360, 44}, "Nam xuat ban", field_g, sizeof(field_g));

    if (draw_button({530, 545, 220, 48}, "Cap nhat sach", ORANGE)) {
        int quantity = 0;
        int year = 0;
        if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
            is_blank(field_d) || is_blank(field_e) ||
            !read_integer(field_f, quantity, 0, 10000) ||
            !read_integer(field_g, year, 1000, 9999)) {
            set_message("Nhap day du thong tin va so hop le.", true);
        } else {
            book replacement(field_b, field_c, field_d, field_e, quantity, year);
            if (controller.update_book_info(field_a, replacement)) {
                set_message("Da cap nhat thong tin sach.");
                clear_fields();
            } else {
                set_message("Cap nhat that bai: kiem tra ma sach cu va ma sach moi.", true);
            }
        }
    }
}

void library_gui::draw_book_delete() {
    DrawText("Xoa sach", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();
    draw_input({360, 220, 420, 46}, "Ma sach can xoa", field_a, sizeof(field_a));

    if (draw_button({460, 315, 200, 48}, "Xoa sach", RED)) {
        if (is_blank(field_a)) {
            set_message("Nhap ma sach can xoa.", true);
        } else if (controller.delete_book(field_a)) {
            set_message("Da xoa sach.");
            clear_fields();
        } else {
            set_message("Khong tim thay ma sach.", true);
        }
    }
}

void library_gui::draw_borrows() {
    DrawText("Quan ly phieu muon", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    DrawText("Chon nghiep vu:", 340, 195, 22, DARKGRAY);
    if (draw_button({340, 240, 210, 50}, "Lap phieu muon")) {
        open_page(BORROW_CREATE);
        return;
    }
    if (draw_button({580, 240, 200, 50}, "Xac nhan tra", ORANGE)) {
        open_page(BORROW_RETURN);
        return;
    }
    if (draw_button({810, 240, 200, 50}, "Xoa phieu", RED)) {
        open_page(BORROW_DELETE);
        return;
    }

    DrawText("Moi phieu can co ma phieu, ma sach, ma doc gia,",
             340, 345, 21, DARKGRAY);
    DrawText("ngay muon va thoi han 7, 14 hoac 30 ngay.",
             340, 380, 21, DARKGRAY);
}

void library_gui::draw_borrow_create() {
    DrawText("Lap phieu muon sach", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({340, 195, 360, 44}, "Ma phieu", field_a, sizeof(field_a));
    draw_input({740, 195, 360, 44}, "Ma sach", field_b, sizeof(field_b));
    draw_input({340, 280, 360, 44}, "Ma doc gia", field_c, sizeof(field_c));
    draw_input({740, 280, 360, 44}, "Ngay muon (DD/MM/YYYY)", field_d, sizeof(field_d));
    draw_input({340, 365, 360, 44}, "Thoi han: 7, 14 hoac 30 ngay",
               field_e, sizeof(field_e));

    if (draw_button({530, 465, 210, 48}, "Tao phieu")) {
        date borrow_date;
        int duration = 0;
        if (is_blank(field_a) || is_blank(field_b) || is_blank(field_c) ||
            !read_date(field_d, borrow_date) ||
            !read_integer(field_e, duration, 1, 30) ||
            (duration != 7 && duration != 14 && duration != 30)) {
            set_message("Kiem tra thong tin, ngay va thoi han muon.", true);
        } else {
            date due_date = borrow_date.add_days(duration);
            if (controller.borrow_book(field_a, field_b, field_c,
                                       borrow_date, due_date)) {
                set_message(TextFormat("Da tao phieu. Han tra: %d/%d/%d",
                                       due_date.get_day(), due_date.get_month(),
                                       due_date.get_year()));
                clear_fields();
            } else {
                set_message("Tao phieu that bai: kiem tra ma, ngay va ton kho.", true);
            }
        }
    }
}

void library_gui::draw_borrow_return(bool deleting) {
    DrawText(deleting ? "Xoa phieu muon" : "Xac nhan tra sach",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({360, 220, 420, 46}, "Ma phieu muon", field_a, sizeof(field_a));
    const char* label = deleting ? "Xoa phieu" : "Xac nhan tra";
    Color color = deleting ? RED : ORANGE;

    if (draw_button({460, 315, 200, 48}, label, color)) {
        if (is_blank(field_a)) {
            set_message("Nhap ma phieu muon.", true);
        } else if ((deleting && controller.delete_borrow(field_a)) ||
                   (!deleting && controller.return_book(field_a))) {
            set_message(deleting ? "Da xoa phieu muon." : "Da xac nhan tra sach.");
            clear_fields();
        } else if (deleting) {
            set_message("Khong xoa duoc. Phieu phai ton tai va sach da tra.", true);
        } else {
            set_message("Khong tim thay phieu hoac sach da duoc tra.", true);
        }
    }
}

void library_gui::draw_requests() {
    DrawText("Duyet yeu cau muon sach", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (!controller.has_pending_requests()) {
        DrawText("Khong co yeu cau nao trong hang doi.", 340, 235, 22, DARKGRAY);
        return;
    }

    borrow request = controller.peek_next_request();
    DrawText(TextFormat("So yeu cau dang cho: %d",
                        controller.get_pending_request_count()),
             340, 190, 23, DARKBLUE);
    DrawText(TextFormat("Ma phieu: %s", request.get_id().c_str()),
             360, 250, 21, DARKGRAY);
    DrawText(TextFormat("Ma sach: %s", request.get_book_id().c_str()),
             360, 290, 21, DARKGRAY);
    DrawText(TextFormat("Ma doc gia: %s", request.get_user_id().c_str()),
             360, 330, 21, DARKGRAY);
    DrawText(TextFormat("Ngay muon: %d/%d/%d",
                        request.get_borrow_date().get_day(),
                        request.get_borrow_date().get_month(),
                        request.get_borrow_date().get_year()),
             360, 370, 21, DARKGRAY);
    DrawText(TextFormat("Han tra: %d/%d/%d",
                        request.get_due_date().get_day(),
                        request.get_due_date().get_month(),
                        request.get_due_date().get_year()),
             360, 410, 21, DARKGRAY);

    if (draw_button({360, 485, 195, 48}, "Duyet", DARKGREEN)) {
        if (controller.approve_next_request()) {
            set_message("Da duyet yeu cau va tao phieu muon.");
        } else {
            set_message("Duyet that bai: het sach hoac trung ma phieu.", true);
        }
    }
    if (draw_button({585, 485, 195, 48}, "Tu choi", RED)) {
        controller.reject_next_request();
        set_message("Da tu choi yeu cau dau hang doi.");
    }
}

void library_gui::draw_borrow_filter() {
    DrawText("Loc phieu muon theo ngay", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({330, 185, 310, 44}, "Tu ngay (DD/MM/YYYY)", field_a, sizeof(field_a));
    draw_input({670, 185, 310, 44}, "Den ngay (DD/MM/YYYY)", field_b, sizeof(field_b));
    if (draw_button({1010, 185, 110, 44}, "Loc")) list_page = 0;

    date start_date;
    date end_date;
    if (is_blank(field_a) || is_blank(field_b)) {
        DrawText("Nhap khoang ngay de hien thi danh sach phieu muon.",
                 330, 275, 20, DARKGRAY);
        return;
    }
    if (!read_date(field_a, start_date) || !read_date(field_b, end_date) ||
        end_date < start_date) {
        DrawText("Khoang ngay khong hop le.", 330, 275, 20, RED);
        return;
    }

    linked_list<borrow> records =
        controller.filter_borrows_by_date(start_date, end_date);
    int total = records.get_size();
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (list_page >= page_count) list_page = page_count - 1;

    const char* headers[] = {"Ma phieu", "Ma sach", "Ma doc gia", "Ngay muon", "Han tra", "Trang thai"};
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
        DrawText(item.get_status().c_str(), 1030, y + 11, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }
    if (total == 0) DrawText("Khong co phieu muon trong khoang ngay nay.",
                             330, 315, 20, RED);

    if (draw_button({CONTENT_X + 230, 595, 110, 40}, "Truoc", GRAY) &&
        list_page > 0) --list_page;
    if (draw_button({CONTENT_X + 550, 595, 110, 40}, "Sau", GRAY) &&
        list_page + 1 < page_count) ++list_page;
    draw_paging_hint(list_page, total, 605);
}

void library_gui::draw_shifts(const std::string& role) {
    DrawText(role == "manager" ? "Quan ly ca truc" : "Ca truc cua toi",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (draw_button({330, 175, 170, 44}, "Dang ky ca")) {
        open_page(SHIFT_REGISTER);
        return;
    }
    if (draw_button({525, 175, 170, 44}, "Rut khoi ca", ORANGE)) {
        open_page(SHIFT_CANCEL);
        return;
    }
    if (role == "manager" &&
        draw_button({720, 175, 170, 44}, "Xoa ca truc", RED)) {
        open_page(SHIFT_DELETE);
        return;
    }

    linked_list<shift> shifts = role == "manager"
        ? controller.get_all_shifts()
        : controller.get_my_shifts(controller.get_current_user()->get_id());

    int total = shifts.get_size();
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (list_page >= page_count) list_page = page_count - 1;

    const char* headers[] = {"Ngay", "Ca", "So nguoi", "Danh sach admin"};
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
        DrawText(item.get_session().c_str(), 535, y + 11, 17, DARKGRAY);
        DrawText(TextFormat("%d/4", item.get_current_count()),
                 680, y + 11, 17, DARKGRAY);
        DrawText(item.get_all_admins_string().c_str(), 840, y + 11, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }
    if (total == 0) {
        DrawText("Chua co ca truc phu hop.", 335, 310, 20, DARKGRAY);
    }

    if (draw_button({CONTENT_X + 230, 595, 110, 40}, "Truoc", GRAY) &&
        list_page > 0) --list_page;
    if (draw_button({CONTENT_X + 550, 595, 110, 40}, "Sau", GRAY) &&
        list_page + 1 < page_count) ++list_page;
    draw_paging_hint(list_page, total, 605);
}

void library_gui::draw_shift_form(bool cancelling, bool deleting) {
    const char* title = deleting ? "Xoa ca truc" :
                        cancelling ? "Rut khoi ca truc" : "Dang ky ca truc";
    DrawText(title, CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({340, 200, 360, 44}, "Ngay (DD/MM/YYYY)", field_a, sizeof(field_a));
    draw_input({740, 200, 360, 44}, "Ca: sang hoac chieu", field_b, sizeof(field_b));

    bool manager = controller.get_current_role() == "manager";
    if (!deleting && manager) {
        draw_input({340, 290, 360, 44}, "Ma admin/manager", field_c, sizeof(field_c));
    } else if (!deleting) {
        DrawText(TextFormat("Nhan vien: %s",
                            controller.get_current_user()->get_id().c_str()),
                 340, 300, 21, DARKGRAY);
    }

    const char* action = deleting ? "Xoa ca" : cancelling ? "Rut khoi ca" : "Dang ky";
    Color action_color = deleting ? RED : cancelling ? ORANGE : BLUE;
    if (!draw_button({530, 400, 210, 48}, action, action_color)) return;

    date shift_date;
    if (!read_date(field_a, shift_date) ||
        (std::strcmp(field_b, "sang") != 0 && std::strcmp(field_b, "chieu") != 0)) {
        set_message("Ngay hoac ca truc khong hop le.", true);
        return;
    }

    if (deleting) {
        if (controller.delete_shift(shift_date, field_b)) {
            set_message("Da xoa toan bo ca truc.");
            clear_fields();
        } else {
            set_message("Khong tim thay ca truc.", true);
        }
        return;
    }

    const char* person_id = manager ? field_c
                                    : controller.get_current_user()->get_id().c_str();
    if (is_blank(person_id)) {
        set_message("Nhap ma nhan vien.", true);
    } else if ((!cancelling &&
                controller.register_shift(shift_date, field_b, person_id)) ||
               (cancelling &&
                controller.cancel_shift(shift_date, field_b, person_id))) {
        set_message(cancelling ? "Da rut khoi ca truc." : "Da dang ky ca truc.");
        clear_fields();
    } else {
        set_message(cancelling
                    ? "Khong tim thay nhan vien trong ca truc."
                    : "Khong dang ky duoc: ca day, trung ca hoac sai vai tro.",
                    true);
    }
}

void library_gui::draw_salary(const std::string& role) {
    DrawText(role == "manager" ? "Tinh luong ca truc" : "Luong ca truc cua toi",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (role == "manager") {
        draw_input({340, 205, 300, 44}, "Ma admin/manager", field_a, sizeof(field_a));
        draw_input({675, 205, 210, 44}, "Thang, 0 = tat ca", field_b, sizeof(field_b));
        draw_input({920, 205, 210, 44}, "Nam, 0 = tat ca", field_c, sizeof(field_c));
    } else {
        draw_input({380, 205, 280, 44}, "Thang, 0 = tat ca", field_a, sizeof(field_a));
        draw_input({710, 205, 280, 44}, "Nam, 0 = tat ca", field_b, sizeof(field_b));
    }

    if (draw_button({530, 320, 210, 48}, "Tinh luong", DARKGREEN)) {
        const char* id = role == "manager" ? field_a
                                           : controller.get_current_user()->get_id().c_str();
        const char* month_text = role == "manager" ? field_b : field_a;
        const char* year_text = role == "manager" ? field_c : field_b;
        int month = 0;
        int year = 0;

        bool month_ok = is_blank(month_text) || read_integer(month_text, month, 0, 12);
        bool year_ok = is_blank(year_text) || read_integer(year_text, year, 0, 2026);
        if (is_blank(id) || !month_ok || !year_ok) {
            set_message("Nhap ma, thang va nam hop le.", true);
        } else {
            long long total = controller.get_admin_salary(id, month, year);
            set_message(TextFormat("Tong luong: %lld VND (%lld buoi)",
                                   total, total / 200000));
        }
    }
}

void library_gui::draw_users() {
    DrawText("Quan ly nguoi dung", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (draw_button({340, 185, 175, 48}, "Them tai khoan")) {
        open_page(USER_ADD);
        return;
    }
    if (draw_button({545, 185, 175, 48}, "Sua tai khoan", ORANGE)) {
        open_page(USER_EDIT);
        return;
    }
    if (draw_button({750, 185, 175, 48}, "Xoa tai khoan", RED)) {
        open_page(USER_DELETE);
        return;
    }

    draw_input({340, 295, 380, 44}, "Ma tai khoan can tra cuu",
               field_a, sizeof(field_a));
    if (draw_button({750, 295, 150, 44}, "Tra cuu")) {
        if (is_blank(field_a)) {
            set_message("Nhap ma tai khoan.", true);
        } else {
            node<user>* found = controller.get_user_by_id(field_a);
            if (found == nullptr) {
                set_message("Khong tim thay tai khoan.", true);
            } else {
                user item = found->get_data();
                set_message("ID: " + item.get_id() + " | Ten: " +
                            item.get_name() + " | Vai tro: " + item.get_role());
            }
        }
    }
}

void library_gui::draw_user_form(bool editing) {
    DrawText(editing ? "Cap nhat tai khoan" : "Them tai khoan moi",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    if (editing) {
        draw_input({340, 185, 360, 44}, "Ma tai khoan cu", field_a, sizeof(field_a));
        draw_input({740, 185, 360, 44}, "Ma tai khoan moi", field_b, sizeof(field_b));
        draw_input({340, 270, 360, 44}, "Ho va ten", field_c, sizeof(field_c));
        draw_input({740, 270, 360, 44}, "Mat khau", field_d, sizeof(field_d));
        draw_input({340, 355, 360, 44}, "Vai tro: manager, admin, user",
                   field_e, sizeof(field_e));
    } else {
        draw_input({340, 195, 360, 44}, "Ma tai khoan", field_a, sizeof(field_a));
        draw_input({740, 195, 360, 44}, "Ho va ten", field_b, sizeof(field_b));
        draw_input({340, 280, 360, 44}, "Mat khau", field_c, sizeof(field_c));
        draw_input({740, 280, 360, 44}, "Vai tro: manager, admin, user",
                   field_d, sizeof(field_d));
    }

    if (!draw_button({530, 465, 220, 48},
                     editing ? "Cap nhat" : "Them tai khoan")) {
        return;
    }

    const char* old_id = editing ? field_a : field_a;
    const char* id = editing ? field_b : field_a;
    const char* name = editing ? field_c : field_b;
    const char* password = editing ? field_d : field_c;
    const char* role = editing ? field_e : field_d;
    bool supported_role = std::strcmp(role, "manager") == 0 ||
                          std::strcmp(role, "admin") == 0 ||
                          std::strcmp(role, "user") == 0;

    if (is_blank(old_id) || is_blank(id) || is_blank(name) ||
        is_blank(password) || !supported_role) {
        set_message("Nhap day du thong tin va vai tro hop le.", true);
    } else if ((!editing && controller.add_new_user(id, name, password, role)) ||
               (editing && controller.update_user_info(old_id,
                                                        user(id, name, password, role)))) {
        set_message(editing ? "Da cap nhat tai khoan." : "Da them tai khoan.");
        clear_fields();
    } else {
        set_message(editing ? "Cap nhat that bai." : "Ma tai khoan da ton tai.", true);
    }
}

void library_gui::draw_user_delete() {
    DrawText("Xoa tai khoan", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();
    draw_input({360, 220, 420, 46}, "Ma tai khoan can xoa",
               field_a, sizeof(field_a));

    if (draw_button({460, 315, 210, 48}, "Xoa tai khoan", RED)) {
        if (is_blank(field_a)) {
            set_message("Nhap ma tai khoan can xoa.", true);
        } else if (controller.delete_user(field_a)) {
            set_message("Da xoa tai khoan.");
            clear_fields();
        } else {
            set_message("Khong tim thay tai khoan.", true);
        }
    }
}

void library_gui::draw_shelf() {
    DrawText("Tra cuu vi tri ke sach", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({360, 220, 420, 46}, "Ma sach", field_a, sizeof(field_a));
    if (draw_button({480, 315, 180, 48}, "Tra cuu")) {
        if (is_blank(field_a)) {
            set_message("Nhap ma sach.", true);
        } else {
            node<book>* found = controller.get_book_by_id(field_a);
            if (found == nullptr) {
                set_message("Khong tim thay sach.", true);
            } else {
                book item = found->get_data();
                set_message("Sach: " + item.get_title() +
                            " | The loai: " + item.get_category() +
                            " | Ke: " + controller.get_book_shelf(item.get_id()));
            }
        }
    }
}

void library_gui::draw_user_request() {
    DrawText("Dang ky muon sach", CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    draw_input({340, 195, 360, 44}, "Ma yeu cau / ma phieu", field_a, sizeof(field_a));
    draw_input({740, 195, 360, 44}, "Ma sach", field_b, sizeof(field_b));
    draw_input({340, 280, 360, 44}, "Ngay muon (DD/MM/YYYY)", field_c, sizeof(field_c));
    draw_input({740, 280, 360, 44}, "Thoi han: 7, 14 hoac 30 ngay",
               field_d, sizeof(field_d));

    if (draw_button({530, 390, 210, 48}, "Gui yeu cau")) {
        date borrow_date;
        int duration = 0;
        if (is_blank(field_a) || is_blank(field_b) ||
            !read_date(field_c, borrow_date) ||
            !read_integer(field_d, duration, 1, 30) ||
            (duration != 7 && duration != 14 && duration != 30)) {
            set_message("Kiem tra thong tin, ngay va thoi han muon.", true);
        } else {
            date due_date = borrow_date.add_days(duration);
            if (controller.register_book_request(
                    field_a, field_b, controller.get_current_user()->get_id(),
                    borrow_date, due_date)) {
                set_message("Da dua yeu cau vao hang doi.");
                clear_fields();
            } else {
                set_message("Ma sach khong ton tai.", true);
            }
        }
    }
}

void library_gui::draw_my_borrows(bool active_only) {
    DrawText(active_only ? "Sach dang muon" : "Lich su muon sach",
             CONTENT_X + 25, 120, 30, DARKBLUE);
    draw_back_button();

    linked_list<borrow> records = controller.get_my_borrows(
        controller.get_current_user()->get_id(), active_only);
    int total = records.get_size();
    int page_count = total == 0 ? 1 : (total + ROWS_PER_PAGE - 1) / ROWS_PER_PAGE;
    if (list_page >= page_count) list_page = page_count - 1;

    const char* headers[] = {"Ma phieu", "Ten sach", "Ngay muon", "Han tra", "Trang thai"};
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
        DrawText(item.get_status().c_str(), 1045, y + 12, 17, DARKGRAY);
        current = current->get_next();
        ++row;
    }

    if (total == 0) {
        DrawText(active_only ? "Ban khong co sach dang muon."
                             : "Ban chua co lich su muon sach.",
                 330, 260, 20, DARKGRAY);
    }
    if (draw_button({CONTENT_X + 230, 595, 110, 40}, "Truoc", GRAY) &&
        list_page > 0) --list_page;
    if (draw_button({CONTENT_X + 550, 595, 110, 40}, "Sau", GRAY) &&
        list_page + 1 < page_count) ++list_page;
    draw_paging_hint(list_page, total, 605);
}

void library_gui::draw(const std::string& role) {
    BeginDrawing();
    ClearBackground(page_background);

    BeginMode2D(ui_layout::camera());

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

    EndMode2D();
    EndDrawing();
}