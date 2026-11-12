
#pragma once
#include <string>
#include "library_entity.h"

// Class Template Action Log dùng chung cho mọi đối tượng T (book, user, borrow...)
template <typename T>
class action_log {
private:
    std::string target_id;    // Mã của đối tượng bị tác động (VD: B001, US02, BR05)
    std::string action_type;  // Loại hành động: "ADD", "DELETE", "UPDATE"
    T backup_data;            // Bản sao lưu dữ liệu trước khi thay đổi

public:
    action_log() : target_id(""), action_type(""), backup_data() {}

    action_log(std::string id, std::string type, const T& data)
        : target_id(id), action_type(type), backup_data(data) {}

    std::string get_target_id() const { return target_id; }
    std::string get_action_type() const { return action_type; }
    T get_backup_data() const { return backup_data; }
};
