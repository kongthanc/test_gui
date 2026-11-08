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
#include "../Model/action_log.h"
#include "../Structure/stack.h" 
#include "../Structure/graph.h"
#include "../Algorithm/graph_algo.h"
#include "../Service/notification_service.h"

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
    notification_service n_service;
    
    queue<borrow> book_request_queue;
    std::string request_file;
    user* current_logged_in_user;

    void load_requests();
    void save_requests();

    linked_list<book_location> position_list; 
    std::string position_file;

    void load_positions();
    void save_positions(); 

    stack<action_log<book>> book_undo_stack;
    stack<action_log<user>> user_undo_stack;
    stack<action_log<borrow>> borrow_undo_stack;

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
    // Hàm gộp duy nhất: Sử dụng Graph BFS bên trong
    linked_list<book> get_recommended_books(std::string author, std::string category, std::string exclude_book_id);
    std::string get_hot_category();
    int get_user_active_borrow_count(std::string user_id) const;
    
    // Đã thêm tham số shelf mặc định
    bool add_new_book(std::string id, std::string title, std::string author, std::string category, int qty, int year, std::string shelf = "Chua xep ke");
    bool update_book_info(std::string old_id, book book_n);
    bool delete_book(std::string id);
    node<book>* get_book_by_id(std::string id);
    bool has_pending_request_for_book(std::string book_id);
    bool is_book_borrowed(std::string book_id) const;
    void sort_books(bool (*cmp)(book, book));
    
    // Lọc phiếu mượn
    linked_list<borrow> filter_borrows_by_date(date start_date, date end_date);

    // ================= 3. NGHIỆP VỤ MƯỢN - TRẢ (BORROW) =================
    bool borrow_book(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date);
    bool return_book(std::string borrow_id);
    bool delete_borrow(std::string borrow_id);
    node<borrow>* get_borrow_by_id(std::string id);
    linked_list<borrow> get_my_borrows(std::string user_id, bool active_only);
    void check_system_overdue(); 

    // ================= 4. HÀNG ĐỢI ĐĂNG KÝ (QUEUE) =================
    std::string generate_request_id();
    bool register_book_request(std::string book_id, std::string user_id, date b_date, date due_date, std::string& out_message);
    bool has_pending_requests() const;
    borrow peek_next_request() const;
    bool approve_next_request();
    bool reject_next_request(std::string reason = "");
    int get_pending_request_count() const;
    linked_list<borrow> get_my_pending_requests(std::string user_id);
    bool is_request_id_in_queue(std::string req_id);

    // ================= 5. NGHIỆP VỤ CA TRỰC (SHIFT) =================
    bool register_shift(date d, std::string session, std::string admin_id);
    bool cancel_shift(date d, std::string session, std::string admin_id);
    linked_list<shift> get_my_shifts(std::string admin_id);
    long long get_admin_salary(std::string admin_id, int month = 0, int year = 0);
    bool delete_shift(date d, std::string session);
    void show_all_shifts();
    linked_list<shift> get_all_shifts();

    // ================= 6. QUẢN LÝ NGƯỜI DÙNG (USER CRUD) =================
    bool add_new_user(std::string id, std::string name, std::string password, std::string role);
    bool update_user_info(std::string old_id, user updated_user);
    bool delete_user(std::string id);
    node<user>* get_user_by_id(std::string id);
    bool has_pending_request_for_user(std::string user_id);
    bool has_user_shifts(std::string user_id) const;

    // ================= 7. VỊ TRÍ KỆ SÁCH =================
    std::string get_book_shelf(std::string book_id);

    // ================= 8. HOÀN TÁC ĐA ĐỐI TƯỢNG (UNDO STACK) =================
    bool can_undo_book() const;
    bool undo_last_book_action(std::string& out_message);
    bool undo_book_action(std::string& out_message);

    bool can_undo_user() const;
    bool undo_user_action(std::string& out_message);

    bool can_undo_borrow() const;
    bool undo_borrow_action(std::string& out_message);

    bool has_overdue_borrows(std::string user_id) const;
    bool has_active_borrows(std::string user_id) const;

    // ================= 9. THÔNG BÁO =================
    std::vector<notification> get_my_notifications(std::string user_id) const;
    int get_unread_notification_count(std::string user_id) const;
    bool mark_notification_read(std::string notification_id);
};