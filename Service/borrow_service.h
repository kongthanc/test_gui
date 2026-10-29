#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Model/borrow.h"
#include "file_helper.h"
#include "../Structure/linked_list.h"
#include "../Structure/hash_table.h"
#include "book_service.h" // Phối hợp để trừ/cộng số lượng sách trong kho
#include "user_service.h" // Phối hợp kiểm tra người dùng

class borrow_service {
private:
    linked_list<borrow> borrow_list;
    hash_table<borrow> borrow_index;
    std::string data_file;

    // Hàm phụ trợ tách chuỗi và xử lý định dạng ngày DD/MM/YYYY từ file txt
    void parse_line(std::string line, borrow& b);

public:
    borrow_service(std::string filename = "borrow.txt");
    ~borrow_service();

    // 1. Nhóm thao tác File (I/O)
    bool load_data();
    bool save_data();

    // 2. Nghiệp vụ cốt lõi: Mượn - Trả sách
    bool create_borrow_ticket(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date, book_service& b_service, user_service& u_service);
    bool return_book(std::string borrow_id, book_service& b_service);
    bool delete_borrow_ticket(std::string borrow_id); // Xóa phiếu mượn

    // 3. Nghiệp vụ kiểm tra tự động quá hạn (Tận dụng toán tử so sánh ngày tháng)
    void check_and_update_overdue(date current_date);

    // 4. Nhóm Tra cứu & Hiển thị
    node<borrow>* get_borrow_by_id(std::string id);
    linked_list<borrow> get_borrows_by_user(std::string user_id, bool active_only);
    linked_list<borrow> filter_borrows_by_date(date start_date, date end_date);
    void display_all();

    // 5. Ràng buộc toàn vẹn dữ liệu
    bool is_book_borrowed(std::string book_id) const;
    bool has_active_borrows(std::string user_id) const;
    bool is_user_borrowing_book(std::string user_id, std::string book_id) const;
    bool has_overdue_borrows(std::string user_id) const;
};
