#pragma once
#include <string>
#include <vector>
#include "../Model/notification.h"

class notification_service {
private:
    std::vector<notification> notifications;
    std::string data_file;

    void load_data();
    void save_data();
    std::string generate_id() const;
    std::string clean_text(std::string text) const;

public:
    notification_service(std::string filename = "Data/notification.txt");

    void add_notification(std::string user_id,
                          std::string title,
                          std::string content);

    std::vector<notification> get_notifications(std::string user_id) const;
    int get_unread_count(std::string user_id) const;
    bool mark_as_read(std::string notification_id);
};