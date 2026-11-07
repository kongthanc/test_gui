#pragma once
#include <string>

class notification {
private:
    std::string id;
    std::string user_id;
    std::string title;
    std::string content;
    std::string created_at;
    bool is_read;

public:
    notification();
    notification(std::string id, std::string user_id,
                 std::string title, std::string content,
                 std::string created_at, bool is_read = false);

    std::string get_id() const;
    std::string get_user_id() const;
    std::string get_title() const;
    std::string get_content() const;
    std::string get_created_at() const;
    bool get_is_read() const;

    void set_is_read(bool value);
};