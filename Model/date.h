
#pragma once

#include <iostream>

class date {
private:
    int day;
    int month;
    int year;

    bool is_leap_year() const;
    int days_in_month() const;

public:
    date();
    date(int day, int month, int year);

    bool is_valid() const;

    void input();
    void output() const;

    int get_day() const;
    int get_month() const;
    int get_year() const;

    void set_day(int day);
    void set_month(int month);
    void set_year(int year);

    date& operator++();
    date operator++(int);
    date add_days(int days) const;

    bool operator==(const date& other) const;
    bool operator!=(const date& other) const;
    bool operator<(const date& other) const;
    bool operator>(const date& other) const;
    bool operator<=(const date& other) const;
    bool operator>=(const date& other) const;

    int operator-(const date& other) const;

    friend std::istream& operator>>(std::istream& in, date& d);
    friend std::ostream& operator<<(std::ostream& out, const date& d);
    std::string get_day_of_week() const; // HÃ m tá»± Ä‘á»™ng tÃ­nh ra thá»©
};
