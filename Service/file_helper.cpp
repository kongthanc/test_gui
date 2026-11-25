#include "file_helper.h"
#include <sstream>
#include "../Algorithm/search_algo.h"

date file_helper::parse_date(const std::string& str) {
    int d = 1, m = 1, y = 2026;
    char s1 = '/', s2 = '/';
    std::stringstream ss(search_algo::trim(str));
    if (ss >> d >> s1 >> m >> s2 >> y) {
        try {
            return date(d, m, y);
        } catch (...) {}
    }
    return date(1, 1, 2026);
}

std::string file_helper::sanitize(std::string text) {
    for (char& c : text) {
        if (c == ';' || c == '\n' || c == '\r') {
            c = ' ';
        }
    }
    return search_algo::trim(text);
}

bool file_helper::ensure_parent_dir(const std::string& filename) {
    try {
        std::filesystem::path p(filename);
        if (p.has_parent_path() && !p.parent_path().empty()) {
            std::filesystem::create_directories(p.parent_path());
        }
        return true;
    } catch (...) {
        return false;
    }
}