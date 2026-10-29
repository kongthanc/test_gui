
#include "book.h"

using namespace std;

book::book() : library_entity("") {
    this->title = "";
    this->author = "";
    this->category = "";
    this->quantity = 0;
    this->year = 0;
}

book::book(string id, string title, string author, string category, int quantity, int year) 
    : library_entity(id) {
    this->title = title;
    this->author = author;
    this->category = category;
    this->quantity = quantity;
    this->year = year;
}

string book::get_title() const { return title; }
string book::get_author() const { return author; }
string book::get_category() const { return category; }
int book::get_quantity() const { return quantity; }
int book::get_year() const { return year; }

void book::set_title(string title) { this->title = title; }
void book::set_author(string author) { this->author = author; }
void book::set_category(string category) { this->category = category; }
void book::set_quantity(int quantity) {
    if (quantity >= 0) {
        this->quantity = quantity;
    }
}
void book::set_year(int year) { this->year = year; }

book& book::operator++() {
    quantity++;
    return *this;
}

book book::operator++(int) {
    book temp = *this;
    quantity++;
    return temp;
}

book& book::operator--() {
    if (quantity > 0) {
        quantity--;
    }
    return *this;
}

book book::operator--(int) {
    book temp = *this;
    if (quantity > 0) {
        quantity--;
    }
    return temp;
}

string book::get_entity_type() const {
    return "BOOK";
}
    
string book::to_record_string() const {
    return id + ";" + title + ";" + author + ";" + category + ";" + to_string(quantity) + ";" + to_string(year);
}

istream& operator>>(istream& in, book& b) {
    cout << "Ma sach: "; getline(in >> ws, b.id);
    cout << "Ten sach: "; getline(in, b.title);
    cout << "Tac gia: "; getline(in, b.author);
    cout << "The loai: "; getline(in, b.category);
    cout << "So luong: "; in >> b.quantity;
    while (in && b.quantity < 0) { cout << "So luong phai >= 0, nhap lai: "; in >> b.quantity; }
    cout << "Nam xuat ban: "; in >> b.year;
    return in;
}

ostream& operator<<(ostream& out, const book& b) {
    out << "Ma sach: " << b.id << '\n'
        << "Ten sach: " << b.title << '\n'
        << "Tac gia: " << b.author << '\n'
        << "The loai: " << b.category << '\n'
        << "So luong: " << b.quantity << '\n'
        << "Nam xuat ban: " << b.year;
    return out;
}

bool book::operator==(const book& other) const {
    return this->id == other.id;
}
