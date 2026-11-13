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

    auto safe_parse_date = [](const std::string& str) -> date {
        int d = 1, m = 1, y = 2026;
        char s1 = '/', s2 = '/';
        std::stringstream ss_d(search_algo::trim(str));
        if (ss_d >> d >> s1 >> m >> s2 >> y) {
            try {
                return date(d, m, y);
            } catch (...) {}
        }
        return date(1, 1, 2026);
    };

    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (search_algo::trim(line).empty()) continue;

        std::stringstream ss(line);
        std::string req_id, b_id, u_id, b_date_str, due_date_str, status;
        std::getline(ss, req_id, ';');
        std::getline(ss, b_id, ';');
        std::getline(ss, u_id, ';');
        std::getline(ss, b_date_str, ';');
        std::getline(ss, due_date_str, ';');
        std::getline(ss, status, ';');

        req_id = file_helper::sanitize(req_id);
        b_id = file_helper::sanitize(b_id);
        u_id = file_helper::sanitize(u_id);
        status = file_helper::sanitize(status);
        if (req_id.empty() || b_id.empty() || u_id.empty()) continue;
        if (status.empty()) status = "Cho duyet";

        date b_date = safe_parse_date(b_date_str);
        date d_date = safe_parse_date(due_date_str);
        if (d_date < b_date) d_date = b_date;

        borrow req(req_id, b_id, u_id, b_date, d_date, status);
        book_request_queue.push(req);
    }
    file.close();
}

void library_controller::save_requests() {
    if (!file_helper::ensure_parent_dir(request_file)) return;
    std::string temp_file = request_file + ".tmp";
    std::ofstream file(temp_file, std::ios::trunc);
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
             
        book_request_queue.push(req);
    }
    file.flush();
    file.close();

    try {
        std::filesystem::rename(temp_file, request_file);
    } catch (...) {
        std::remove(request_file.c_str());
        std::rename(temp_file.c_str(), request_file.c_str());
    }
}

bool library_controller::login(std::string id, std::string password) {
    current_logged_in_user = u_service.login(file_helper::sanitize(id), password);
    if (current_logged_in_user != nullptr) {
        check_system_overdue(); 
    }
    return (current_logged_in_user != nullptr);
}

