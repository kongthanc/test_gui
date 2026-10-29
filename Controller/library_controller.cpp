
#include "library_controller.h"

library_controller::library_controller() 
    : b_service("Data/book.txt"), u_service("Data/user.txt"), 
      br_service("Data/borrow.txt"), s_service("Data/shift.txt") {
    current_logged_in_user = nullptr;
    request_file = "Data/request.txt";
    position_file = "Data/position.txt";
    
    load_requests();
    load_positions(); // Äá»c dá»¯ liá»‡u vá»‹ trÃ­ sÃ¡ch tá»« file
    check_system_overdue(); // Tá»± Ä‘á»™ng cáº­p nháº­t cÃ¡c phiáº¿u quÃ¡ háº¡n theo ngÃ y thá»±c táº¿ cá»§a mÃ¡y
}

library_controller::~library_controller() {
    save_requests();
}

// --- Quáº£n lÃ½ File cho HÃ ng Ä‘á»£i ÄÄƒng kÃ½ sÃ¡ch ---
void library_controller::load_requests() {
    std::ifstream file(request_file);
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string req_id, b_id, u_id, b_date_str, due_date_str, status;
        std::getline(ss, req_id, ';');
        std::getline(ss, b_id, ';');
        std::getline(ss, u_id, ';');
        std::getline(ss, b_date_str, ';');
        std::getline(ss, due_date_str, ';');
        std::getline(ss, status, ';');

        int d1, m1, y1, d2, m2, y2;
        char slash;
        std::stringstream ss_b(b_date_str); ss_b >> d1 >> slash >> m1 >> slash >> y1;
        std::stringstream ss_d(due_date_str); ss_d >> d2 >> slash >> m2 >> slash >> y2;

        borrow req(req_id, b_id, u_id, date(d1, m1, y1), date(d2, m2, y2), status);
        book_request_queue.push(req);
    }
    file.close();
}

void library_controller::save_requests() {
    std::ofstream file(request_file, std::ios::trunc);
    if (!file.is_open()) return;

    queue<borrow> temp_queue;
    while (!book_request_queue.is_empty()) {
        borrow req = book_request_queue.front();
        book_request_queue.pop();
        file << req.get_id() << ";"
             << req.get_book_id() << ";"
             << req.get_user_id() << ";"
             << req.get_borrow_date() << ";"
             << req.get_due_date() << ";"
             << req.get_status() << "\n";
        temp_queue.push(req);
    }
    // KhÃ´i phá»¥c láº¡i hÃ ng Ä‘á»£i trÃªn RAM khÃ´ng lÃ m máº¥t dá»¯ liá»‡u
    while (!temp_queue.is_empty()) {
        book_request_queue.push(temp_queue.front());
        temp_queue.pop();
    }
    file.close();
}

// ================= 1. PHIÃŠN ÄÄ‚NG NHáº¬P =================
bool library_controller::login(std::string id, std::string password) {
    current_logged_in_user = u_service.login(id, password);
    if (current_logged_in_user != nullptr) {
        check_system_overdue(); // Má»—i láº§n Ä‘Äƒng nháº­p tá»± Ä‘á»™ng rÃ  soÃ¡t phiáº¿u quÃ¡ háº¡n
    }
    return (current_logged_in_user != nullptr);
}

void library_controller::logout() {
    current_logged_in_user = nullptr;
}

user* library_controller::get_current_user() const {
    return current_logged_in_user;
}

std::string library_controller::get_current_role() const {
    if (current_logged_in_user == nullptr) return "";
    return current_logged_in_user->get_role();
}

// ================= 2. QUáº¢N LÃ SÃCH =================
linked_list<book> library_controller::search_books(std::string keyword) {
    return b_service.search_books(keyword);
}

linked_list<book> library_controller::get_related_books(std::string category, std::string exclude_book_id) {
    linked_list<book> result;
    linked_list<book> same_cat = b_service.search_books(category);
    node<book>* curr = same_cat.get_head();
    while (curr != nullptr) {
        if (curr->get_data().get_id() != exclude_book_id) {
            result.push_back(curr->get_data());
        }
        curr = curr->get_next();
    }
    return result;
}

bool library_controller::add_new_book(std::string id, std::string title, std::string author, std::string category, int qty, int year) {
    book new_b(id, title, author, category, qty, year);
    return b_service.add_book(new_b);
}

bool library_controller::update_book_info(std::string old_id, book book_n) {
    // Náº¿u Ä‘á»•i mÃ£ sÃ¡ch mÃ  sÃ¡ch cÅ© Ä‘ang cÃ³ ngÆ°á»i mÆ°á»£n -> KhÃ´ng cho Ä‘á»•i Ä‘á»ƒ trÃ¡nh sai lá»‡ch dá»¯ liá»‡u
    if (old_id != book_n.get_id() && br_service.is_book_borrowed(old_id)) {
        return false;
    }
    return b_service.update_book(old_id, book_n);
}

