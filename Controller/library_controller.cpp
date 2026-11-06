#include "library_controller.h"

library_controller::library_controller() 
    : b_service("Data/book.txt"), u_service("Data/user.txt"), 
      br_service("Data/borrow.txt"), s_service("Data/shift.txt") {
    current_logged_in_user = nullptr;
    request_file = "Data/request.txt";
    position_file = "Data/position.txt";
    
    load_requests();
    load_positions(); 
    check_system_overdue(); 
}

library_controller::~library_controller() {
    save_requests();
}

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

    int size = book_request_queue.get_size();
    for (int i = 0; i < size; i++) {
        borrow req = book_request_queue.front();
        book_request_queue.pop();
        
        file << req.get_id() << ";"
             << req.get_book_id() << ";"
             << req.get_user_id() << ";"
             << req.get_borrow_date() << ";"
             << req.get_due_date() << ";"
             << req.get_status() << "\n";
             
        book_request_queue.push(req); // Đẩy lại xuống cuối hàng, xoay vòng đúng 1 lượt
    }
    file.close();
}

bool library_controller::login(std::string id, std::string password) {
    current_logged_in_user = u_service.login(id, password);
    if (current_logged_in_user != nullptr) {
        check_system_overdue(); 
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

linked_list<book> library_controller::search_books(std::string keyword) {
    return b_service.search_books(keyword);
}

// LƯU KỆ SÁCH KHI THÊM SÁCH
bool library_controller::add_new_book(std::string id, std::string title, std::string author, std::string category, int qty, int year, std::string shelf) {
    book new_b(id, title, author, category, qty, year);
    if (b_service.add_book(new_b)) {
        book_undo_stack.push(action_log<book>(id, "ADD", new_b));
        
        std::string lower_id = search_algo::to_lower(search_algo::trim(id));
        position_list.push_back(book_location(lower_id, category, shelf));
        save_positions();
        return true;
    }
    return false;
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

linked_list<borrow> library_controller::filter_borrows_by_date(date start_date, date end_date) {
    return br_service.filter_borrows_by_date(start_date, end_date);
}

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
    if (now == nullptr) return; 
    date today(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
    br_service.check_and_update_overdue(today);
}
bool library_controller::has_overdue_borrows(std::string user_id) const {
    return br_service.has_overdue_borrows(user_id);
}

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

// HÀM MỚI: Sinh mã REQ tự động
std::string library_controller::generate_request_id() {
    // Biến tĩnh (static) sẽ giữ giá trị trong suốt quá trình app chạy
    static int counter = 1; 
    
    time_t now = time(0);
    
    // Ghép thời gian (giây) với bộ đếm (counter)
    // Ví dụ: REQ1732549200_1, REQ1732549200_2
    std::string unique_id = "REQ" + std::to_string(now) + "_" + std::to_string(counter);
    
    counter++; // Tăng bộ đếm cho lần bấm tiếp theo
    
    return unique_id;
}

bool library_controller::register_book_request(std::string req_id, std::string book_id, std::string user_id, date b_date, date due_date) {
    node<book>* b_node = b_service.get_book_by_id(book_id);
    if (b_node == nullptr) return false;
    if (b_node->get_data().get_quantity() <= 0) return false;
    if (due_date < b_date) return false;
    if (br_service.get_borrow_by_id(req_id) != nullptr) return false;
    if (is_request_id_in_queue(req_id)) return false;
    if (br_service.is_user_borrowing_book(user_id, book_id)) return false;
    if (br_service.has_overdue_borrows(user_id)) return false;
    if (br_service.count_active_borrows_by_user(user_id) >= 7) return false; // Chặn mượn > 7 cuốn

    borrow ticket(req_id, book_id, user_id, b_date, due_date, "Cho duyet");
    book_request_queue.push(ticket);
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

    if (borrow_book(req.get_id(), req.get_book_id(), req.get_user_id(), req.get_borrow_date(), req.get_due_date())) {
        book_request_queue.pop();
        save_requests();
        return true;
    }
    return false; 
}

bool library_controller::reject_next_request() {
    if (book_request_queue.is_empty()) return false;
    // 1. Lấy thông tin phiếu đang đứng đầu hàng đợi
    borrow req = book_request_queue.front(); 
    
    // 2. Loại bỏ khỏi hàng đợi chờ duyệt
    book_request_queue.pop();
    save_requests(); 
    
    // 3. Đẩy sang Lịch sử mượn với trạng thái "Tu choi"
    br_service.log_rejected_request(req);
    return true;
}

int library_controller::get_pending_request_count() const {
    return book_request_queue.get_size();
}

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

bool library_controller::add_new_user(std::string id, std::string name, std::string password, std::string role) {
    user new_u(id, name, password, role);
    return u_service.add_user(new_u);
}

bool library_controller::update_user_info(std::string old_id, user updated_user) {
    return u_service.update_user(old_id, updated_user);
}

bool library_controller::delete_user(std::string id) {
    if (current_logged_in_user != nullptr && current_logged_in_user->get_id() == id) {
        return false; 
    }
    if (br_service.has_active_borrows(id)) {
        return false;
    }
    if (s_service.has_shifts(id)) {
        return false;
    }
    return u_service.delete_user(id);
}

node<user>* library_controller::get_user_by_id(std::string id) {
    return u_service.get_user_by_id(id);
}

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

void library_controller::save_positions() {
    std::ofstream file(position_file, std::ios::trunc);
    if (!file.is_open()) return;
    node<book_location>* curr = position_list.get_head();
    while (curr != nullptr) {
        file << curr->get_data().book_id << ";" << curr->get_data().category << ";" << curr->get_data().shelf << "\n";
        curr = curr->get_next();
    }
    file.close();
}

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

bool library_controller::has_active_borrows(std::string user_id) const {
    return br_service.has_active_borrows(user_id);
}

int library_controller::get_user_active_borrow_count(std::string user_id) const {
    return br_service.count_active_borrows_by_user(user_id);
}

// THUẬT TOÁN ĐỒ THỊ (GRAPH BFS) - GỢI Ý SÁCH
// ================= TÍNH NĂNG GỢI Ý SÁCH (DÙNG ĐỒ THỊ GRAPH BFS) =================
linked_list<book> library_controller::get_recommended_books(std::string author, std::string category, std::string exclude_book_id) {
    linked_list<book> all = b_service.search_books("");
    node<book>* target = b_service.get_book_by_id(exclude_book_id);
    
    if (target == nullptr || all.get_size() <= 1) return linked_list<book>();

    int cap = all.get_size() + 2; 
    graph<book> g(cap > 100 ? cap : 100); 

    node<book>* curr = all.get_head();
    while (curr != nullptr) {
        g.add_vertex(curr->get_data());
        curr = curr->get_next();
    }

    // Nối các đỉnh nếu có chung Thể loại hoặc chung Tác giả (Lấy theo 2 tham số GUI truyền xuống)
    for (int i = 0; i < g.get_num_vertices(); ++i) {
        book b1 = g.get_vertex_data(i);
        for (int j = i + 1; j < g.get_num_vertices(); ++j) {
            book b2 = g.get_vertex_data(j);
            if (search_algo::contains(b1.get_category(), b2.get_category()) ||
                search_algo::contains(b1.get_author(), b2.get_author())) {
                g.add_edge(i, j, false);
            }
        }
    }

    // Chạy BFS 
    linked_list<book> bfs_list = graph_algo::BFS(g, target->get_data());
    linked_list<book> result;
    node<book>* r_curr = bfs_list.get_head();
    
    // Lọc bỏ chính cuốn sách đang được chọn
    while (r_curr != nullptr) {
        if (!(r_curr->get_data() == target->get_data())) { 
            result.push_back(r_curr->get_data());
        }
        r_curr = r_curr->get_next();
    }
    
    return result;
}

std::string library_controller::get_hot_category() {
    linked_list<book> all = b_service.search_books("");
    if (all.empty()) return "Giao trinh";

    std::string best_cat = all.get_head()->get_data().get_category();
    int max_cnt = -1;
    for (node<book>* p = all.get_head(); p != nullptr; p = p->get_next()) {
        int cnt = br_service.count_borrows_by_book(p->get_data().get_id());
        if (cnt > max_cnt) {
            max_cnt = cnt;
            best_cat = p->get_data().get_category();
        }
    }
    return best_cat;
}

// HOÀN TÁC (UNDO)
bool library_controller::can_undo_book() const { return !book_undo_stack.empty(); }

bool library_controller::undo_last_book_action(std::string& out_message) {
    if (book_undo_stack.empty()) {
        out_message = "Khong co thao tac sach nao trong Stack de hoan tac!";
        return false;
    }
    action_log<book> last = book_undo_stack.peek();
    book_undo_stack.pop();

    bool success = false;
    if (last.get_action_type() == "ADD") {
        success = b_service.delete_book(last.get_target_id());
        out_message = "Da hoan tac: Xoa sach vua them [" + last.get_target_id() + "]";
    } else if (last.get_action_type() == "DELETE") {
        success = b_service.add_book(last.get_backup_data());
        // Phục hồi vị trí mặc định để không bị trắng cột Kệ Sách
        std::string lower_id = search_algo::to_lower(search_algo::trim(last.get_target_id()));
        position_list.push_back(book_location(lower_id, last.get_backup_data().get_category(), "Da khoi phuc"));
        save_positions();
        out_message = "Da hoan tac: Khoi phuc sach vua xoa [" + last.get_target_id() + "]";
    } else if (last.get_action_type() == "UPDATE") {
        success = b_service.update_book(last.get_target_id(), last.get_backup_data());
        out_message = "Da hoan tac: Khoi phuc thong tin cu cua sach [" + last.get_target_id() + "]";
    }
    return success;
}

bool library_controller::undo_book_action(std::string& out_message) {
    return undo_last_book_action(out_message);
}

bool library_controller::can_undo_user() const { return !user_undo_stack.empty(); }
bool library_controller::undo_user_action(std::string& out_message) { return false; }
bool library_controller::can_undo_borrow() const { return !borrow_undo_stack.empty(); }
bool library_controller::undo_borrow_action(std::string& out_message) { return false; }