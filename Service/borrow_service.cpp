#include "borrow_service.h"

borrow_service::borrow_service(std::string filename) {
    data_file = filename;
    load_data();
}

borrow_service::~borrow_service() {
    save_data();
}

// --- Tách dòng từ borrow.txt (Định dạng: BorrowID;BookID;UserID;BorrowDate;DueDate;Status) ---
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

    date borrow_date = safe_parse_date(b_date_str);
    date due_date = safe_parse_date(due_date_str);
    if (due_date < borrow_date) {
        due_date = borrow_date;
    }

    std::string st = file_helper::sanitize(status);
    if (st != "Dang muon" && st != "Da tra" && st != "Qua han" && st != "Tu choi") {
        st = "Da tra";
    }

    b = borrow(file_helper::sanitize(id),
               file_helper::sanitize(book_id),
               file_helper::sanitize(user_id),
               borrow_date, due_date, st);
}

// --- 1. ĐỌC DỮ LIỆU TỪ FILE ---
bool borrow_service::load_data() {
    std::ifstream file(data_file);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (search_algo::trim(line).empty()) continue;

        borrow b;
        parse_line(line, b);
        if (b.get_id().empty() || b.get_book_id().empty() || b.get_user_id().empty()) continue;
        if (borrow_index.find(b.get_id()) != nullptr) continue;

        borrow_list.push_back(b);
        node<borrow>* new_node = borrow_list.get_tail();
        borrow_index.insert(b.get_id(), new_node);
    }
    file.close();
    return true;
}

// --- 2. GHI DỮ LIỆU XUỐNG FILE ---
bool borrow_service::save_data() {
    return file_helper::save_list_to_file(data_file, borrow_list);
}

bool borrow_service::create_borrow_ticket(
    std::string borrow_id, std::string book_id, std::string user_id,
    date borrow_date, date due_date, book_service& b_service,
    user_service& u_service, std::string& out_message) {

    borrow_id = file_helper::sanitize(borrow_id);
    book_id = file_helper::sanitize(book_id);
    user_id = file_helper::sanitize(user_id);

    if (borrow_id.empty()) {
        out_message = "Mã phiếu mượn không hợp lệ.";
        return false;
    }
    if (borrow_index.find(borrow_id) != nullptr) {
        out_message = "Mã phiếu mượn đã tồn tại.";
        return false;
    }
    if (u_service.get_user_by_id(user_id) == nullptr) {
        out_message = "Không tìm thấy mã độc giả.";
        return false;
    }
    if (has_overdue_borrows(user_id)) {
        out_message = "Độc giả đang có sách quá hạn chưa trả.";
        return false;
    }
    if (count_active_borrows_by_user(user_id) >= 7) {
        out_message = "Độc giả đã mượn tối đa 7 cuốn sách.";
        return false;
    }
    if (is_user_borrowing_book(user_id, book_id)) {
        out_message = "Độc giả đang mượn cuốn sách này rồi.";
        return false;
    }
    node<book>* book_node = b_service.get_book_by_id(book_id);
    if (book_node == nullptr) {
        out_message = "Không tìm thấy mã sách.";
        return false;
    }
    if (due_date < borrow_date) {
        out_message = "Ngày hạn trả không được trước ngày mượn.";
        return false;
    }
    book& target_book = book_node->get_Data();
    if (target_book.get_quantity() <= 0) {
        out_message = "Sách đã hết số lượng trong kho.";
        return false;
    }

    --target_book;
    borrow new_ticket(borrow_id, book_id, user_id, borrow_date, due_date, "Dang muon");
    borrow_list.push_back(new_ticket);
    borrow_index.insert(borrow_id, borrow_list.get_tail());

    if (!save_data() || !b_service.save_data()) {
        out_message = "Không thể lưu dữ liệu xuống ổ đĩa.";
        return false;
    }

    out_message = "Tạo phiếu mượn thành công.";
    return true;
}

// --- 4. TRẢ SÁCH (Dùng toán tử ++) ---
bool borrow_service::return_book(std::string borrow_id, book_service& b_service) {
    borrow_id = file_helper::sanitize(borrow_id);
    node<borrow>* ticket_node = borrow_index.find(borrow_id);
    if (ticket_node == nullptr) return false;

    borrow& ticket = ticket_node->get_Data();
    std::string st = ticket.get_status();

    // Chỉ cho phép xác nhận trả đối với sách đang thực sự được mượn hoặc quá hạn
    if (st != "Dang muon" && st != "Qua han") return false;

    // Cập nhật trạng thái phiếu thành "Da tra"
    ticket.set_status("Da tra");

    // Hoàn lại số lượng sách vào kho (+1)
    node<book>* book_node = b_service.get_book_by_id(ticket.get_book_id());
    if (book_node != nullptr) {
        book& target_book = book_node->get_Data();
        ++target_book;
    }

    return save_data() && b_service.save_data();
}

