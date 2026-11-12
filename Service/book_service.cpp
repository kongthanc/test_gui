
#include "book_service.h"

book_service::book_service(std::string filename) {
    data_file = filename;
    load_data();
}

book_service::~book_service() {
    save_data(); // Tự động lưu file khi tắt chương trình
}

// --- 1. ĐỌC DỮ LIỆU TỪ FILE ---
bool book_service::load_data() {
    std::ifstream file(data_file);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        book b;
        parse_line(line, b); // Tách dòng thành đối tượng book
        
        // Đẩy vào Danh sách liên kết
        book_list.push_back(b);
        
        // Đăng ký ngay địa chỉ node đó vào Bảng băm
        node<book>* new_node = book_list.get_tail(); 
        book_index.insert(b.get_id(), new_node);
    }
    file.close();
    return true;
}

// --- 2. GHI DỮ LIỆU XUỐNG FILE ---
bool book_service::save_data() {
    return file_helper::save_list_to_file(data_file, book_list);
}

// --- 3. THÊM SÁCH ---
bool book_service::add_book(book new_book) {
    // Kiểm tra O(1) xem mã sách đã tồn tại chưa
    if (book_index.find(new_book.get_id()) != nullptr) {
        return false; // Báo lỗi trùng ID
    }

    book_list.push_back(new_book);
    book_index.insert(new_book.get_id(), book_list.get_tail());

    save_data(); // Lưu ngay xuống file
    return true;
}

// --- 4. XÓA SÁCH ---
bool book_service::delete_book(std::string id) {
    node<book>* target = book_index.find(id);
    if (target == nullptr) return false; // Không tìm thấy

    // Hủy mục lục trên Bảng băm trước
    book_index.remove(id);
    
    // Xóa node khỏi DSLK (Yêu cầu linked_list có hàm remove_node(node*))
    book_list.remove_node(target); 

    save_data(); // Lưu ngay xuống file
    return true;
}

// --- 5. CẬP NHẬT THÔNG TIN ---
bool book_service::update_book(std::string old_id, book new_data) {
    node<book>* target = book_index.find(old_id);
    if (target == nullptr) return false;

    std::string new_id = new_data.get_id();

    // Nếu Admin đổi Mã sách (ID)
    if (old_id != new_id) {
        // Kiểm tra xem ID mới có bị trùng với sách khác không
        if (book_index.find(new_id) != nullptr) return false; 
        
        book_index.remove(old_id);             // Xóa chỉ mục cũ
        target->set_data(new_data);            // Ghi đè dữ liệu mới
        book_index.insert(new_id, target);     // Cấp chỉ mục mới
    } else {
        // Chỉ sửa tên/tác giả, giữ nguyên mã ID
        target->set_data(new_data);
    }

    save_data(); // Lưu ngay xuống file
    return true;
}

// --- 6. TÌM KIẾM CHÍNH XÁC (Tốc độ O(1)) ---
node<book>* book_service::get_book_by_id(std::string id) {
    return book_index.find(id); // Dùng Bảng băm
}

// --- 7. TÌM KIẾM MỞ RỘNG (Tốc độ O(N)) ---
linked_list<book> book_service::search_books(std::string keyword) {
    linked_list<book> result;
    node<book>* curr = book_list.get_head();

    while (curr != nullptr) {
        book b = curr->get_data();
        
        // Gọi thuật toán xử lý chuỗi để tìm theo Tên, Tác giả, và Thể loại
        if (search_algo::contains(b.get_id(), keyword) ||
            search_algo::contains(b.get_title(), keyword) ||
            search_algo::contains(b.get_author(), keyword)) {
            
            result.push_back(b);
        }
        curr = curr->get_next();
    }
    return result;
}

linked_list<book> book_service::get_recommended_books(
    std::string author, std::string category, std::string exclude_book_id) {
    linked_list<book> result;

    // Ưu tiên sách cùng tác giả.
    node<book>* current = book_list.get_head();
    while (current != nullptr) {
        book item = current->get_data();
        if (item.get_id() != exclude_book_id &&
            !author.empty() && item.get_author() == author) {
            result.push_back(item);
        }
        current = current->get_next();
    }

    // Bổ sung sách cùng thể loại, không thêm trùng sách đã có.
    current = book_list.get_head();
    while (current != nullptr) {
        book item = current->get_data();
        bool already_added = false;

        node<book>* added = result.get_head();
        while (added != nullptr) {
            if (added->get_data().get_id() == item.get_id()) {
                already_added = true;
                break;
            }
            added = added->get_next();
        }

        if (item.get_id() != exclude_book_id &&
            !category.empty() &&
            item.get_category() == category &&
            !already_added) {
            result.push_back(item);
        }
        current = current->get_next();
    }

    return result;
}

// --- 8. SẮP XẾP BẰNG MERGE SORT ---
void book_service::sort_books(bool (*cmp)(book, book)) {
    node<book>* head_ptr = book_list.get_head();    

    sort_algo::merge_sort(&head_ptr, cmp);
    book_list.update_after_sort(head_ptr);
}

// --- Tách dòng từ book.txt (Định dạng: ID;Title;Author;Category;Quantity;Year) ---
void book_service::parse_line(std::string line, book& b) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }   
    std::stringstream ss(line);
    std::string id, title, author, category;
    std::string qty_str, year_str;

    // Đọc các trường dạng chuỗi phân cách bởi dấu ';'
    std::getline(ss, id, ';');
    std::getline(ss, title, ';');
    std::getline(ss, author, ';');
    std::getline(ss, category, ';');
    std::getline(ss, qty_str, ';');
    std::getline(ss, year_str, ';');

    // Chuyển đổi các trường số lượng và năm xuất bản từ string sang int an toàn
    int quantity = 0;
    int year = 0;
    try
    {
        if (!qty_str.empty())
            quantity = std::stoi(search_algo::trim(qty_str));
    }
    catch (...)
    {
        quantity = 0;
    }

    try
    {
        if (!year_str.empty())
            year = std::stoi(search_algo::trim(year_str));
    }
    catch (...)
    {
        year = 0;
    }
    // Khởi tạo đối tượng book hoàn chỉnh
    b = book(id, title, author, category, quantity, year);
}

linked_list<book> book_service::filter_books(std::string title_kw, std::string author_kw, std::string category_kw, int year) {
    linked_list<book> result;
    node<book>* curr = book_list.get_head();

    while (curr != nullptr) {
        book b = curr->get_data();
        bool match = true;

        if (!title_kw.empty() && !search_algo::contains(b.get_title(), title_kw)) match = false;
        if (!author_kw.empty() && !search_algo::contains(b.get_author(), author_kw)) match = false;
        if (!category_kw.empty() && !search_algo::contains(b.get_category(), category_kw)) match = false;
        if (year > 0 && b.get_year() != year) match = false;

        if (match) {
            result.push_back(b);
        }
        curr = curr->get_next();
    }
    return result;
}
