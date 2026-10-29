
#pragma once
#include <string>
#include "date.h" // NhÃºng class date vÃ o Ä‘á»ƒ quáº£n lÃ­ ngÃ y thá»±c táº¿

class shift {
private:
    date shift_date;         // NgÃ y lÃ m viá»‡c cá»¥ thá»ƒ
    std::string session;     // Ca lÃ m ("sang" hoáº·c "chieu")
    std::string admins[4];   // Tá»‘i Ä‘a 4 ngÆ°á»i/ca
    int current_count;       // Sá»‘ ngÆ°á»i hiá»‡n táº¡i Ä‘Ã£ Ä‘Äƒng kÃ­

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
