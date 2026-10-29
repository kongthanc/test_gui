
#pragma once
#include <string>

class search_algo {
public:
    // 1. Chuyá»ƒn chuá»—i vá» in thÆ°á»ng (Há»— trá»£ tÃ¬m kiáº¿m khÃ´ng phÃ¢n biá»‡t Hoa/ThÆ°á»ng)
    static std::string to_lower(std::string str) {
        std::string result = str;
        for (int i = 0; i < result.length(); i++) {
            if (result[i] >= 'A' && result[i] <= 'Z') {
                result[i] = result[i] + 32;
            }
        }
        return result;
    }

    // 2. Gá»t bá» khoáº£ng tráº¯ng thá»«a á»Ÿ 2 Ä‘áº§u do ngÆ°á»i dÃ¹ng gÃµ nháº§m
    static std::string trim(std::string str) {
        if (str.empty()) return str;
        
        int start = 0;
        while (start < str.length() && str[start] == ' ') start++;
        
        int end = str.length() - 1;
        while (end >= 0 && str[end] == ' ') end--;
        
        if (start > end) return "";
        return str.substr(start, end - start + 1);
    }

    // 3. Thuáº­t toÃ¡n tÃ¬m chuá»—i con (Tráº£ vá» true náº¿u pattern náº±m trong text)
    static bool contains(std::string text, std::string pattern) {
        text = to_lower(trim(text));
        pattern = to_lower(trim(pattern));

        if (pattern.empty()) return true;
        if (text.length() < pattern.length()) return false;

        // TrÆ°á»£t dá»c theo text Ä‘á»ƒ tÃ¬m pattern
        for (int i = 0; i <= text.length() - pattern.length(); i++) {
            int j;
            for (j = 0; j < pattern.length(); j++) {
                if (text[i + j] != pattern[j]) {
                    break; 
                }
            }
            if (j == pattern.length()) return true; // Khá»›p 100%
        }
        return false;
    }
};
