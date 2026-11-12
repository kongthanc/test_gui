#include "library_controller.h"

library_controller::library_controller()
    : b_service("Data/book.txt"),
      u_service("Data/user.txt"),
      br_service("Data/borrow.txt"),
      s_service("Data/shift.txt"),
      n_service("Data/notification.txt") {

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
    // Không đổi ID sách khi vẫn còn yêu cầu mượn đang chờ.
    if (old_id != book_n.get_id() &&
        (br_service.is_book_borrowed(old_id) ||
        has_pending_request_for_book(old_id))) {
            return false;
    }
    return b_service.update_book(old_id, book_n);
}

bool library_controller::delete_book(std::string id) {
    // Không xóa sách đang được yêu cầu mượn hoặc đang có người mượn.
    if (has_pending_request_for_book(id) || br_service.is_book_borrowed(id)) {
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

bool library_controller::borrow_book(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date, std::string& out_message) {
    if (br_service.create_borrow_ticket(borrow_id, book_id, user_id, borrow_date, due_date, b_service, u_service, out_message)) {
        // Lưu lịch sử: Đã tạo phiếu mượn
        borrow new_ticket(borrow_id, book_id, user_id, borrow_date, due_date, "Dang muon");
        borrow_undo_stack.push(action_log<borrow>(borrow_id, "ADD", new_ticket));
        return true;
    }
    return false;
}

bool library_controller::return_book(std::string borrow_id) {
    return br_service.return_book(borrow_id, b_service);
}

bool library_controller::delete_borrow(std::string borrow_id) {
    node<borrow>* old_node = br_service.get_borrow_by_id(borrow_id);
    if (old_node != nullptr) {
        borrow old_data = old_node->get_data();
        if (br_service.delete_borrow_ticket(borrow_id)) {
            // LƯU LỊCH SỬ: Đã xóa phiếu mượn
            borrow_undo_stack.push(action_log<borrow>(borrow_id, "DELETE", old_data));
            return true;
        }
    }
    return false;
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

linked_list<borrow> library_controller::get_borrows_by_book(std::string book_id) const {
    return br_service.get_borrows_by_book(book_id);
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
    int max_id = 0;

    auto scan_file = [&max_id](const std::string& filename) {
        std::ifstream file(filename);
        std::string line;

        while (std::getline(file, line)) {
            if (line.empty()) continue;

            std::stringstream ss(line);
            std::string id;
            std::getline(ss, id, ';');

            if (id.size() <= 3 || id.compare(0, 3, "REQ") != 0)
                continue;

            std::string number = id.substr(3);
            bool valid = true;

            for (char c : number) {
                if (c < '0' || c > '9') {
                    valid = false;
                    break;
                }
            }

            if (!valid) continue;

            int value = std::stoi(number);
            if (value > max_id) max_id = value;
        }
    };

    scan_file(request_file);
    scan_file("Data/borrow.txt");

    return "REQ" + std::to_string(max_id + 1);
}

bool library_controller::has_pending_request_for_book(std::string book_id) {
    int count = book_request_queue.get_size();
    bool found = false;

    // Duyệt một vòng, giữ nguyên thứ tự FIFO.
    for (int i = 0; i < count; ++i) {
        borrow req = book_request_queue.front();
        book_request_queue.pop();

        if (req.get_book_id() == book_id &&
            req.get_status() == "Cho duyet") {
            found = true;
        }

        book_request_queue.push(req);
    }

    return found;
}

bool library_controller::has_pending_request_for_user(std::string user_id) {
    int count = book_request_queue.get_size();
    bool found = false;

    // Duyệt một vòng, giữ nguyên thứ tự FIFO.
    for (int i = 0; i < count; ++i) {
        borrow req = book_request_queue.front();
        book_request_queue.pop();

        if (req.get_user_id() == user_id &&
            req.get_status() == "Cho duyet") {
            found = true;
        }

        book_request_queue.push(req);
    }

    return found;
}

bool library_controller::register_book_request(
    std::string book_id, std::string user_id,
    date b_date, date due_date, std::string& out_message) {

    node<book>* b_node = b_service.get_book_by_id(book_id);
    if (b_node == nullptr) {
        out_message = "Không tìm thấy mã sách.";
        return false;
    }
    if (b_node->get_data().get_quantity() <= 0) {
        out_message = "Sách này đã hết trong kho.";
        return false;
    }
    if (due_date < b_date) {
        out_message = "Hạn trả phải lớn hơn hoặc bằng ngày mượn.";
        return false;
    }

    // Không cho một độc giả tạo nhiều yêu cầu đang chờ cho cùng một đầu sách.
    bool has_duplicate_pending_request = false;
    int request_count = book_request_queue.get_size();

    for (int i = 0; i < request_count; ++i) {
        borrow pending_request = book_request_queue.front();
        book_request_queue.pop();

        if (pending_request.get_user_id() == user_id &&
            pending_request.get_book_id() == book_id &&
            pending_request.get_status() == "Cho duyet") {
            has_duplicate_pending_request = true;
        }

        book_request_queue.push(pending_request);
    }

    if (has_duplicate_pending_request) {
        out_message = "Bạn đã có yêu cầu mượn cuốn sách này đang chờ duyệt.";
        return false;
    }

    if (br_service.is_user_borrowing_book(user_id, book_id)) {
        out_message = "Bạn đang mượn cuốn sách này rồi.";
        return false;
    }

    if (br_service.has_overdue_borrows(user_id)) {
        out_message = "Bạn còn sách quá hạn chưa trả, không thể mượn thêm.";
        return false;
    }

    if (br_service.count_active_borrows_by_user(user_id) >= 7) {
        out_message = "Bạn đã đạt giới hạn mượn tối đa 7 cuốn sách.";
        return false;
    }

    std::string req_id = generate_request_id();
    borrow ticket(req_id, book_id, user_id, b_date, due_date, "Cho duyet");

    book_request_queue.push(ticket);
    save_requests();

    out_message = "Đã đưa yêu cầu vào hàng đợi.";
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
    std::string msg;
    if (borrow_book(req.get_id(), req.get_book_id(), 
        req.get_user_id(), req.get_borrow_date(), 
            req.get_due_date(), msg)) {
        book_request_queue.pop();
        save_requests();
        node<book>* found = b_service.get_book_by_id(req.get_book_id());
        std::string book_name = req.get_book_id();

        if (found != nullptr) {
            book_name = found->get_data().get_title();
        }

        n_service.add_notification(
            req.get_user_id(),
            "Yêu cầu mượn đã được duyệt",
            "Yêu cầu mượn sách " + book_name +
            " (" + req.get_book_id() + ") đã được duyệt."
        );
        return true;
    }
    return false;
}

bool library_controller::reject_next_request(std::string reason) {
    if (book_request_queue.is_empty()) return false;
    borrow req = book_request_queue.front();
    book_request_queue.pop();
    save_requests();
    br_service.log_rejected_request(req);
    node<book>* found = b_service.get_book_by_id(req.get_book_id());
    std::string book_name = req.get_book_id();

    if (found != nullptr) {
        book_name = found->get_data().get_title();
    }

    std::string content = "Yêu cầu mượn sách " + book_name +
        " (" + req.get_book_id() + ") đã bị từ chối.";

    if (!reason.empty()) {
        content += " Lý do: " + reason;
    }

    n_service.add_notification(
        req.get_user_id(),
        "Yêu cầu mượn bị từ chối",
        content
    );
    return true;
}

int library_controller::get_pending_request_count() const {
    return book_request_queue.get_size();
}

linked_list<borrow> library_controller::get_my_pending_requests(std::string user_id) {
    linked_list<borrow> requests;
    int size = book_request_queue.get_size();

    // Duyệt đúng một vòng và giữ nguyên thứ tự FIFO của hàng đợi.
    for (int i = 0; i < size; ++i) {
        borrow req = book_request_queue.front();
        book_request_queue.pop();

        if (req.get_user_id() == user_id && req.get_status() == "Cho duyet") {
            requests.push_back(req);
        }

        book_request_queue.push(req);
    }

    return requests;
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
    if (u_service.add_user(new_u)) {
        // LƯU LỊCH SỬ: Thêm tài khoản thành công -> Ghi log ADD
        user_undo_stack.push(action_log<user>(id, "ADD", new_u));
        return true;
    }
    return false;
}

bool library_controller::update_user_info(std::string old_id, user updated_user) {
    // Không đổi ID tài khoản khi còn yêu cầu mượn đang chờ.
    if (old_id != updated_user.get_id() && has_pending_request_for_user(old_id)) {
        return false;
    }
    
    // LƯU LỊCH SỬ: Lấy dữ liệu cũ trước khi update
    node<user>* old_node = u_service.get_user_by_id(old_id);
    if (old_node != nullptr) {
        user old_data = old_node->get_data();
        if (u_service.update_user(old_id, updated_user)) {
            user_undo_stack.push(action_log<user>(old_id, "UPDATE", old_data));
            return true;
        }
    }
    return false;
}

bool library_controller::delete_user(std::string id) {
    if (current_logged_in_user != nullptr && current_logged_in_user->get_id() == id) {
        return false;
    }

    // Không xóa tài khoản còn yêu cầu mượn đang chờ, có sách đang mượn, có ca trực
    if (has_pending_request_for_user(id) || br_service.has_active_borrows(id) || s_service.has_shifts(id)) {
        return false;
    }
    
    // LƯU LỊCH SỬ: Lấy dữ liệu cũ trước khi xóa
    node<user>* old_node = u_service.get_user_by_id(id);
    if (old_node != nullptr) {
        user old_data = old_node->get_data();
        if (u_service.delete_user(id)) {
            user_undo_stack.push(action_log<user>(id, "DELETE", old_data));
            return true;
        }
    }
    return false;
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

bool library_controller::is_book_borrowed(std::string book_id) const {
    return br_service.is_book_borrowed(book_id);
}

bool library_controller::has_user_shifts(std::string user_id) const {
    return s_service.has_shifts(user_id);
}

bool library_controller::has_active_borrows(std::string user_id) const {
    return br_service.has_active_borrows(user_id);
}

int library_controller::get_user_active_borrow_count(std::string user_id) const {
    return br_service.count_active_borrows_by_user(user_id);
}

linked_list<book> library_controller::filter_books(std::string title, std::string author, std::string category) {
    return b_service.filter_books(title, author, category, 0); // year = 0
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

bool library_controller::undo_user_action(std::string& out_message) {
    if (user_undo_stack.empty()) {
        out_message = "Không có thao tác tài khoản nào để hoàn tác!";
        return false;
    }
    
    // Lấy thao tác gần nhất ra khỏi stack
    action_log<user> last = user_undo_stack.peek();
    user_undo_stack.pop();

    bool success = false;
    if (last.get_action_type() == "ADD") {
        success = u_service.delete_user(last.get_target_id());
        out_message = "Đã hoàn tác: Xóa tài khoản vừa thêm [" + last.get_target_id() + "]";
    } else if (last.get_action_type() == "DELETE") {
        success = u_service.add_user(last.get_backup_data());
        out_message = "Đã hoàn tác: Khôi phục tài khoản vừa xóa [" + last.get_target_id() + "]";
    } else if (last.get_action_type() == "UPDATE") {
        success = u_service.update_user(last.get_target_id(), last.get_backup_data());
        out_message = "Đã hoàn tác: Khôi phục thông tin cũ của tài khoản [" + last.get_target_id() + "]";
    }
    return success;
}

bool library_controller::can_undo_borrow() const { return !borrow_undo_stack.empty(); }

bool library_controller::undo_borrow_action(std::string& out_message) {
    if (borrow_undo_stack.empty()) {
        out_message = "Không có thao tác phiếu mượn nào để hoàn tác!";
        return false;
    }

    action_log<borrow> last = borrow_undo_stack.peek();
    borrow_undo_stack.pop();

    bool success = false;
    if (last.get_action_type() == "ADD") {
        success = br_service.undo_create_borrow(last.get_target_id(), b_service);
        out_message = "Đã hoàn tác: Xóa phiếu mượn vừa lập [" + last.get_target_id() + "]. Đã cộng lại số lượng sách.";
    } else if (last.get_action_type() == "DELETE") {
        success = br_service.undo_delete_borrow(last.get_backup_data(), b_service);
        out_message = "Đã hoàn tác: Khôi phục phiếu mượn vừa xóa [" + last.get_target_id() + "]";
    }
    
    if (!success) {
        out_message = "Không thể hoàn tác thao tác phiếu mượn này vì trạng thái không còn phù hợp.";
    }
    return success;
}

std::vector<notification> library_controller::get_my_notifications(
    std::string user_id) const {

    return n_service.get_notifications(user_id);
}

int library_controller::get_unread_notification_count(
    std::string user_id) const {

    return n_service.get_unread_count(user_id);
}

bool library_controller::mark_notification_read(
    std::string notification_id) {

    return n_service.mark_as_read(notification_id);
}