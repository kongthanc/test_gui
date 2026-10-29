
#include<iostream>
#include "shift_service.h"
#include "../Algorithm/search_algo.h"
#include"../Model/date.h"
shift_service::shift_service(std::string filename) {
    data_file = filename;
    load_data();
}

shift_service::~shift_service() {
    save_data(); // Tá»± Ä‘á»™ng lÆ°u file khi táº¯t chÆ°Æ¡ng trÃ¬nh
}

// --- Táº¡o khÃ³a Ä‘á»‹nh danh cho Hash Table (VD: 01/10/2026_sang) ---
std::string shift_service::generate_key(date d, std::string session) const {
    std::stringstream ss;
    ss << d.get_day() << "/" << d.get_month() << "/" << d.get_year() << "_" << session;
    return ss.str();
}

// --- TÃ¡ch dÃ²ng tá»« shift.txt (Äá»‹nh dáº¡ng: 01/10/2026;sang;AD01,AD02) ---
// --- TÃ¡ch dÃ²ng an toÃ n tá»« shift.txt (Äá»‹nh dáº¡ng: DD/MM/YYYY;ca;AD01,AD02) ---
void shift_service::parse_line(std::string line, shift& s) {
    // 1. Dá»n sáº¡ch kÃ½ tá»± xuá»‘ng dÃ²ng Windows \r\n hoáº·c khoáº£ng tráº¯ng 2 Ä‘áº§u
    line = search_algo::trim(line);
    if (line.empty()) return;

    std::stringstream ss(line);
    std::string date_str, session, admins_str;

    if (!std::getline(ss, date_str, ';')) return;
    if (!std::getline(ss, session, ';')) return;
    std::getline(ss, admins_str, ';');

    date_str = search_algo::trim(date_str);
    session = search_algo::to_lower(search_algo::trim(session));

    // 2. Khá»Ÿi táº¡o giÃ¡ trá»‹ máº·c Ä‘á»‹nh Ä‘á»ƒ trÃ¡nh sá»‘ rÃ¡c tá»« bá»™ nhá»› RAM
    int d = 1, m = 1, y = 2026;
    char slash1 = '/', slash2 = '/';
    std::stringstream ss_d(date_str);
    if (!(ss_d >> d >> slash1 >> m >> slash2 >> y)) {
        d = 1; m = 1; y = 2026;
    }
    date shift_date(d, m, y);

    s = shift(shift_date, session);

    // 3. TÃ¡ch danh sÃ¡ch admin, lá»c bá» hoÃ n toÃ n chuá»—i rá»—ng vÃ  dáº¥u pháº©y thá»«a
    std::stringstream ss_adm(admins_str);
    std::string admin_id;
    while (std::getline(ss_adm, admin_id, ',')) {
        admin_id = search_algo::trim(admin_id);
        if (!admin_id.empty() && s.get_current_count() < 4) {
            s.add_admin(admin_id); // Äáº£m báº£o chá»‰ nháº­n mÃ£ admin chuáº©n, tá»‘i Ä‘a 4 ngÆ°á»i
        }
    }
}

// --- 1. Äá»ŒC Dá»® LIá»†U Tá»ª FILE ---
bool shift_service::load_data() {
    std::ifstream file(data_file);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        shift s;
        parse_line(line, s);
        
        shift_list.push_back(s);
        node<shift>* new_node = shift_list.get_tail(); 
        
        // ÄÄƒng kÃ½ vÃ o báº£ng bÄƒm vá»›i khÃ³a ngÃ y_ca
        std::string key = generate_key(s.get_shift_date(), s.get_session());
        shift_index.insert(key, new_node);
    }
    file.close();
    return true;
}

// --- 2. GHI Dá»® LIá»†U XUá»NG FILE ---
bool shift_service::save_data() {
    return file_helper::save_list_to_file(data_file, shift_list);
}
// --- 3. ÄÄ‚NG KÃ CA TRá»°C CHO ADMIN ---
bool shift_service::register_shift(date shift_date, std::string session, std::string admin_id, user_service& u_service) {
    // Kiá»ƒm tra xem user cÃ³ tá»“n táº¡i vÃ  cÃ³ pháº£i lÃ  admin/manager khÃ´ng
    node<user>* user_node = u_service.get_user_by_id(admin_id);
    if (user_node == nullptr) return false;
    
    std::string role = user_node->get_data().get_role();
    if (role != "admin" && role != "manager") return false;

    std::string key = generate_key(shift_date, session);
    node<shift>* shift_node = shift_index.find(key);

    if (shift_node == nullptr) {
        // Náº¿u ca trá»±c chÆ°a tá»“n táº¡i trong ngÃ y hÃ´m Ä‘Ã³ -> Táº¡o má»›i
        shift new_shift(shift_date, session);
        new_shift.add_admin(admin_id);

        shift_list.push_back(new_shift);
        shift_index.insert(key, shift_list.get_tail());

        save_data(); // LÆ°u ngay xuá»‘ng file
        return true;
    } else {
        // Náº¿u ca Ä‘Ã£ tá»“n táº¡i -> Láº¥y ra cáº­p nháº­t
        shift target_shift = shift_node->get_data();
        
        if (!target_shift.add_admin(admin_id)) {
            return false; // Tháº¥t báº¡i (do Ä‘áº§y 4 ngÆ°á»i hoáº·c Ä‘Ã£ Ä‘Äƒng kÃ½ rá»“i)
        }

        shift_node->set_data(target_shift);
        save_data(); // LÆ°u ngay xuá»‘ng file
        return true;
    }
}

