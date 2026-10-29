#include "shift.h"

using namespace std;

shift::shift() {
    this->session = "";
    this->current_count = 0;
    for (int i = 0; i < 4; i++) {
        admins[i] = "";
    }
    // shift_date sẽ tự gọi constructor mặc định của class date
}

shift::shift(date shift_date, string session) {
    this->shift_date = shift_date;
    this->session = session;
    this->current_count = 0;
    for (int i = 0; i < 4; i++) {
        admins[i] = "";
    }
}

date shift::get_shift_date() const { return shift_date; }
string shift::get_session() const { return session; }
int shift::get_current_count() const { return current_count; }

string shift::get_admin_at(int index) const {
    if (index >= 0 && index < current_count) return admins[index];
    return "";
}

void shift::set_shift_date(date shift_date) { this->shift_date = shift_date; }
void shift::set_session(string session) { this->session = session; }

bool shift::add_admin(string admin_id) {
    if (has_admin(admin_id)) return false; // Đã đăng kí rồi
    if (current_count >= 4) return false;  // Đã đầy slot

    admins[current_count] = admin_id;
    current_count++;
    return true;
}

bool shift::remove_admin(string admin_id) {
    int pos = -1;
    for (int i = 0; i < current_count; i++) {
        if (admins[i] == admin_id) {
            pos = i;
            break;
        }
    }

    if (pos == -1) return false; // Không tìm thấy admin trong ca này

    // Dồn mảng để lấp chỗ trống
    for (int i = pos; i < current_count - 1; i++) {
        admins[i] = admins[i + 1];
    }
    admins[current_count - 1] = "";
    current_count--;
    return true;
}

bool shift::has_admin(string admin_id) const {
    for (int i = 0; i < current_count; i++) {
        if (admins[i] == admin_id) return true;
    }
    return false;
}

string shift::get_all_admins_string() const {
    string result = "";
    for (int i = 0; i < current_count; i++) {
        result += admins[i];
        if (i < current_count - 1) result += ",";
    }
    return result;
}
#include <sstream> // Đảm bảo có thư viện này ở đầu file hoặc trước hàm

std::string shift::to_record_string() const {
    std::stringstream ss;
    ss << shift_date << ";" << session << ";" << get_all_admins_string();
    return ss.str();
}