bool library_controller::delete_book(std::string id) {
    // RÃ€NG BUá»˜C TOÃ€N Váº¸N: KhÃ´ng cho xÃ³a sÃ¡ch Ä‘ang cÃ³ ngÆ°á»i mÆ°á»£n hoáº·c quÃ¡ háº¡n chÆ°a tráº£
    if (br_service.is_book_borrowed(id)) {
        return false; 
    }
    return b_service.delete_book(id);
}

node<book>* library_controller::get_book_by_id(std::string id) {
    return b_service.get_book_by_id(id);
}

void library_controller::sort_books(bool (*cmp)(book, book)) {
    b_service.sort_books(cmp);
}

// Lá»c phiáº¿u mÆ°á»£n
linked_list<borrow> library_controller::filter_borrows_by_date(date start_date, date end_date) {
    return br_service.filter_borrows_by_date(start_date, end_date);
}

// ================= 3. QUáº¢N LÃ MÆ¯á»¢N - TRáº¢ =================
bool library_controller::borrow_book(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date) {
    return br_service.create_borrow_ticket(borrow_id, book_id, user_id, borrow_date, due_date, b_service, u_service);
}

bool library_controller::return_book(std::string borrow_id) {
    return br_service.return_book(borrow_id, b_service);
}

bool library_controller::delete_borrow(std::string borrow_id) {
    return br_service.delete_borrow_ticket(borrow_id);
}

node<borrow>* library_controller::get_borrow_by_id(std::string id) {
    return br_service.get_borrow_by_id(id);
}

linked_list<borrow> library_controller::get_my_borrows(std::string user_id, bool active_only) {
    return br_service.get_borrows_by_user(user_id, active_only);
}

