#include "borrow.h"
#include <sstream>

using namespace std;

borrow::borrow() : library_entity("") {
    this->book_id = "";
    this->user_id = "";
    this->borrow_date = date();
    this->due_date = date();
    this->status = "";
}

borrow::borrow(string id, string book_id, string user_id, date borrow_date, date due_date, string status) 
    : library_entity(id) {
    this->book_id = book_id;
    this->user_id = user_id;
    this->borrow_date = borrow_date;
    this->due_date = due_date;
    this->status = status;
}

string borrow::get_book_id() const { return book_id; }
string borrow::get_user_id() const { return user_id; }
date borrow::get_borrow_date() const { return borrow_date; }
date borrow::get_due_date() const { return due_date; }
string borrow::get_status() const { return status; }

void borrow::set_book_id(string book_id) { this->book_id = book_id; }
void borrow::set_user_id(string user_id) { this->user_id = user_id; }
void borrow::set_borrow_date(date borrow_date) { this->borrow_date = borrow_date; }
void borrow::set_due_date(date due_date) { this->due_date = due_date; }
void borrow::set_status(string status) { this->status = status; }

string borrow::get_entity_type() const {
    return "BORROW";
}

string borrow::to_record_string() const {
    stringstream ss;
    ss << id << ";" << book_id << ";" << user_id << ";" << borrow_date << ";" << due_date << ";" << status;
    return ss.str();
}

istream& operator>>(istream& in, borrow& b) {
    cout << "Ma phieu muon: "; getline(in >> ws, b.id);
    cout << "Ma sach: "; getline(in >> ws, b.book_id);
    cout << "Ma nguoi muon: "; getline(in >> ws, b.user_id);
    cout << "Ngay muon (DD MM YYYY): "; in >> b.borrow_date;
    cout << "Han tra (DD MM YYYY): "; in >> b.due_date;
    cout << "Trang thai: "; getline(in >> ws, b.status);
    return in;
}

ostream& operator<<(ostream& out, const borrow& b) {
    out << "Ma phieu muon: " << b.id << '\n'
        << "Ma sach: " << b.book_id << '\n'
        << "Ma nguoi muon: " << b.user_id << '\n'
        << "Ngay muon: " << b.borrow_date << '\n'
        << "Han tra: " << b.due_date << '\n'
        << "Trang thai: " << b.status;
    return out;
}