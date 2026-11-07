#include "notification_service.h"
#include <fstream>
#include <sstream>
#include <ctime>
#include <iomanip>

notification_service::notification_service(std::string filename) {
    data_file = filename;
    load_data();
}

std::string notification_service::clean_text(std::string text) const {
    for (char& c : text) {
        if (c == ';' || c == '\n' || c == '\r') {
            c = ' ';
        }
    }
    return text;
}

void notification_service::load_data() {
    std::ifstream file(data_file);
    if (!file.is_open()) return;

    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string id, user_id, title, content, created_at, read_text;

        std::getline(ss, id, ';');
        std::getline(ss, user_id, ';');
        std::getline(ss, title, ';');
        std::getline(ss, content, ';');
        std::getline(ss, created_at, ';');
        std::getline(ss, read_text, ';');

        notifications.emplace_back(
            id, user_id, title, content, created_at,
            read_text == "1"
        );
    }

    file.close();
}

void notification_service::save_data() {
    std::ofstream file(data_file, std::ios::trunc);
    if (!file.is_open()) return;

    for (const notification& n : notifications) {
        file << n.get_id() << ";"
             << n.get_user_id() << ";"
             << clean_text(n.get_title()) << ";"
             << clean_text(n.get_content()) << ";"
             << n.get_created_at() << ";"
             << (n.get_is_read() ? "1" : "0") << "\n";
    }

    file.close();
}

std::string notification_service::generate_id() const {
    int max_id = 0;

    for (const notification& n : notifications) {
        std::string id = n.get_id();

        if (id.rfind("NTF", 0) == 0) {
            try {
                int number = std::stoi(id.substr(3));
                if (number > max_id) max_id = number;
            } catch (...) {}
        }
    }

    return "NTF" + std::to_string(max_id + 1);
}

void notification_service::add_notification(
    std::string user_id, std::string title, std::string content) {

    std::time_t now = std::time(nullptr);
    std::tm local_time{};

#ifdef _WIN32
    localtime_s(&local_time, &now);
#else
    localtime_r(&now, &local_time);
#endif

    std::ostringstream time_text;
    time_text << std::put_time(&local_time, "%d/%m/%Y %H:%M");

    notifications.emplace_back(
        generate_id(),
        user_id,
        clean_text(title),
        clean_text(content),
        time_text.str(),
        false
    );

    save_data();
}

std::vector<notification> notification_service::get_notifications(
    std::string user_id) const {

    std::vector<notification> result;

    for (const notification& n : notifications) {
        if (n.get_user_id() == user_id) {
            result.push_back(n);
        }
    }

    return result;
}

int notification_service::get_unread_count(std::string user_id) const {
    int count = 0;

    for (const notification& n : notifications) {
        if (n.get_user_id() == user_id && !n.get_is_read()) {
            ++count;
        }
    }

    return count;
}

bool notification_service::mark_as_read(std::string notification_id) {
    for (notification& n : notifications) {
        if (n.get_id() == notification_id) {
            n.set_is_read(true);
            save_data();
            return true;
        }
    }

    return false;
}