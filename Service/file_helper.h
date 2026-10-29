#pragma once
#include <fstream>
#include <string>
#include "../Structure/linked_list.h"

class file_helper {
public:
    // Đơn nhiệm GHI: Chỉ làm duy nhất 1 việc là ghi danh sách ra file
    template <typename T>
    static bool save_list_to_file(const std::string& filename, const linked_list<T>& list) {
        std::ofstream file(filename, std::ios::trunc);
        if (!file.is_open()) return false;

        node<T>* curr = list.get_head();
        while (curr != nullptr) {
            // Gọi hàm to_record_string() của chính thực thể (Kế thừa từ library_entity)
            file << curr->get_data().to_record_string() << "\n";
            curr = curr->get_next();
        }
        file.close();
        return true;
    }
};