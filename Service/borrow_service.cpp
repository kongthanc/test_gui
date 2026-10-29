
#include "borrow_service.h"

borrow_service::borrow_service(std::string filename) {
    data_file = filename;
    load_data();
}

borrow_service::~borrow_service() {
    save_data(); // Tá»± Ä‘á»™ng Ä‘á»“ng bá»™ file khi thoÃ¡t
}

// --- TÃ¡ch dÃ²ng tá»« borrow.txt (Äá»‹nh dáº¡ng: BorrowID;BookID;UserID;BorrowDate;DueDate;Status) ---
void borrow_service::parse_line(std::string line, borrow& b) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    std::stringstream ss(line);
    std::string id, book_id, user_id, b_date_str, due_date_str, status;

    std::getline(ss, id, ';');
    std::getline(ss, book_id, ';');
    std::getline(ss, user_id, ';');
    std::getline(ss, b_date_str, ';');
    std::getline(ss, due_date_str, ';');
    std::getline(ss, status, ';');

    // Parse ngÃ y mÆ°á»£n (VD: 18/09/2026)
    int d1, m1, y1;
    char slash;
    std::stringstream ss_b(b_date_str);
    ss_b >> d1 >> slash >> m1 >> slash >> y1;
    date borrow_date(d1, m1, y1);

    // Parse ngÃ y háº¹n tráº£ (VD: 25/09/2026)
    int d2, m2, y2;
    std::stringstream ss_d(due_date_str);
    ss_d >> d2 >> slash >> m2 >> slash >> y2;
    date due_date(d2, m2, y2);

    b = borrow(id, book_id, user_id, borrow_date, due_date, status);
}

// --- 1. Äá»ŒC Dá»® LIá»†U Tá»ª FILE ---
bool borrow_service::load_data() {
    std::ifstream file(data_file);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        borrow b;
        parse_line(line, b);
        
        borrow_list.push_back(b);
        node<borrow>* new_node = borrow_list.get_tail(); 
        borrow_index.insert(b.get_id(), new_node); // ÄÄƒng kÃ½ báº£ng bÄƒm O(1)
    }
    file.close();
    return true;
}

// --- 2. GHI Dá»® LIá»†U XUá»NG FILE ---
bool borrow_service::save_data() {
    return file_helper::save_list_to_file(data_file, borrow_list);
}

// --- 3. Táº O PHIáº¾U MÆ¯á»¢N SÃCH (DÃ¹ng toÃ¡n tá»­ --) ---
bool borrow_service::create_borrow_ticket(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date, book_service& b_service, user_service& u_service) {
    // Check trÃ¹ng mÃ£ phiáº¿u mÆ°á»£n O(1)
    if (borrow_index.find(borrow_id) != nullptr) return false;

    // Check xem User cÃ³ tá»“n táº¡i khÃ´ng
    if (u_service.get_user_by_id(user_id) == nullptr) return false;

    // Check xem User cÃ³ Ä‘ang ná»£ sÃ¡ch quÃ¡ háº¡n khÃ´ng
    if (has_overdue_borrows(user_id)) return false;

    // GIá»šI Háº N MÆ¯á»¢N Tá»I ÄA 7 CUá»N ÄANG MÆ¯á»¢N
    if (count_active_borrows_by_user(user_id) >= 7) return false;

    // Check xem User cÃ³ Ä‘ang mÆ°á»£n chÃ­nh cuá»‘n sÃ¡ch nÃ y chÆ°a tráº£ khÃ´ng
    if (is_user_borrowing_book(user_id, book_id)) return false;

    // Check xem Book cÃ³ tá»“n táº¡i vÃ  cÃ²n sá»‘ lÆ°á»£ng khÃ´ng
    node<book>* book_node = b_service.get_book_by_id(book_id);
    if (book_node == nullptr) return false;
    if (due_date < borrow_date) return false; // NgÃ y háº¹n tráº£ khÃ´ng Ä‘Æ°á»£c trÆ°á»›c ngÃ y mÆ°á»£n
    
    // Láº¥y tham chiáº¿u trá»±c tiáº¿p Ä‘áº¿n Ä‘á»‘i tÆ°á»£ng book bÃªn trong node kho sÃ¡ch
    book& target_book = book_node->get_Data();
    if (target_book.get_quantity() <= 0) return false; // Háº¿t sÃ¡ch trong kho

    // DÃ™NG TOÃN Tá»¬ -- Äá»‚ TRá»ª KHO TRá»°C TIáº¾P TRÃŠN RAM
    --target_book;

    // Táº¡o phiáº¿u mÆ°á»£n má»›i vá»›i tráº¡ng thÃ¡i ban Ä‘áº§u lÃ  "Dang muon"
    borrow new_ticket(borrow_id, book_id, user_id, borrow_date, due_date, "Dang muon");

    borrow_list.push_back(new_ticket);
    borrow_index.insert(borrow_id, borrow_list.get_tail());

    // Tá»± Ä‘á»™ng lÆ°u ngay xuá»‘ng file Ä‘á»ƒ Ä‘áº£m báº£o an toÃ n dá»¯ liá»‡u
    save_data();
    b_service.save_data();

    return true;
}