// --- 5. TỰ ĐỘNG KIỂM TRA QUÁ HẠN ---
void borrow_service::check_and_update_overdue(date current_date) {
    bool has_changed = false;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow& ticket = curr->get_Data();

        // Nếu phiếu đang mượn và ngày hiện tại vượt quá ngày hạn trả (dùng toán tử > đã overload ở class date)
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

// --- 7. TRA CỨU NHANH THEO ID ---
node<borrow>* borrow_service::get_borrow_by_id(std::string id) {
    return borrow_index.find(file_helper::sanitize(id));
}

// Lọc phiếu mượn theo User
linked_list<borrow> borrow_service::get_borrows_by_user(std::string user_id, bool active_only) {
    linked_list<borrow> result;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id) {
            if (active_only) {
                if (b.get_status() == "Dang muon" || b.get_status() == "Qua han") {
                    result.push_back(b);
                }
            } else {
                if (b.get_status() == "Da tra" || b.get_status() == "Tu choi") {
                    result.push_back(b);
                }
            }
        }
        curr = curr->get_next();
    }
    return result;
}

// Lọc theo khoảng ngày mượn
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

// --- XÓA PHIẾU MƯỢN ---
bool borrow_service::delete_borrow_ticket(std::string borrow_id) {
    borrow_id = file_helper::sanitize(borrow_id);
    node<borrow>* target = borrow_index.find(borrow_id);
    if (target == nullptr) return false;

    std::string st = target->get_Data().get_status();
    // Chỉ cấm xóa khi sách đang được mượn hoặc quá hạn chưa trả
    if (st == "Dang muon" || st == "Qua han") {
        return false;
    }

    borrow_index.remove(borrow_id);
    borrow_list.remove_node(target);
    return save_data();
}

// --- KIỂM TRA RÀNG BUỘC TOÀN VẸN ---
bool borrow_service::is_book_borrowed(std::string book_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_book_id() == book_id &&
            (b.get_status() == "Dang muon" || b.get_status() == "Qua han")) {
            return true;
        }
        curr = curr->get_next();
    }
    return false;
}

bool borrow_service::has_active_borrows(std::string user_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id &&
            (b.get_status() == "Dang muon" || b.get_status() == "Qua han")) {
            return true;
        }
        curr = curr->get_next();
    }
    return false;
}

bool borrow_service::is_user_borrowing_book(std::string user_id, std::string book_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id &&
            b.get_book_id() == book_id &&
            (b.get_status() == "Dang muon" || b.get_status() == "Qua han")) {
            return true;
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
            return true;
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
        if (b.get_user_id() == user_id &&
            (b.get_status() == "Dang muon" || b.get_status() == "Qua han")) {
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

void borrow_service::log_rejected_request(borrow b) {
    if (borrow_index.find(b.get_id()) != nullptr) return;
    b.set_status("Tu choi");
    borrow_list.push_back(b);
    borrow_index.insert(b.get_id(), borrow_list.get_tail());
    save_data();
}

// --- 8. HOÀN TÁC TRẢ SÁCH / XÓA PHIẾU ---
// Khi hoàn tác xóa, ta phải phục hồi phiếu về trạng thái "Đã trả" (vì chỉ xóa được khi đã trả)
// Nên KHÔNG cần trừ đi 1 số lượng sách trong kho (vì sách đang nằm trong kho rồi)
bool borrow_service::undo_delete_borrow(borrow old_borrow, book_service& b_service) {
    if (borrow_index.find(old_borrow.get_id()) != nullptr) return false;

    borrow_list.push_back(old_borrow);
    borrow_index.insert(old_borrow.get_id(), borrow_list.get_tail());
    return save_data();
}

// --- 9. HOÀN TÁC LẬP PHIẾU MƯỢN ---
// Khi hoàn tác lập phiếu, phiếu đó sẽ bị xóa bỏ hoàn toàn
// Và ta phải HOÀN LẠI (+1) số lượng sách vào kho
bool borrow_service::undo_create_borrow(std::string borrow_id, book_service& b_service) {
    node<borrow>* target = borrow_index.find(borrow_id);
    if (target == nullptr) return false;

    // Chỉ hoàn tác được nếu phiếu vẫn đang ở trạng thái "Dang muon"
    if (target->get_Data().get_status() != "Dang muon") return false;

    // +1 lại số lượng sách vào kho
    node<book>* book_node = b_service.get_book_by_id(target->get_Data().get_book_id());
    if (book_node != nullptr) {
        book& target_book = book_node->get_Data();
        ++target_book;
    }

    borrow_index.remove(borrow_id);
    borrow_list.remove_node(target);

    return save_data() && b_service.save_data();
}

// --- LẤY DANH SÁCH PHIẾU MƯỢN THEO MÃ SÁCH ---
linked_list<borrow> borrow_service::get_borrows_by_book(std::string book_id) const {
    linked_list<borrow> result;
    node<borrow>* curr = borrow_list.get_head();

    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_book_id() == book_id) {
            result.push_back(b);
        }
        curr = curr->get_next();
    }
    return result;
}

void borrow_service::cascade_book_id(const std::string& old_id, const std::string& new_id) {
    if (old_id == new_id) return;
    bool changed = false;
    for (node<borrow>* curr = borrow_list.get_head(); curr != nullptr; curr = curr->get_next()) {
        if (curr->get_Data().get_book_id() == old_id) {
            curr->get_Data().set_book_id(new_id);
            changed = true;
        }
    }
    if (changed) save_data();
}

void borrow_service::cascade_user_id(const std::string& old_id, const std::string& new_id) {
    if (old_id == new_id) return;
    bool changed = false;
    for (node<borrow>* curr = borrow_list.get_head(); curr != nullptr; curr = curr->get_next()) {
        if (curr->get_Data().get_user_id() == old_id) {
            curr->get_Data().set_user_id(new_id);
            changed = true;
        }
    }
    if (changed) save_data();
}