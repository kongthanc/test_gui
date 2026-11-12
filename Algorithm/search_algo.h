
#pragma once
#include <string>

class search_algo {
public:
    static std::string to_lower(std::string str) {
        std::string result;
        result.reserve(str.size());
        for (size_t i = 0; i < str.size();) {
            unsigned char c = (unsigned char)str[i];
            unsigned int cp = 0;
            size_t bytes = 1;
            if (c < 0x80) {
                cp = c;
            } else if ((c & 0xE0) == 0xC0 && i + 1 < str.size()) {
                cp = ((c & 0x1F) << 6) |
                     ((unsigned char)str[i + 1] & 0x3F);
                bytes = 2;
            } else if ((c & 0xF0) == 0xE0 && i + 2 < str.size()) {
                cp = ((c & 0x0F) << 12) |
                     (((unsigned char)str[i + 1] & 0x3F) << 6) |
                     ((unsigned char)str[i + 2] & 0x3F);
                bytes = 3;
            } else if ((c & 0xF8) == 0xF0 && i + 3 < str.size()) {
                cp = ((c & 0x07) << 18) |
                     (((unsigned char)str[i + 1] & 0x3F) << 12) |
                     (((unsigned char)str[i + 2] & 0x3F) << 6) |
                     ((unsigned char)str[i + 3] & 0x3F);
                bytes = 4;
            } else {
                result += str[i++];
                continue;
            }

            if (cp >= 'A' && cp <= 'Z') cp += 32;
            else if ((cp >= 0x00C0 && cp <= 0x00D6) ||
                     (cp >= 0x00D8 && cp <= 0x00DE)) cp += 32;
            else if ((cp >= 0x0100 && cp <= 0x012F && cp % 2 == 0) ||
                     (cp >= 0x0132 && cp <= 0x0137 && cp % 2 == 0) ||
                     (cp >= 0x014A && cp <= 0x0177 && cp % 2 == 0) ||
                     (cp >= 0x1E00 && cp <= 0x1E95 && cp % 2 == 0) ||
                     (cp >= 0x1EA0 && cp <= 0x1EF8 && cp % 2 == 0)) cp += 1;
            else if (cp >= 0x0139 && cp <= 0x0148 && cp % 2 == 1) cp += 1;
            else if (cp == 0x0130) cp = 0x0069;
            else if (cp == 0x0178) cp = 0x00FF;
            else if (cp == 0x01A0) cp = 0x01A1;
            else if (cp == 0x01AF) cp = 0x01B0;

            if (cp < 0x80) {
                result += (char)cp;
            } else if (cp < 0x800) {
                result += (char)(0xC0 | (cp >> 6));
                result += (char)(0x80 | (cp & 0x3F));
            } else if (cp < 0x10000) {
                result += (char)(0xE0 | (cp >> 12));
                result += (char)(0x80 | ((cp >> 6) & 0x3F));
                result += (char)(0x80 | (cp & 0x3F));
            } else {
                result += (char)(0xF0 | (cp >> 18));
                result += (char)(0x80 | ((cp >> 12) & 0x3F));
                result += (char)(0x80 | ((cp >> 6) & 0x3F));
                result += (char)(0x80 | (cp & 0x3F));
            }
            i += bytes;
        }
        return result;
    }

    static std::string trim(std::string str) {
        if (str.empty()) return str;
        
        int start = 0;
        while (start < str.length() && str[start] == ' ') start++;
        
        int end = str.length() - 1;
        while (end >= 0 && str[end] == ' ') end--;
        
        if (start > end) return "";
        return str.substr(start, end - start + 1);
    }

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