// --- 4. TRáº¢ SÃCH (DÃ¹ng toÃ¡n tá»­ ++) ---
bool borrow_service::return_book(std::string borrow_id, book_service& b_service) {
    node<borrow>* ticket_node = borrow_index.find(borrow_id);
    if (ticket_node == nullptr) return false;
    
    // Láº¥y tham chiáº¿u trá»±c tiáº¿p Ä‘áº¿n Ä‘á»‘i tÆ°á»£ng borrow náº±m bÃªn trong node
    borrow& ticket = ticket_node->get_Data();
    
    // Náº¿u sÃ¡ch Ä‘Ã£ tráº£ rá»“i thÃ¬ khÃ´ng xá»­ lÃ½ ná»¯a
    if (ticket.get_status() == "Da tra") return false;

    // Cáº­p nháº­t tráº¡ng thÃ¡i phiáº¿u thÃ nh "Da tra"
    ticket.set_status("Da tra");

    // HoÃ n láº¡i sá»‘ lÆ°á»£ng sÃ¡ch vÃ o kho (+1)
    node<book>* book_node = b_service.get_book_by_id(ticket.get_book_id());
    if (book_node != nullptr) {
        // Láº¥y tham chiáº¿u trá»±c tiáº¿p Ä‘áº¿n Ä‘á»‘i tÆ°á»£ng book trong node kho sÃ¡ch
        book& target_book = book_node->get_Data(); 
        
        // DÃ™NG TOÃN Tá»¬ ++ Äá»‚ HOÃ€N KHO TRá»°C TIáº¾P TRÃŠN RAM
        ++target_book; 
    }

    // Tá»± Ä‘á»™ng lÆ°u ngay xuá»‘ng file
    save_data();
    b_service.save_data();

    return true;
}

// --- 5. Tá»° Äá»˜NG KIá»‚M TRA QUÃ Háº N ---
void borrow_service::check_and_update_overdue(date current_date) {
    bool has_changed = false;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow& ticket = curr->get_Data();

        // Náº¿u phiáº¿u Ä‘ang mÆ°á»£n vÃ  ngÃ y hiá»‡n táº¡i vÆ°á»£t quÃ¡ ngÃ y háº¹n tráº£ (dÃ¹ng toÃ¡n tá»­ > Ä‘Ã£ overload á»Ÿ class date)
        if (ticket.get_status() == "Dang muon" && current_date > ticket.get_due_date()) {
            ticket.set_status("Qua han");
            has_changed = true;
        }
        curr = curr->get_next();
    }
    if (has_changed) {
        save_data();
    }
}

// --- 7. TRA Cá»¨U NHANH THEO ID ---
node<borrow>* borrow_service::get_borrow_by_id(std::string id) {
    return borrow_index.find(id);
}

