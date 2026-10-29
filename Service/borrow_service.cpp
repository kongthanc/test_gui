#include "borrow_service.h"

borrow_service::borrow_service(std::string filename) {
    data_file = filename;
    load_data();
}

borrow_service::~borrow_service() {
    save_data(); // Tự động đồng bộ file khi thoát
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

    // Parse ngày mượn (VD: 18/09/2026)
    int d1, m1, y1;
    char slash;
    std::stringstream ss_b(b_date_str);
    ss_b >> d1 >> slash >> m1 >> slash >> y1;
    date borrow_date(d1, m1, y1);

    // Parse ngày hẹn trả (VD: 25/09/2026)
    int d2, m2, y2;
    std::stringstream ss_d(due_date_str);
    ss_d >> d2 >> slash >> m2 >> slash >> y2;
    date due_date(d2, m2, y2);

    b = borrow(id, book_id, user_id, borrow_date, due_date, status);
}

// --- 1. ĐỌC DỮ LIỆU TỪ FILE ---
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
        borrow_index.insert(b.get_id(), new_node); // Đăng ký bảng băm O(1)
    }
    file.close();
    return true;
}

// --- 2. GHI DỮ LIỆU XUỐNG FILE ---
bool borrow_service::save_data() {
    return file_helper::save_list_to_file(data_file, borrow_list);
}

// --- 3. TẠO PHIẾU MƯỢN SÁCH (Dùng toán tử --) ---
bool borrow_service::create_borrow_ticket(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date, book_service& b_service, user_service& u_service) {
    // Check trùng mã phiếu mượn O(1)
    if (borrow_index.find(borrow_id) != nullptr) return false;

    // Check xem User có tồn tại không
    if (u_service.get_user_by_id(user_id) == nullptr) return false;

    // Check xem User có đang nợ sách quá hạn không
    if (has_overdue_borrows(user_id)) return false;

    // Check xem User có đang mượn chính cuốn sách này chưa trả không
    if (is_user_borrowing_book(user_id, book_id)) return false;

    // Check xem Book có tồn tại và còn số lượng không
    node<book>* book_node = b_service.get_book_by_id(book_id);
    if (book_node == nullptr) return false;
    if (due_date < borrow_date) return false; // Ngày hẹn trả không được trước ngày mượn
    
    // Lấy tham chiếu trực tiếp đến đối tượng book bên trong node kho sách
    book& target_book = book_node->get_Data();
    if (target_book.get_quantity() <= 0) return false; // Hết sách trong kho

    // DÙNG TOÁN TỬ -- ĐỂ TRỪ KHO TRỰC TIẾP TRÊN RAM
    --target_book;

    // Tạo phiếu mượn mới với trạng thái ban đầu là "Dang muon"
    borrow new_ticket(borrow_id, book_id, user_id, borrow_date, due_date, "Dang muon");

    borrow_list.push_back(new_ticket);
    borrow_index.insert(borrow_id, borrow_list.get_tail());

    // Tự động lưu ngay xuống file để đảm bảo an toàn dữ liệu
    save_data();
    b_service.save_data();

    return true;
}

// --- 4. TRẢ SÁCH (Dùng toán tử ++) ---
bool borrow_service::return_book(std::string borrow_id, book_service& b_service) {
    node<borrow>* ticket_node = borrow_index.find(borrow_id);
    if (ticket_node == nullptr) return false;
    
    // Lấy tham chiếu trực tiếp đến đối tượng borrow nằm bên trong node
    borrow& ticket = ticket_node->get_Data();
    
    // Nếu sách đã trả rồi thì không xử lý nữa
    if (ticket.get_status() == "Da tra") return false;

    // Cập nhật trạng thái phiếu thành "Da tra"
    ticket.set_status("Da tra");

    // Hoàn lại số lượng sách vào kho (+1)
    node<book>* book_node = b_service.get_book_by_id(ticket.get_book_id());
    if (book_node != nullptr) {
        // Lấy tham chiếu trực tiếp đến đối tượng book trong node kho sách
        book& target_book = book_node->get_Data(); 
        
        // DÙNG TOÁN TỬ ++ ĐỂ HOÀN KHO TRỰC TIẾP TRÊN RAM
        ++target_book; 
    }

    // Tự động lưu ngay xuống file
    save_data();
    b_service.save_data();

    return true;
}

// --- 5. TỰ ĐỘNG KIỂM TRA QUÁ HẠN ---
void borrow_service::check_and_update_overdue(date current_date) {
    bool has_changed = false;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow& ticket = curr->get_Data();

        // Nếu phiếu đang mượn và ngày hiện tại vượt quá ngày hẹn trả (dùng toán tử > đã overload ở class date)
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
    return borrow_index.find(id);
}

// Lọc phiếu mượn theo User
linked_list<borrow> borrow_service::get_borrows_by_user(std::string user_id, bool active_only) {
    linked_list<borrow> result;
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_user_id() == user_id) {
            // active_only = true: chỉ lấy phiếu chưa trả (Dang muon hoặc Qua han)
            // active_only = false: lấy toàn bộ phiếu của User, kể cả đã trả
            if (!active_only || b.get_status() != "Da tra") {
                result.push_back(b);
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
    // 1. Tìm node phiếu mượn qua bảng băm O(1)
    node<borrow>* target = borrow_index.find(borrow_id);
    if (target == nullptr) {
        return false; // Không tìm thấy mã phiếu
    }

    // CHỈ CHO PHÉP XÓA KHI ĐÃ TRẢ SÁCH XONG! ("Da tra")
    // Tuyệt đối không cho xóa khi "Dang muon" hoặc "Qua han" để tránh mất sách
    if (target->get_Data().get_status() != "Da tra") {
        return false; 
    }

    // 2. Xóa chỉ mục trên Bảng băm trước
    borrow_index.remove(borrow_id);

    // 3. Xóa node khỏi Danh sách liên kết kép
    borrow_list.remove_node(target);

    // Tự động lưu thay đổi
    save_data();

    return true;
}

// --- KIỂM TRA RÀNG BUỘC TOÀN VẸN ---
bool borrow_service::is_book_borrowed(std::string book_id) const {
    node<borrow>* curr = borrow_list.get_head();
    while (curr != nullptr) {
        borrow b = curr->get_data();
        if (b.get_book_id() == book_id && b.get_status() != "Da tra") {
            return true; // Sách đang có người mượn hoặc quá hạn
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
            return true; // Người dùng còn sách chưa trả
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
            return true; // Đang mượn cuốn này rồi
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
            return true; // Đang có sách quá hạn chưa trả
        }
        curr = curr->get_next();
    }
    return false;
}