void library_controller::logout() {
    current_logged_in_user = nullptr;
    book_undo_stack.clear();
    user_undo_stack.clear();
    borrow_undo_stack.clear();
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

bool library_controller::add_new_book(std::string id, std::string title, std::string author, std::string category, int qty, int year, std::string shelf) {
    id = file_helper::sanitize(id);
    title = file_helper::sanitize(title);
    author = file_helper::sanitize(author);
    category = file_helper::sanitize(category);
    shelf = file_helper::sanitize(shelf);
    if (id.empty() || title.empty() || qty < 0) return false;

    book new_b(id, title, author, category, qty, year);
    if (b_service.add_book(new_b)) {
        std::string safe_shelf = shelf.empty() ? "Chua xep ke" : shelf;
        book_undo_stack.push(action_log<book>(id, "ADD", new_b, safe_shelf));
        
        position_list.push_back(book_location(id, category, safe_shelf));
        save_positions();
        return true;
    }
    return false;
}

bool library_controller::update_book_info(std::string old_id, book book_n, std::string new_shelf) {
    old_id = file_helper::sanitize(old_id);
    std::string new_id = file_helper::sanitize(book_n.get_id());
    new_shelf = file_helper::sanitize(new_shelf);

    node<book>* old_node = b_service.get_book_by_id(old_id);
    if (old_node == nullptr) return false;

    if (old_id != new_id &&
        (br_service.is_book_borrowed(old_id) ||
         has_pending_request_for_book(old_id))) {
        return false;
    }

    book old_backup = old_node->get_data();
    std::string old_shelf = get_book_shelf(old_id);

    book sanitized_book(new_id,
                        file_helper::sanitize(book_n.get_title()),
                        file_helper::sanitize(book_n.get_author()),
                        file_helper::sanitize(book_n.get_category()),
                        book_n.get_quantity(),
                        book_n.get_year());

    if (!b_service.update_book(old_id, sanitized_book)) {
        return false;
    }

    if (old_id != new_id) {
        br_service.cascade_book_id(old_id, new_id);
    }

    std::string old_lower = search_algo::to_lower(old_id);
    bool found_pos = false;
    for (node<book_location>* curr = position_list.get_head(); curr != nullptr; curr = curr->get_next()) {
        if (search_algo::to_lower(curr->get_Data().book_id) == old_lower) {
            curr->get_Data().book_id = new_id;
            curr->get_Data().category = sanitized_book.get_category();
            if (!new_shelf.empty()) {
                curr->get_Data().shelf = new_shelf;
            }
            found_pos = true;
            break;
        }
    }
    if (!found_pos) {
        position_list.push_back(book_location(
            new_id, sanitized_book.get_category(),
            new_shelf.empty() ? old_shelf : new_shelf));
    }
    save_positions();

    book_undo_stack.push(action_log<book>(new_id, "UPDATE", old_backup, old_shelf));
    return true;
}

bool library_controller::delete_book(std::string id) {
    id = file_helper::sanitize(id);
    if (has_pending_request_for_book(id) || br_service.is_book_borrowed(id)) {
        return false;
    }

    node<book>* target = b_service.get_book_by_id(id);
    if (target == nullptr) return false;

    book backup = target->get_data();
    std::string old_shelf = get_book_shelf(id);

    if (!b_service.delete_book(id)) {
        return false;
    }

    std::string lower_id = search_algo::to_lower(id);
    int pos = 0;
    for (node<book_location>* curr = position_list.get_head(); curr != nullptr; curr = curr->get_next(), ++pos) {
        if (search_algo::to_lower(curr->get_data().book_id) == lower_id) {
            position_list.remove(pos);
            save_positions();
            break;
        }
    }

    book_undo_stack.push(action_log<book>(id, "DELETE", backup, old_shelf));
    return true;
}

node<book>* library_controller::get_book_by_id(std::string id) {
    return b_service.get_book_by_id(file_helper::sanitize(id));
}

void library_controller::sort_books(bool (*cmp)(book, book)) {
    b_service.sort_books(cmp);
}

linked_list<borrow> library_controller::filter_borrows_by_date(date start_date, date end_date, std::string& out_message) {
    // Logic nghiệp vụ: Ngày kết thúc không được nhỏ hơn ngày bắt đầu nằm ở đây!
    if (end_date < start_date) {
        out_message = "Lỗi: Khoảng ngày lọc không hợp lệ!";
        return linked_list<borrow>(); 
    }
    
    out_message = "Đã lọc dữ liệu thành công.";
    return br_service.filter_borrows_by_date(start_date, end_date);
}

bool library_controller::borrow_book(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date, std::string& out_message) {
    if (br_service.create_borrow_ticket(borrow_id, book_id, user_id, borrow_date, due_date, b_service, u_service, out_message)) {
        borrow new_ticket(file_helper::sanitize(borrow_id),
                          file_helper::sanitize(book_id),
                          file_helper::sanitize(user_id),
                          borrow_date, due_date, "Dang muon");
        borrow_undo_stack.push(action_log<borrow>(new_ticket.get_id(), "ADD", new_ticket));
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
            borrow_undo_stack.push(action_log<borrow>(old_data.get_id(), "DELETE", old_data));
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
    try {
        date today(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
        br_service.check_and_update_overdue(today);
    } catch (...) {}
}

linked_list<borrow> library_controller::get_borrows_by_book(std::string book_id) const {
    return br_service.get_borrows_by_book(book_id);
}

bool library_controller::has_overdue_borrows(std::string user_id) const {
    return br_service.has_overdue_borrows(user_id);
}

linked_list<borrow> library_controller::get_all_borrows() const {
    return br_service.get_all_borrows();
}
bool library_controller::is_request_id_in_queue(std::string req_id) {
    int size = book_request_queue.get_size();
    bool found = false;
    for (int i = 0; i < size; ++i) {
        borrow req = book_request_queue.front();
        book_request_queue.pop();
        if (req.get_id() == req_id) {
            found = true;
        }
        book_request_queue.push(req);
    }
    return found;
}

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
            id = search_algo::trim(id);

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

            try {
                int value = std::stoi(number);
                if (value > max_id) max_id = value;
            } catch (...) {
                continue;
            }
        }
    };

    scan_file(request_file);
    scan_file("Data/borrow.txt");

    return "REQ" + std::to_string(max_id + 1);
}