// Lá»c phiáº¿u mÆ°á»£n theo User
linked_list<borrow> borrow_service::get_borrows_by_user(std::string user_id, bool active_only) {
    linked_list<borrow> result;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id) {
            // active_only = true: chá»‰ láº¥y phiáº¿u chÆ°a tráº£ (Dang muon hoáº·c Qua han)
            // active_only = false: láº¥y toÃ n bá»™ phiáº¿u cá»§a User, ká»ƒ cáº£ Ä‘Ã£ tráº£
            if (!active_only || b.get_status() != "Da tra") {
                result.push_back(b);
            }
        }
        curr = curr->get_next();
    }
    return result;
}

// Lá»c theo khoáº£ng ngÃ y mÆ°á»£n
linked_list<borrow> borrow_service::filter_borrows_by_date(date start_date, date end_date) {
    linked_list<borrow> result;
    node<borrow>* curr = borrow_list.get_head();

    while (curr != nullptr) {
        borrow b = curr->get_data();
        date borrow_date = b.get_borrow_date();

        if (borrow_date >= start_date && borrow_date <= end_date) {
            result.push_back(b);
        }

        curr = curr->get_next();
    }

    return result;
}

// --- XÃ“A PHIáº¾U MÆ¯á»¢N ---
bool borrow_service::delete_borrow_ticket(std::string borrow_id) {
    // 1. TÃ¬m node phiáº¿u mÆ°á»£n qua báº£ng bÄƒm O(1)
    node<borrow>* target = borrow_index.find(borrow_id);
    if (target == nullptr) {
        return false; // KhÃ´ng tÃ¬m tháº¥y mÃ£ phiáº¿u
    }

    // CHá»ˆ CHO PHÃ‰P XÃ“A KHI ÄÃƒ TRáº¢ SÃCH XONG! ("Da tra")
    // Tuyá»‡t Ä‘á»‘i khÃ´ng cho xÃ³a khi "Dang muon" hoáº·c "Qua han" Ä‘á»ƒ trÃ¡nh máº¥t sÃ¡ch
    if (target->get_Data().get_status() != "Da tra") {
        return false; 
    }

    // 2. XÃ³a chá»‰ má»¥c trÃªn Báº£ng bÄƒm trÆ°á»›c
    borrow_index.remove(borrow_id);

    // 3. XÃ³a node khá»i Danh sÃ¡ch liÃªn káº¿t kÃ©p
    borrow_list.remove_node(target);

    // Tá»± Ä‘á»™ng lÆ°u thay Ä‘á»•i
    save_data();

    return true;
}

// --- KIá»‚M TRA RÃ€NG BUá»˜C TOÃ€N Váº¸N ---
bool borrow_service::is_book_borrowed(std::string book_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_book_id() == book_id && b.get_status() != "Da tra") {
            return true; // SÃ¡ch Ä‘ang cÃ³ ngÆ°á»i mÆ°á»£n hoáº·c quÃ¡ háº¡n
        }
        curr = curr->get_next();
    }
    return false;
}

bool borrow_service::has_active_borrows(std::string user_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id && b.get_status() != "Da tra") {
            return true; // NgÆ°á»i dÃ¹ng cÃ²n sÃ¡ch chÆ°a tráº£
        }
        curr = curr->get_next();
    }
    return false;
}

bool borrow_service::is_user_borrowing_book(std::string user_id, std::string book_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id && b.get_book_id() == book_id && b.get_status() != "Da tra") {
            return true; // Äang mÆ°á»£n cuá»‘n nÃ y rá»“i
        }
        curr = curr->get_next();
    }
    return false;
}

bool borrow_service::has_overdue_borrows(std::string user_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id && b.get_status() == "Qua han") {
            return true; // Äang cÃ³ sÃ¡ch quÃ¡ háº¡n chÆ°a tráº£
        }
        curr = curr->get_next();
    }
    return false;
}
int borrow_service::count_active_borrows_by_user(std::string user_id) const {
    int count = 0;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id && b.get_status() != "Da tra") {
            count++;
        }
        curr = curr->get_next();
    }
    return count;
}

int borrow_service::count_borrows_by_book(std::string book_id) const {
    int count = 0;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        if (curr->get_data().get_book_id() == book_id) {
            count++;
        }
        curr = curr->get_next();
    }
    return count;
}
