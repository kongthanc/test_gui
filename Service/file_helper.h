
#pragma once
#include <fstream>
#include <string>
#include "../Structure/linked_list.h"

class file_helper {
public:
    // ÄÆ¡n nhiá»‡m GHI: Chá»‰ lÃ m duy nháº¥t 1 viá»‡c lÃ  ghi danh sÃ¡ch ra file
    template <typename T>
    static bool save_list_to_file(const std::string& filename, const linked_list<T>& list) {
        std::ofstream file(filename, std::ios::trunc);
        if (!file.is_open()) return false;

        node<T>* curr = list.get_head();
        while (curr != nullptr) {
            // Gá»i hÃ m to_record_string() cá»§a chÃ­nh thá»±c thá»ƒ (Káº¿ thá»«a tá»« library_entity)
            file << curr->get_data().to_record_string() << "\n";
            curr = curr->get_next();
        }
        file.close();
        return true;
    }
};