void library_controller::check_system_overdue() {
    time_t t = time(0);
    tm* now = localtime(&t);
    if (now == nullptr) return; // Báº£o vá»‡ an toÃ n chá»‘ng crash
    date today(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
    br_service.check_and_update_overdue(today);
}
bool library_controller::has_overdue_borrows(
    std::string user_id) const {
    return br_service.has_overdue_borrows(user_id);
}
// ================= 4. HÃ€NG Äá»¢I ÄÄ‚NG KÃ SÃCH (USER QUEUE) =================
bool library_controller::is_request_id_in_queue(std::string req_id) {
    queue<borrow> temp_queue;
    bool found = false;
    while (!book_request_queue.is_empty()) {
        borrow req = book_request_queue.front();
        book_request_queue.pop();
        if (req.get_id() == req_id) {
            found = true;
        }
        temp_queue.push(req);
    }
    while (!temp_queue.is_empty()) {
        book_request_queue.push(temp_queue.front());
        temp_queue.pop();
    }
    return found;
}

bool library_controller::register_book_request(std::string req_id, std::string book_id, std::string user_id, date b_date, date due_date) {
    // 1. Kiá»ƒm tra sÃ¡ch cÃ³ tá»“n táº¡i khÃ´ng
    node<book>* b_node = b_service.get_book_by_id(book_id);
    if (b_node == nullptr) return false;

    // 2. Kiá»ƒm tra sÃ¡ch cÃ²n tá»“n kho > 0
    if (b_node->get_data().get_quantity() <= 0) return false;

    // 3. Kiá»ƒm tra ngÃ y háº¹n tráº£ há»£p lá»‡
    if (due_date < b_date) return false;

    // 4. Kiá»ƒm tra trÃ¹ng mÃ£ vá»›i phiáº¿u mÆ°á»£n Ä‘Ã£ cÃ³ trong há»‡ thá»‘ng
    if (br_service.get_borrow_by_id(req_id) != nullptr) return false;

    // 5. Kiá»ƒm tra trÃ¹ng mÃ£ trong hÃ ng Ä‘á»£i Ä‘ang chá» duyá»‡t
    if (is_request_id_in_queue(req_id)) return false;

    // 6. Kiá»ƒm tra báº¡n Ä‘á»c nÃ y cÃ³ Ä‘ang mÆ°á»£n chÃ­nh cuá»‘n nÃ y chÆ°a tráº£ khÃ´ng
    if (br_service.is_user_borrowing_book(user_id, book_id)) return false;

    // 7. Kiá»ƒm tra báº¡n Ä‘á»c cÃ³ sÃ¡ch ná»£ quÃ¡ háº¡n chÆ°a tráº£ khÃ´ng
    if (br_service.has_overdue_borrows(user_id)) return false;

    borrow ticket(req_id, book_id, user_id, b_date, due_date, "Cho duyet");
    book_request_queue.push(ticket);

    // LÆ°u ngay xuá»‘ng file request.txt
    save_requests();
    return true;
}

bool library_controller::has_pending_requests() const {
    return !book_request_queue.is_empty();
}

borrow library_controller::peek_next_request() const {
    return book_request_queue.front();
}

bool library_controller::approve_next_request() {
    if (book_request_queue.is_empty()) return false;
    borrow req = book_request_queue.front();

    // Táº¡o phiáº¿u mÆ°á»£n chÃ­nh thá»©c vÃ  trá»« sá»‘ lÆ°á»£ng trong kho
    if (borrow_book(req.get_id(), req.get_book_id(), req.get_user_id(), req.get_borrow_date(), req.get_due_date())) {
        book_request_queue.pop();
        save_requests(); // Cáº­p nháº­t ngay file request.txt
        return true;
    }
    return false; // Háº¿t sÃ¡ch hoáº·c trÃ¹ng mÃ£ phiáº¿u
}

bool library_controller::reject_next_request() {
    if (book_request_queue.is_empty()) return false;
    book_request_queue.pop();
    save_requests(); // Cáº­p nháº­t ngay file request.txt
    return true;
}

int library_controller::get_pending_request_count() const {
    return book_request_queue.get_size();
}

// ================= 5. CA TRá»°C =================
bool library_controller::register_shift(date d, std::string session, std::string admin_id) {
    return s_service.register_shift(d, session, admin_id, u_service);
}

bool library_controller::cancel_shift(date d, std::string session, std::string admin_id) {
    return s_service.cancel_shift(d, session, admin_id);
}

linked_list<shift> library_controller::get_my_shifts(std::string admin_id) {
    return s_service.get_shifts_by_admin(admin_id);
}

long long library_controller::get_admin_salary(std::string admin_id, int month, int year) {
    return s_service.calculate_salary(admin_id, month, year, 200000);
}

bool library_controller::delete_shift(date d, std::string session) {
    return s_service.delete_entire_shift(d, session);
}

void library_controller::show_all_shifts() {
    s_service.display_all();
}
linked_list<shift> library_controller::get_all_shifts() {
    return s_service.get_all_shifts();
}

// ================= 6. QUáº¢N LÃ NGÆ¯á»œI DÃ™NG (USER CRUD - MANAGER) =================
bool library_controller::add_new_user(std::string id, std::string name, std::string password, std::string role) {
    user new_u(id, name, password, role);
    return u_service.add_user(new_u);
}

bool library_controller::update_user_info(std::string old_id, user updated_user) {
    return u_service.update_user(old_id, updated_user);
}

bool library_controller::delete_user(std::string id) {
    // 1. Cháº·n tá»± xÃ³a tÃ i khoáº£n Ä‘ang Ä‘Äƒng nháº­p
    if (current_logged_in_user != nullptr && current_logged_in_user->get_id() == id) {
        return false; 
    }
    // 2. Cháº·n xÃ³a tÃ i khoáº£n Ä‘ang cÃ³ sÃ¡ch mÆ°á»£n chÆ°a tráº£
    if (br_service.has_active_borrows(id)) {
        return false;
    }
    // 3. Cháº·n xÃ³a tÃ i khoáº£n Admin Ä‘ang cÃ³ lá»‹ch trá»±c
    if (s_service.has_shifts(id)) {
        return false;
    }

    return u_service.delete_user(id);
}

node<user>* library_controller::get_user_by_id(std::string id) {
    return u_service.get_user_by_id(id);
}

// --- Äá»c dá»¯ liá»‡u tá»« file position.txt (Äá»‹nh dáº¡ng: b001;it;a1) ---
void library_controller::load_positions() {
    std::ifstream file(position_file);
    if (!file.is_open()) {
        file.open("position.txt");
    }
    if (!file.is_open()) return;

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string b_id, cat, shelf;
        std::getline(ss, b_id, ';');
        std::getline(ss, cat, ';');
        std::getline(ss, shelf, ';');

        b_id = search_algo::to_lower(search_algo::trim(b_id));
        shelf = search_algo::trim(shelf);

        position_list.push_back(book_location(b_id, cat, shelf));
    }
    file.close();
}

// --- TÃ¬m vá»‹ trÃ­ ká»‡ theo mÃ£ sÃ¡ch ---
std::string library_controller::get_book_shelf(std::string book_id) {
    std::string target_id = search_algo::to_lower(search_algo::trim(book_id));
    node<book_location>* curr = position_list.get_head();

    while (curr != nullptr) {
        if (curr->get_data().book_id == target_id) {
            return curr->get_data().shelf;
        }
        curr = curr->get_next();
    }
    return "Chua xep ke";
}
