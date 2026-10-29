
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
#include "../Structure/stack.h" // Stack tá»± cháº¿ cá»§a báº¡n

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
    
    // HÃ ng Ä‘á»£i lÆ°u cÃ¡c yÃªu cáº§u Ä‘Äƒng kÃ½ mÆ°á»£n sÃ¡ch cá»§a Ä‘á»™c giáº£ (FIFO)
    queue<borrow> book_request_queue;
    std::string request_file;

    user* current_logged_in_user;

    void load_requests();
    void save_requests();

    linked_list<book_location> position_list; // LÆ°u danh sÃ¡ch vá»‹ trÃ­ ká»‡
    std::string position_file;

    void load_positions();
    stack<action_log<book>> book_undo_stack;
    stack<action_log<user>> user_undo_stack;
    stack<action_log<borrow>> borrow_undo_stack;

public:
    library_controller();
    ~library_controller();

    // ================= 1. QUáº¢N LÃ PHIÃŠN & PHÃ‚N QUYá»€N =================
    bool login(std::string id, std::string password);
    void logout();
    user* get_current_user() const;
    std::string get_current_role() const;

    // ================= 2. NGHIá»†P Vá»¤ SÃCH (BOOK) =================
    linked_list<book> search_books(std::string keyword);
    linked_list<book> get_related_books(std::string category, std::string exclude_book_id);
    bool add_new_book(std::string id, std::string title, std::string author, std::string category, int qty, int year);
    bool update_book_info(std::string old_id, book book_n);
    bool delete_book(std::string id);
    node<book>* get_book_by_id(std::string id);
    void sort_books(bool (*cmp)(book, book));
    
    // Lá»c phiáº¿u mÆ°á»£n
    linked_list<borrow> filter_borrows_by_date(date start_date, date end_date);

    // ================= 3. NGHIá»†P Vá»¤ MÆ¯á»¢N - TRáº¢ (BORROW) =================
    bool borrow_book(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date);
    bool return_book(std::string borrow_id);
    bool delete_borrow(std::string borrow_id);
    node<borrow>* get_borrow_by_id(std::string id);
    linked_list<borrow> get_my_borrows(std::string user_id, bool active_only);
    void check_system_overdue(); // Tá»± Ä‘á»™ng cáº­p nháº­t phiáº¿u quÃ¡ háº¡n theo ngÃ y thá»±c táº¿

    // ================= 4. HÃ€NG Äá»¢I ÄÄ‚NG KÃ SÃCH Cá»¦A USER (QUEUE) =================
    bool register_book_request(std::string req_id, std::string book_id, std::string user_id, date b_date, date due_date);
    bool has_pending_requests() const;
    borrow peek_next_request() const;
    bool approve_next_request();
    bool reject_next_request();
    int get_pending_request_count() const;
    bool is_request_id_in_queue(std::string req_id);

    // ================= 5. NGHIá»†P Vá»¤ CA TRá»°C (SHIFT) =================
    bool register_shift(date d, std::string session, std::string admin_id);
    bool cancel_shift(date d, std::string session, std::string admin_id);
    linked_list<shift> get_my_shifts(std::string admin_id);
    long long get_admin_salary(std::string admin_id, int month = 0, int year = 0);
    bool delete_shift(date d, std::string session);
    void show_all_shifts();
    linked_list<shift> get_all_shifts();

    // ================= 6. QUáº¢N LÃ NGÆ¯á»œI DÃ™NG (USER CRUD - MANAGER) =================
    bool add_new_user(std::string id, std::string name, std::string password, std::string role);
    bool update_user_info(std::string old_id, user updated_user);
    bool delete_user(std::string id);
    node<user>* get_user_by_id(std::string id);

    // ================= 7. Vá»Š TRÃ Ká»† SÃCH =================
    std::string get_book_shelf(std::string book_id);

    // ================= 8. HOÃ€N TÃC ÄA Äá»I TÆ¯á»¢NG (TEMPLATE STACK UNDO) =================
    bool can_undo_book() const;
    bool undo_last_book_action(std::string& out_message);
    bool undo_book_action(std::string& out_message); // TÃªn gá»i tÆ°Æ¡ng Ä‘Æ°Æ¡ng

    bool can_undo_user() const;
    bool undo_user_action(std::string& out_message);

    bool can_undo_borrow() const;
    bool undo_borrow_action(std::string& out_message);

    bool has_overdue_borrows(std::string user_id) const;
};
