#pragma once
#include <fstream>
#include <string>
#include <filesystem>
#include <cstdio>
#include "../Structure/linked_list.h"
#include "../Model/date.h"

class file_helper {
public:
    static date parse_date(const std::string& str);
    static std::string sanitize(std::string text);
    static bool ensure_parent_dir(const std::string& filename);

    // Đơn nhiệm GHI: Ghi danh sách ra file
    template <typename T>
    static bool save_list_to_file(const std::string& filename, const linked_list<T>& list) {
        if (!ensure_parent_dir(filename)) return false;

        std::string temp_file = filename + ".tmp";
        std::ofstream file(temp_file, std::ios::trunc);
        if (!file.is_open()) return false;

        node<T>* curr = list.get_head();
        while (curr != nullptr) {
            // Gọi hàm to_record_string() của chính thực thể (Kế thừa từ library_entity)
            file << curr->get_data().to_record_string() << "\n";
            curr = curr->get_next();
        }
        file.flush();
        bool ok = file.good();
        file.close();

        if (!ok) return false;

        try {
            std::filesystem::rename(temp_file, filename);
            return true;
        } catch (...) {
            std::remove(filename.c_str());
            return std::rename(temp_file.c_str(), filename.c_str()) == 0;
        }
    }
};