// --- 4. Há»¦Y ÄÄ‚NG KÃ CA TRá»°C ---
bool shift_service::cancel_shift(date shift_date, std::string session, std::string admin_id) {
    std::string key = generate_key(shift_date, session);
    node<shift>* shift_node = shift_index.find(key);
    if (shift_node == nullptr) return false;

    shift target_shift = shift_node->get_data();
    if (!target_shift.remove_admin(admin_id)) {
        return false; // KhÃ´ng tÃ¬m tháº¥y admin nÃ y trong ca
    }

    // Náº¿u ca trá»±c trá»‘ng sáº¡ch bÃ³ng admin -> XÃ³a luÃ´n ca trá»±c Ä‘Ã³ khá»i danh sÃ¡ch
    if (target_shift.get_current_count() == 0) {
        shift_index.remove(key);
        shift_list.remove_node(shift_node);
    } else {
        shift_node->set_data(target_shift);
    }

    save_data(); // LÆ°u ngay xuá»‘ng file
    return true;
}

// --- 5. TRA Cá»¨U CA TRá»°C ---
node<shift>* shift_service::get_shift(date shift_date, std::string session) {
    std::string key = generate_key(shift_date, session);
    return shift_index.find(key);
}

// --- 6. XÃ“A TOÃ€N Bá»˜ CA TRá»°C (DÃ nh cho Manager há»§y lá»‹ch trá»±c ngÃ y lá»…/Ä‘á»™t xuáº¥t) ---
bool shift_service::delete_entire_shift(date shift_date, std::string session) {
    std::string key = generate_key(shift_date, session);
    node<shift>* target = shift_index.find(key);
    
    if (target == nullptr) return false; // KhÃ´ng tá»“n táº¡i ca trá»±c nÃ y

    // XÃ³a khá»i báº£ng bÄƒm vÃ  danh sÃ¡ch liÃªn káº¿t
    shift_index.remove(key);
    shift_list.remove_node(target);

    save_data(); // LÆ°u ngay xuá»‘ng file
    return true;
}

// --- 7. Äáº¾M Sá» BUá»”I TRá»°C Cá»¦A ADMIN (Há»— trá»£ lá»c theo thÃ¡ng/nÄƒm, máº·c Ä‘á»‹nh 0 lÃ  tÃ­nh táº¥t cáº£) ---
int shift_service::count_shifts_by_admin(std::string admin_id, int month, int year) const {
    int count = 0;
    node<shift>* curr = shift_list.get_head();

    while (curr != nullptr) {
        shift s = curr->get_data();
        date d = s.get_shift_date();

        // Kiá»ƒm tra khá»›p thÃ¡ng vÃ  nÄƒm (náº¿u ngÆ°á»i dÃ¹ng truyá»n vÃ o > 0)
        bool match_time = true;
        if (month > 0 && d.get_month() != month) match_time = false;
        if (year > 0 && d.get_year() != year) match_time = false;

        // Náº¿u khá»›p thá»i gian vÃ  cÃ³ máº·t admin trong ca Ä‘Ã³ (dÃ¹ng hÃ m has_admin cá»§a class shift)
        if (match_time && s.has_admin(admin_id)) {
            count++;
        }
        curr = curr->get_next();
    }
    return count;
}

// --- 8. TÃNH LÆ¯Æ NG CA TRá»°C (Máº·c Ä‘á»‹nh: 200.000 VNÄ / 1 buá»•i) ---
long long shift_service::calculate_salary(std::string admin_id, int month, int year, long long rate_per_shift) const {
    int total_shifts = count_shifts_by_admin(admin_id, month, year);
    return total_shifts * rate_per_shift;
}

// --- 9. Láº¤Y DANH SÃCH Lá»ŠCH TRá»°C Cá»¦A RIÃŠNG 1 ADMIN ---
linked_list<shift> shift_service::get_shifts_by_admin(std::string admin_id) const {
    linked_list<shift> result;
    node<shift>* curr = shift_list.get_head();

    while (curr != nullptr) {
        shift s = curr->get_data();
        if (s.has_admin(admin_id)) {
            result.push_back(s);
        }
        curr = curr->get_next();
    }
    return result;
}

// --- 10. KIá»‚M TRA ADMIN CÃ“ Lá»ŠCH TRá»°C KHÃ”NG ---
bool shift_service::has_shifts(std::string admin_id) const {
    node<shift>* curr = shift_list.get_head();
    while (curr != nullptr) {
        if (curr->get_data().has_admin(admin_id)) {
            return true;
        }
        curr = curr->get_next();
    }
    return false;
}

// --- 11. HIá»‚N THá»Š TOÃ€N Bá»˜ Lá»ŠCH TRá»°C ---
void shift_service::display_all() {
    node<shift>* curr = shift_list.get_head();
    if (curr == nullptr) {
        std::cout << "Hien chua co lich truc nao!\n";
        return;
    }

    std::cout << "\n================ DANH SACH CA TRUC ================\n";
    while (curr != nullptr) {
        shift s = curr->get_data();
        date d = s.get_shift_date();
        std::cout << "- Ngay: " << d << " (" << d.get_day_of_week() << ")"
                  << " | Ca: " << s.get_session()
                  << " | So luong: " << s.get_current_count() << "/4"
                  << " | Admin: " << s.get_all_admins_string() << "\n";
        curr = curr->get_next();
    }
    std::cout << "===================================================\n";
}

linked_list<shift> shift_service::get_all_shifts() const {
    return shift_list;
}
