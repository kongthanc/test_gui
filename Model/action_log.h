
#pragma once
#include <string>
#include "library_entity.h"

// Class Template Action Log dÃ¹ng chung cho má»i Ä‘á»‘i tÆ°á»£ng T (book, user, borrow...)
template <typename T>
class action_log {
private:
    std::string target_id;    // MÃ£ cá»§a Ä‘á»‘i tÆ°á»£ng bá»‹ tÃ¡c Ä‘á»™ng (VD: B001, US02, BR05)
    std::string action_type;  // Loáº¡i hÃ nh Ä‘á»™ng: "ADD", "DELETE", "UPDATE"
    T backup_data;            // Báº£n sao lÆ°u dá»¯ liá»‡u trÆ°á»›c khi thay Ä‘á»•i

public:
    action_log() : target_id(""), action_type(""), backup_data() {}

    action_log(std::string id, std::string type, const T& data)
        : target_id(id), action_type(type), backup_data(data) {}

    std::string get_target_id() const { return target_id; }
    std::string get_action_type() const { return action_type; }
    T get_backup_data() const { return backup_data; }
};
