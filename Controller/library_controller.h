#pragma once
#include <string>
#include <fstream>
#include <sstream>
#include <ctime>
#include "../Service/book_service.h"
#include "../Service/user_service.h"
#include "../Service/borrow_service.h"
#include "../Service/shift_service.h"
#include "../Structure/queue.h"

class library_controller {
private:
    struct book_location {
        std::string book_id;
        std::string category;
        std::string shelf;

        book_location() : book_id(""), category(""), shelf("") {}
        book_location(std::string id, std::string cat, std::string sh) 
            : book_id(id), category(cat), shelf(sh) {}
    };
    book_service b_service;
    user_service u_service;
    borrow_service br_service;
    shift_service s_service;
    
    // Hàng đợi lưu các yêu cầu đăng ký mượn sách của độc giả (FIFO)
    queue<borrow> book_request_queue;
    std::string request_file;

    user* current_logged_in_user;

    void load_requests();
    void save_requests();

    linked_list<book_location> position_list; // Lưu danh sách vị trí kệ
    std::string position_file;

    void load_positions();

public:
    library_controller();
    ~library_controller();

    // ================= 1. QUẢN LÝ PHIÊN & PHÂN QUYỀN =================
    bool login(std::string id, std::string password);
    void logout();
    user* get_current_user() const;
    std::string get_current_role() const;

    // ================= 2. NGHIỆP VỤ SÁCH (BOOK) =================
    linked_list<book> search_books(std::string keyword);
    linked_list<book> get_related_books(std::string category, std::string exclude_book_id);
    bool add_new_book(std::string id, std::string title, std::string author, std::string category, int qty, int year);
    bool update_book_info(std::string old_id, book book_n);
    bool delete_book(std::string id);
    node<book>* get_book_by_id(std::string id);
    void sort_books(bool (*cmp)(book, book));
    
    // Lọc phiếu mượn
    linked_list<borrow> filter_borrows_by_date(date start_date, date end_date);

    // ================= 3. NGHIỆP VỤ MƯỢN - TRẢ (BORROW) =================
    bool borrow_book(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date);
    bool return_book(std::string borrow_id);
    bool delete_borrow(std::string borrow_id);
    node<borrow>* get_borrow_by_id(std::string id);
    linked_list<borrow> get_my_borrows(std::string user_id, bool active_only);
    void check_system_overdue(); // Tự động cập nhật phiếu quá hạn theo ngày thực tế

    // ================= 4. HÀNG ĐỢI ĐĂNG KÝ SÁCH CỦA USER (QUEUE) =================
    bool register_book_request(std::string req_id, std::string book_id, std::string user_id, date b_date, date due_date);
    bool has_pending_requests() const;
    borrow peek_next_request() const;
    bool approve_next_request();
    bool reject_next_request();
    int get_pending_request_count() const;
    bool is_request_id_in_queue(std::string req_id);

    // ================= 5. NGHIỆP VỤ CA TRỰC (SHIFT) =================
    bool register_shift(date d, std::string session, std::string admin_id);
    bool cancel_shift(date d, std::string session, std::string admin_id);
    linked_list<shift> get_my_shifts(std::string admin_id);
    long long get_admin_salary(std::string admin_id, int month = 0, int year = 0);
    bool delete_shift(date d, std::string session);
    void show_all_shifts();

    // ================= 6. QUẢN LÝ NGƯỜI DÙNG (USER CRUD - MANAGER) =================
    bool add_new_user(std::string id, std::string name, std::string password, std::string role);
    bool update_user_info(std::string old_id, user updated_user);
    bool delete_user(std::string id);
    node<user>* get_user_by_id(std::string id);

    // ================= 7. VỊ TRÍ KỆ SÁCH =================
    std::string get_book_shelf(std::string book_id);
    linked_list<shift> get_all_shifts();
};
