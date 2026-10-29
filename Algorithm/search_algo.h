#pragma once
#include <string>

class search_algo {
public:
    // 1. Chuyển chuỗi về in thường (Hỗ trợ tìm kiếm không phân biệt Hoa/Thường)
    static std::string to_lower(std::string str) {
        std::string result = str;
        for (int i = 0; i < result.length(); i++) {
            if (result[i] >= 'A' && result[i] <= 'Z') {
                result[i] = result[i] + 32;
            }
        }
        return result;
    }

    // 2. Gọt bỏ khoảng trắng thừa ở 2 đầu do người dùng gõ nhầm
    static std::string trim(std::string str) {
        if (str.empty()) return str;
        
        int start = 0;
        while (start < str.length() && str[start] == ' ') start++;
        
        int end = str.length() - 1;
        while (end >= 0 && str[end] == ' ') end--;
        
        if (start > end) return "";
        return str.substr(start, end - start + 1);
    }

    // 3. Thuật toán tìm chuỗi con (Trả về true nếu pattern nằm trong text)
    static bool contains(std::string text, std::string pattern) {
        text = to_lower(trim(text));
        pattern = to_lower(trim(pattern));

        if (pattern.empty()) return true;
        if (text.length() < pattern.length()) return false;

        // Trượt dọc theo text để tìm pattern
        for (int i = 0; i <= text.length() - pattern.length(); i++) {
            int j;
            for (j = 0; j < pattern.length(); j++) {
                if (text[i + j] != pattern[j]) {
                    break; 
                }
            }
            if (j == pattern.length()) return true; // Khớp 100%
        }
        return false;
    }
};