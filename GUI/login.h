#pragma once
#include "../Controller/library_controller.h"
#include <string>

class login_screen {
private:
    library_controller& controller;
    char id[64];
    char password[128];
    bool focus_id;
    bool focus_password;
    bool show_password;
    bool logged_in;
    std::string message;
    std::string role;

    void handle_input();
    void draw_input_box(int x, int y, int w, int h,
                        const char* label, const char* value,
                        bool focused);
    void try_login();

public:
    login_screen(library_controller& c);
    void draw();
    bool is_logged_in() const;
    std::string get_role() const;   
    void reset();    
};