
#pragma once
#include <string>
#include <iostream>
#include "library_entity.h"

class book : public library_entity {
private:
    std::string title;
    std::string author;
    std::string category;
    int quantity;
    int year;

public:
    book();
    book(std::string id, std::string title, std::string author, std::string category, int quantity, int year);

    std::string get_title() const;
    std::string get_author() const;
    std::string get_category() const;
    int get_quantity() const;
    int get_year() const;

    void set_title(std::string title);
    void set_author(std::string author);
    void set_category(std::string category);
    void set_quantity(int quantity);
    void set_year(int year);

    book& operator++();
    book operator++(int);
    book& operator--();
    book operator--(int);

    // Override hÃ m thuáº§n áº£o tá»« library_entity
    std::string get_entity_type() const override;
    std::string to_record_string() const override;

    friend std::istream& operator>>(std::istream& in, book& b);
    friend std::ostream& operator<<(std::ostream& out, const book& b);

    bool operator==(const book& other) const;
};
