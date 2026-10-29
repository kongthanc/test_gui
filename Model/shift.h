#pragma once
#include <string>
#include "date.h" // Nhúng class date vào để quản lí ngày thực tế

class shift {
private:
    date shift_date;         // Ngày làm việc cụ thể
    std::string session;     // Ca làm ("sang" hoặc "chieu")
    std::string admins[4];   // Tối đa 4 người/ca
    int current_count;       // Số người hiện tại đã đăng kí

public:
    shift();
    shift(date shift_date, std::string session);

    date get_shift_date() const;
    std::string get_session() const;
    int get_current_count() const;
    std::string get_admin_at(int index) const;

    void set_shift_date(date shift_date);
    void set_session(std::string session);

    bool add_admin(std::string admin_id);
    bool remove_admin(std::string admin_id);
    bool has_admin(std::string admin_id) const;

    std::string get_all_admins_string() const; 

    std::string to_record_string() const;
};