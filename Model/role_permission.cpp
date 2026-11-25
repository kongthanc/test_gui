#include "role_permission.h"

std::string manager_permission::get_role_name() const {
    return "manager";
}

bool manager_permission::can_manage_users() const {
    return true;
}

std::string admin_permission::get_role_name() const {
    return "admin";
}

bool admin_permission::can_manage_users() const {
    return false;
}

std::string reader_permission::get_role_name() const {
    return "user";
}

bool reader_permission::can_manage_users() const {
    return false;
}

const role_permission* permission_factory::create(const std::string& role_str) {
    static const manager_permission manager_instance;
    static const admin_permission admin_instance;
    static const reader_permission reader_instance;

    if (role_str == "manager") return &manager_instance;
    if (role_str == "admin")   return &admin_instance;
    return &reader_instance;
}   