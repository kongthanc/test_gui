#pragma once
#include <string>
#include <iostream>
#include "date.h"
#include "library_entity.h"

class borrow : public library_entity {
private:
    std::string book_id;
    std::string user_id;
    date borrow_date;
    date due_date;
    std::string status;

public:
    borrow();
    borrow(std::string id, std::string book_id, std::string user_id, date borrow_date, date due_date, std::string status);

    std::string get_book_id() const;
    std::string get_user_id() const;
    date get_borrow_date() const;
    date get_due_date() const;
    std::string get_status() const;

    void set_book_id(std::string book_id);
    void set_user_id(std::string user_id);
    void set_borrow_date(date borrow_date);
    void set_due_date(date due_date);
    void set_status(std::string status);

    // Override hàm thuần ảo từ library_entity
    std::string get_entity_type() const override;
    std::string to_record_string() const override;

    friend std::istream& operator>>(std::istream& in, borrow& b);
    friend std::ostream& operator<<(std::ostream& out, const borrow& b);
};