#include "user.h"

using namespace std;

void user::update_permission_strategy() {
    delete permission_strategy;
    permission_strategy = permission_factory::create(this->role);
}

user::user() : library_entity(""), name(""), password(""), role("user"), permission_strategy(nullptr) {
    update_permission_strategy();
}

user::user(string id, string name, string password, string role) 
    : library_entity(id), name(name), password(password), role(role), permission_strategy(nullptr) {
    update_permission_strategy();
}

user::user(const user& other) 
    : library_entity(other.id), name(other.name), password(other.password), role(other.role), permission_strategy(nullptr) {
    update_permission_strategy();
}

user& user::operator=(const user& other) {
    if (this != &other) {
        this->id = other.id;
        this->name = other.name;
        this->password = other.password;
        this->role = other.role;
        update_permission_strategy();
    }
    return *this;
}

user::~user() {
    delete permission_strategy;
}

string user::get_name() const { return name; }
string user::get_password() const { return password; }
string user::get_role() const { return role; }

void user::set_name(string name) { this->name = name; }
void user::set_password(string password) { this->password = password; }
void user::set_role(string role) { 
    this->role = role; 
    update_permission_strategy();
}

void user::display_role_menu() const {
    if (permission_strategy != nullptr) {
        permission_strategy->show_menu(this->name);
    }
}

int user::get_max_menu_option() const {
    return (permission_strategy != nullptr) ? permission_strategy->get_max_menu_choice() : 0;
}

bool user::has_user_management_privilege() const {
    return (permission_strategy != nullptr) ? permission_strategy->can_manage_users() : false;
}

string user::get_entity_type() const {
    return "USER";
}

string user::to_record_string() const {
    return id + ";" + name + ";" + password + ";" + role;
}

istream& operator>>(istream& in, user& u) {
    cout << "Ma nguoi dung: "; getline(in >> ws, u.id);
    cout << "Ho ten: "; getline(in, u.name);
    cout << "Mat khau: "; getline(in, u.password);
    cout << "Vai tro: "; getline(in, u.role);
    u.update_permission_strategy();
    return in;
}

ostream& operator<<(ostream& out, const user& u) {
    out << "Ma nguoi dung: " << u.id << '\n'
        << "Ho ten: " << u.name << '\n'
        << "Mat khau: [an]" << '\n'
        << "Vai tro: " << u.role;
    return out;
}