bool library_controller::has_pending_request_for_book(std::string book_id) {
    int count = book_request_queue.get_size();
    bool found = false;

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

    book_id = file_helper::sanitize(book_id);
    user_id = file_helper::sanitize(user_id);

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
    // Gọi trực tiếp br_service.create_borrow_ticket để không làm bẩn borrow_undo_stack
    if (br_service.create_borrow_ticket(req.get_id(), req.get_book_id(),
                                        req.get_user_id(), req.get_borrow_date(),
                                        req.get_due_date(), b_service, u_service, msg)) {
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
        content += " Lý do: " + file_helper::sanitize(reason);
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

bool library_controller::register_shift(date d, std::string session, std::string admin_id, std::string& out_message) {
    time_t t = time(0);
    tm* now = localtime(&t);
    if (now != nullptr) {
        date today(now->tm_mday, now->tm_mon + 1, now->tm_year + 1900);
        if (d < today) {
            out_message = "Lỗi: Không thể đăng ký ca trực trong quá khứ!";
            return false;
        }
    }

    if (s_service.register_shift(d, session, file_helper::sanitize(admin_id), u_service)) {
        out_message = "Đã đăng ký ca trực thành công.";
        return true;
    }
    
    out_message = "Lỗi: Ca đầy, trùng ca hoặc mã nhân viên không hợp lệ.";
    return false;
}

bool library_controller::cancel_shift(date d, std::string session, std::string admin_id) {
    return s_service.cancel_shift(d, session, file_helper::sanitize(admin_id));
}

linked_list<shift> library_controller::get_my_shifts(std::string admin_id) {
    return s_service.get_shifts_by_admin(file_helper::sanitize(admin_id));
}

long long library_controller::get_admin_salary(std::string admin_id, int month, int year) {
    return s_service.calculate_salary(file_helper::sanitize(admin_id), month, year, 200000);
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
    id = file_helper::sanitize(id);
    name = file_helper::sanitize(name);
    password = file_helper::sanitize(password);
    role = file_helper::sanitize(role);
    if (id.empty() || name.empty() || password.empty()) return false;

    user new_u(id, name, password, role);
    if (u_service.add_user(new_u)) {
        user_undo_stack.push(action_log<user>(id, "ADD", new_u));
        return true;
    }
    return false;
}

bool library_controller::update_user_info(std::string old_id, user updated_user) {
    old_id = file_helper::sanitize(old_id);
    std::string new_id = file_helper::sanitize(updated_user.get_id());

    if (old_id != new_id &&
        (has_pending_request_for_user(old_id) ||
         br_service.has_active_borrows(old_id) ||
         s_service.has_shifts(old_id))) {
        return false;
    }

    node<user>* old_node = u_service.get_user_by_id(old_id);
    if (old_node != nullptr) {
        user old_data = old_node->get_data();
        user sanitized_u(new_id,
                         file_helper::sanitize(updated_user.get_name()),
                         file_helper::sanitize(updated_user.get_password()),
                         file_helper::sanitize(updated_user.get_role()));

        if (u_service.update_user(old_id, sanitized_u)) {
            if (old_id != new_id) {
                br_service.cascade_user_id(old_id, new_id);
            }
            user_undo_stack.push(action_log<user>(new_id, "UPDATE", old_data));
            return true;
        }
    }
    return false;
}

bool library_controller::delete_user(std::string id) {
    id = file_helper::sanitize(id);
    if (current_logged_in_user != nullptr && current_logged_in_user->get_id() == id) {
        return false;
    }

    if (has_pending_request_for_user(id) || br_service.has_active_borrows(id) || s_service.has_shifts(id)) {
        return false;
    }

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
    return u_service.get_user_by_id(file_helper::sanitize(id));
}
linked_list<user> library_controller::get_all_users() const {
    return u_service.get_all_users();
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
        if (search_algo::trim(line).empty()) continue;

        std::stringstream ss(line);
        std::string b_id, cat, shelf;
        std::getline(ss, b_id, ';');
        std::getline(ss, cat, ';');
        std::getline(ss, shelf, ';');

        b_id = file_helper::sanitize(b_id);
        cat = file_helper::sanitize(cat);
        shelf = file_helper::sanitize(shelf);
        if (b_id.empty()) continue;

        position_list.push_back(book_location(b_id, cat, shelf));
    }
    file.close();
}

void library_controller::save_positions() {
    if (!file_helper::ensure_parent_dir(position_file)) return;
    std::string temp_file = position_file + ".tmp";
    std::ofstream file(temp_file, std::ios::trunc);
    if (!file.is_open()) return;

    node<book_location>* curr = position_list.get_head();
    while (curr != nullptr) {
        file << curr->get_data().book_id << ";"
             << curr->get_data().category << ";"
             << curr->get_data().shelf << "\n";
        curr = curr->get_next();
    }
    file.flush();
    file.close();

    try {
        std::filesystem::rename(temp_file, position_file);
    } catch (...) {
        std::remove(position_file.c_str());
        std::rename(temp_file.c_str(), position_file.c_str());
    }
}

std::string library_controller::get_book_shelf(std::string book_id) {
    std::string exact_id = search_algo::trim(book_id);
    std::string lower_id = search_algo::to_lower(exact_id);

    // Ưu tiên khớp chính xác mã sách trước
    for (node<book_location>* curr = position_list.get_head(); curr != nullptr; curr = curr->get_next()) {
        if (curr->get_data().book_id == exact_id) {
            return curr->get_data().shelf;
        }
    }
    // Dự phòng khớp không phân biệt hoa thường cho dữ liệu cũ
    for (node<book_location>* curr = position_list.get_head(); curr != nullptr; curr = curr->get_next()) {
        if (search_algo::to_lower(curr->get_data().book_id) == lower_id) {
            return curr->get_data().shelf;
        }
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
    return b_service.filter_books(title, author, category, 0);
}

linked_list<book> library_controller::get_recommended_books(std::string author, std::string category, std::string exclude_book_id) {
    linked_list<book> all = b_service.search_books("");
    node<book>* target = b_service.get_book_by_id(exclude_book_id);
    if (target == nullptr || all.get_size() <= 1) return linked_list<book>();

    std::string target_auth = search_algo::to_lower(search_algo::trim(
        author.empty() ? target->get_data().get_author() : author));
    std::string target_cat = search_algo::to_lower(search_algo::trim(
        category.empty() ? target->get_data().get_category() : category));

    linked_list<book> candidates;
    candidates.push_back(target->get_data());

    for (node<book>* curr = all.get_head(); curr != nullptr; curr = curr->get_next()) {
        book b = curr->get_data();
        if (b.get_id() == exclude_book_id) continue;

        std::string b_auth = search_algo::to_lower(search_algo::trim(b.get_author()));
        std::string b_cat = search_algo::to_lower(search_algo::trim(b.get_category()));

        bool same_auth = (!target_auth.empty() && !b_auth.empty() && target_auth == b_auth);
        bool same_cat  = (!target_cat.empty() && !b_cat.empty() && target_cat == b_cat);

        if (same_auth || same_cat) {
            candidates.push_back(b);
            if (candidates.get_size() >= 60) break;
        }
    }

    if (candidates.get_size() <= 1) return linked_list<book>();

    graph<book> g(candidates.get_size() + 2);
    for (node<book>* curr = candidates.get_head(); curr != nullptr; curr = curr->get_next()) {
        g.add_vertex(curr->get_data());
    }

    for (int i = 0; i < g.get_num_vertices(); ++i) {
        book b1 = g.get_vertex_data(i);
        std::string a1 = search_algo::to_lower(search_algo::trim(b1.get_author()));
        std::string c1 = search_algo::to_lower(search_algo::trim(b1.get_category()));
        for (int j = i + 1; j < g.get_num_vertices(); ++j) {
            book b2 = g.get_vertex_data(j);
            std::string a2 = search_algo::to_lower(search_algo::trim(b2.get_author()));
            std::string c2 = search_algo::to_lower(search_algo::trim(b2.get_category()));

            if ((!a1.empty() && a1 == a2) || (!c1.empty() && c1 == c2)) {
                g.add_edge(i, j, false);
            }
        }
    }

    // Chạy BFS 
    linked_list<book> bfs_list = graph_algo::BFS(g, target->get_data());
    linked_list<book> result;
    for (node<book>* r_curr = bfs_list.get_head(); r_curr != nullptr; r_curr = r_curr->get_next()) {
        if (!(r_curr->get_data() == target->get_data())) {
            result.push_back(r_curr->get_data());
        }
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
        out_message = "Không có thao tác sách nào trong Stack để hoàn tác!";
        return false;
    }
    action_log<book> last = book_undo_stack.peek();
    std::string current_id = last.get_target_id();
    book backup = last.get_backup_data();
    std::string old_shelf = last.get_extra_info();

    if (last.get_action_type() == "ADD") {
        if (has_pending_request_for_book(current_id) || br_service.is_book_borrowed(current_id)) {
            out_message = "Không thể hoàn tác: Sách vừa thêm hiện đang có người mượn hoặc yêu cầu!";
            return false;
        }
        if (!b_service.delete_book(current_id)) {
            out_message = "Không thể hoàn tác thao tác thêm sách này!";
            return false;
        }
        std::string lower_id = search_algo::to_lower(current_id);
        int pos = 0;
        for (node<book_location>* c = position_list.get_head(); c != nullptr; c = c->get_next(), ++pos) {
            if (search_algo::to_lower(c->get_data().book_id) == lower_id) {
                position_list.remove(pos);
                save_positions();
                break;
            }
        }
        book_undo_stack.pop();
        out_message = "Đã hoàn tác: Xóa sách vừa thêm [" + current_id + "]";
        return true;
    } 
    else if (last.get_action_type() == "DELETE") {
        if (!b_service.add_book(backup)) {
            out_message = "Không thể khôi phục: Mã sách [" + backup.get_id() + "] đã bị trùng!";
            return false;
        }
        position_list.push_back(book_location(
            backup.get_id(), backup.get_category(),
            old_shelf.empty() ? "Da khoi phuc" : old_shelf));
        save_positions();
        book_undo_stack.pop();
        out_message = "Đã hoàn tác: Khôi phục sách vừa xóa [" + backup.get_id() + "]";
        return true;
    } 
    else if (last.get_action_type() == "UPDATE") {
        if (current_id != backup.get_id() &&
            (br_service.is_book_borrowed(current_id) || has_pending_request_for_book(current_id))) {
            out_message = "Không thể hoàn tác đổi mã sách khi sách đang được mượn!";
            return false;
        }
        if (!b_service.update_book(current_id, backup)) {
            out_message = "Không thể hoàn tác cập nhật sách!";
            return false;
        }
        if (current_id != backup.get_id()) {
            br_service.cascade_book_id(current_id, backup.get_id());
        }
        std::string cur_lower = search_algo::to_lower(current_id);
        for (node<book_location>* c = position_list.get_head(); c != nullptr; c = c->get_next()) {
            if (search_algo::to_lower(c->get_Data().book_id) == cur_lower) {
                c->get_Data().book_id = backup.get_id();
                c->get_Data().category = backup.get_category();
                if (!old_shelf.empty()) c->get_Data().shelf = old_shelf;
                break;
            }
        }
        save_positions();
        book_undo_stack.pop();
        out_message = "Đã hoàn tác: Khôi phục thông tin cũ của sách [" + backup.get_id() + "]";
        return true;
    }
    return false;
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
    
    action_log<user> last = user_undo_stack.peek();
    std::string current_id = last.get_target_id();
    user backup = last.get_backup_data();

    if (last.get_action_type() == "ADD") {
        if (current_logged_in_user != nullptr && current_logged_in_user->get_id() == current_id) {
            out_message = "Không thể hoàn tác xóa tài khoản đang đăng nhập!";
            return false;
        }
        if (has_pending_request_for_user(current_id) ||
            br_service.has_active_borrows(current_id) ||
            s_service.has_shifts(current_id)) {
            out_message = "Không thể hoàn tác: Tài khoản vừa thêm đang có phát sinh mượn sách hoặc ca trực!";
            return false;
        }
        if (u_service.delete_user(current_id)) {
            user_undo_stack.pop();
            out_message = "Đã hoàn tác: Xóa tài khoản vừa thêm [" + current_id + "]";
            return true;
        }
        out_message = "Không thể hoàn tác xóa tài khoản này!";
        return false;
    } 
    else if (last.get_action_type() == "DELETE") {
        if (u_service.add_user(backup)) {
            user_undo_stack.pop();
            out_message = "Đã hoàn tác: Khôi phục tài khoản vừa xóa [" + backup.get_id() + "]";
            return true;
        }
        out_message = "Không thể khôi phục: Mã tài khoản đã tồn tại!";
        return false;
    } 
    else if (last.get_action_type() == "UPDATE") {
        if (current_id != backup.get_id() &&
            (has_pending_request_for_user(current_id) ||
             br_service.has_active_borrows(current_id) ||
             s_service.has_shifts(current_id))) {
            out_message = "Không thể hoàn tác đổi mã khi tài khoản đang có giao dịch!";
            return false;
        }
        if (u_service.update_user(current_id, backup)) {
            if (current_id != backup.get_id()) {
                br_service.cascade_user_id(current_id, backup.get_id());
            }
            user_undo_stack.pop();
            out_message = "Đã hoàn tác: Khôi phục thông tin cũ của tài khoản [" + backup.get_id() + "]";
            return true;
        }
        out_message = "Không thể hoàn tác cập nhật tài khoản!";
        return false;
    }
    return false;
}

bool library_controller::can_undo_borrow() const { return !borrow_undo_stack.empty(); }

bool library_controller::undo_borrow_action(std::string& out_message) {
    if (borrow_undo_stack.empty()) {
        out_message = "Không có thao tác phiếu mượn nào để hoàn tác!";
        return false;
    }

    action_log<borrow> last = borrow_undo_stack.peek();

    bool success = false;
    if (last.get_action_type() == "ADD") {
        success = br_service.undo_create_borrow(last.get_target_id(), b_service);
        if (success) {
            borrow_undo_stack.pop();
            out_message = "Đã hoàn tác: Xóa phiếu mượn vừa lập [" + last.get_target_id() + "]. Đã cộng lại số lượng sách.";
            return true;
        }
    } else if (last.get_action_type() == "DELETE") {
        success = br_service.undo_delete_borrow(last.get_backup_data(), b_service);
        if (success) {
            borrow_undo_stack.pop();
            out_message = "Đã hoàn tác: Khôi phục phiếu mượn vừa xóa [" + last.get_target_id() + "]";
            return true;
        }
    }
    
    out_message = "Không thể hoàn tác thao tác phiếu mượn này vì trạng thái không còn phù hợp.";
    return false;
}

std::vector<notification> library_controller::get_my_notifications(std::string user_id) const {
    return n_service.get_notifications(user_id);
}

int library_controller::get_unread_notification_count(std::string user_id) const {
    return n_service.get_unread_count(user_id);
}

bool library_controller::mark_notification_read(std::string notification_id) {
    return n_service.mark_as_read(notification_id);
}
linked_list<user> library_controller::filter_users(std::string id_kw, std::string name_kw, std::string role_kw) const {
    return u_service.filter_users(id_kw, name_kw, role_kw);
}

linked_list<borrow> library_controller::filter_borrows(std::string id_kw, std::string book_kw, std::string user_kw, std::string status_kw) const {
    return br_service.filter_borrows(id_kw, book_kw, user_kw, status_kw);
}