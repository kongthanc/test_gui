#include "notification.h"

notification::notification()
    : is_read(false) {}

notification::notification(std::string id, std::string user_id,
                           std::string title, std::string content,
                           std::string created_at, bool is_read)
    : id(id), user_id(user_id), title(title),
      content(content), created_at(created_at), is_read(is_read) {}

std::string notification::get_id() const {
    return id;
}

std::string notification::get_user_id() const {
    return user_id;
}

std::string notification::get_title() const {
    return title;
}

std::string notification::get_content() const {
    return content;
}

std::string notification::get_created_at() const {
    return created_at;
}

bool notification::get_is_read() const {
    return is_read;
}

void notification::set_is_read(bool value) {
    is_read = value;
}