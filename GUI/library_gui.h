#pragma once

#include "raylib.h"
#include "../Controller/library_controller.h"
#include <functional>
#include <string>
#include <vector>

class library_gui {
private:
    enum page {
        DASHBOARD,
        BOOKS, BOOK_ADD, BOOK_EDIT, BOOK_DELETE,
        BORROWS, BORROW_CREATE, BORROW_RETURN, BORROW_DELETE,
        REQUESTS, BORROW_FILTER,
        SHIFTS, SHIFT_REGISTER, SHIFT_CANCEL, SHIFT_DELETE,
        SALARY,
        USERS, USER_ADD, USER_EDIT, USER_DELETE,
        SHELF,
        USER_SEARCH, USER_REQUEST, USER_ACTIVE_BORROWS, USER_HISTORY
    };

    library_controller& controller;
    page current_page;
    bool logout_requested;
    int list_page;

    // Lich su cac trang da di qua, dung cho nut "Quay lai".
    std::vector<page> history;

    char field_a[96];
    char field_b[160];
    char field_c[160];
    char field_d[96];
    char field_e[96];
    char field_f[96];
    char field_g[96];
    char* active_field;
    int active_field_capacity;
    std::string message;
    bool message_is_error;
    bool showing_related;

    // Hop thoai xac nhan.
    bool confirm_open;
    bool confirm_just_opened;
    bool input_locked;
    std::string confirm_title;
    std::string confirm_text;
    std::string confirm_ok_label;
    Color confirm_ok_color;
    std::function<void()> confirm_action;

    void collect_text_input();
    void clear_fields();
    void switch_page(page next_page);
    void open_page(page next_page);      // di toi trang con, luu lai trang hien tai
    void open_root_page(page next_page); // chon tu menu ben trai, xoa lich su
    void go_back();                      // quay ve trang truoc do
    void set_message(const std::string& text, bool is_error = false);

    void ask_confirm(const std::string& title, const std::string& text,
                     const std::string& ok_label, Color ok_color,
                     std::function<void()> action);
    void draw_confirm_dialog();

    void draw_shell(const std::string& role);
    void draw_sidebar(const std::string& role);
    void draw_input(Rectangle box, const char* label, char* value, int capacity);
    bool draw_button(Rectangle box, const char* label, Color color = BLUE);
    void draw_message();
    void draw_back_button();

    void draw_dashboard(const std::string& role);
    void draw_books(const std::string& role, bool user_search);
    void draw_book_form(bool editing);
    void draw_book_delete();

    void draw_borrows();
    void draw_borrow_create();
    void draw_borrow_return(bool deleting);
    void draw_requests();
    void draw_borrow_filter();

    void draw_shifts(const std::string& role);
    void draw_shift_form(bool cancelling, bool deleting);
    void draw_salary(const std::string& role);

    void draw_users();
    void draw_user_form(bool editing);
    void draw_user_delete();
    void draw_shelf();

    void draw_user_request();
    void draw_my_borrows(bool active_only);

    bool read_integer(const char* text, int& value, int min_value, int max_value) const;
    bool read_date(const char* text, date& value) const;
    bool is_blank(const char* text) const;

public:
    explicit library_gui(library_controller& controller);

    void draw(const std::string& role);
    void reset();
    bool wants_logout() const;
};