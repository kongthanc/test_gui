#pragma once
#include <string>
#include "library_entity.h"

// Class Template Action Log dùng chung cho mọi đối tượng T (book, user, borrow...)
template <typename T>
class action_log {
private:
    std::string target_id;    // Mã hiện tại của đối tượng bị tác động
    std::string action_type;  // Loại hành động: "ADD", "DELETE", "UPDATE"
    T backup_data;            // Bản sao lưu dữ liệu trước khi thay đổi
    std::string extra_info;   // Lưu thông tin phụ (VD: vị trí kệ sách cũ)

public:
    action_log() : target_id(""), action_type(""), backup_data(), extra_info("") {}

    action_log(std::string id, std::string type, const T& data, std::string extra = "")
        : target_id(id), action_type(type), backup_data(data), extra_info(extra) {}

    std::string get_target_id() const { return target_id; }
    std::string get_action_type() const { return action_type; }
    T get_backup_data() const { return backup_data; }
    std::string get_extra_info() const { return extra_info; }
};