#include "date.h"

date::date() {
    day = 1;
    month = 1;
    year = 2026;
}

date::date(int day, int month, int year) {
    this->day = day;
    this->month = month;
    this->year = year;

    if (!is_valid()) {
        this->day = 1;
        this->month = 1;
        this->year = 2026;
    }
}

bool date::is_leap_year() const {
    return year % 400 == 0 ||
           (year % 4 == 0 && year % 100 != 0);
}

int date::days_in_month() const {
    if (month == 2)
        return is_leap_year() ? 29 : 28;

    if (month == 4 || month == 6 ||
        month == 9 || month == 11)
        return 30;

    return 31;
}

bool date::is_valid() const {
    if (year < 1)
        return false;

    if (month < 1 || month > 12)
        return false;

    if (day < 1 || day > days_in_month())
        return false;

    return true;
}

void date::input() {
    do {
        std::cout << "nhap ngay thang nam: ";
        std::cin >> day >> month >> year;

        if (!is_valid())
            std::cout << "ngay khong hop le!\n";

    } while (!is_valid());
}

void date::output() const {
    std::cout << day << "/" << month << "/" << year;
}

int date::get_day() const {
    return day;
}

int date::get_month() const {
    return month;
}

int date::get_year() const {
    return year;
}

void date::set_day(int day) {
    int old = this->day;
    this->day = day;

    if (!is_valid())
        this->day = old;
}

void date::set_month(int month) {
    int old = this->month;
    this->month = month;

    if (!is_valid())
        this->month = old;
}

void date::set_year(int year) {
    int old = this->year;
    this->year = year;

    if (!is_valid())
        this->year = old;
}

date& date::operator++() {
    day++;

    if (day > days_in_month()) {
        day = 1;
        month++;

        if (month > 12) {
            month = 1;
            year++;
        }
    }

    return *this;
}

date date::operator++(int) {
    date temp = *this;
    ++(*this);
    return temp;
}

date date::add_days(int days) const {
    date result = *this;

    for (int i = 0; i < days; i++) {
        ++result;
    }

    return result;
}

bool date::operator==(const date& other) const {
    return day == other.day &&
           month == other.month &&
           year == other.year;
}

bool date::operator!=(const date& other) const {
    return !(*this == other);
}

bool date::operator<(const date& other) const {
    if (year != other.year)
        return year < other.year;

    if (month != other.month)
        return month < other.month;

    return day < other.day;
}

bool date::operator>(const date& other) const {
    return other < *this;
}

bool date::operator<=(const date& other) const {
    return !(*this > other);
}

bool date::operator>=(const date& other) const {
    return !(*this < other);
}

int date::operator-(const date& other) const {
    date a = *this;
    date b = other;

    int count = 0;

    if (a == b)
        return 0;

    if (a > b) {
        while (b < a) {
            ++b;
            count++;
        }
    }
    else {
        while (a < b) {
            ++a;
            count++;
        }
    }

    return (a == *this) ? count : -count;
}

std::istream& operator>>(std::istream& in, date& d) {
    do {
        in >> d.day >> d.month >> d.year;

        if (!d.is_valid())
            std::cout << "ngay khong hop le, nhap lai: ";

    } while (!d.is_valid());

    return in;
}

std::ostream& operator<<(std::ostream& out, const date& d) {
    out << d.day << "/" << d.month << "/" << d.year;
    return out;
}
std::string date::get_day_of_week() const {
    int d = this->day;
    int m = this->month;
    int y = this->year;
    
    // Thuật toán yêu cầu tháng 1, 2 phải được tính là tháng 13, 14 của năm trước
    if (m < 3) {
        m += 12;
        y -= 1;
    }
    
    int k = y % 100;
    int j = y / 100;
    
    // Công thức Zeller
    int f = d + ((13 * (m + 1)) / 5) + k + (k / 4) + (j / 4) + (5 * j);
    int day_of_week = f % 7;
    
    // Zeller trả về: 0 = Thứ 7, 1 = Chủ nhật, 2 = Thứ 2...
    if(day_of_week == 0 ) return "thu 7";
        else if(day_of_week == 1 ) return "chu nhat";
             else if(day_of_week == 2 ) return "thu 2";
                  else if(day_of_week == 3 ) return "thu 3";
                        else if(day_of_week == 4 ) return "thu 4";
                            else if(day_of_week == 5 ) return "thu 5";
                                 else if(day_of_week == 6 ) return "thu 6";
    return "";
}