
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Model/borrow.h"
#include "file_helper.h"
#include "../Structure/linked_list.h"
#include "../Structure/hash_table.h"
#include "book_service.h" // Phá»‘i há»£p Ä‘á»ƒ trá»«/cá»™ng sá»‘ lÆ°á»£ng sÃ¡ch trong kho
#include "user_service.h" // Phá»‘i há»£p kiá»ƒm tra ngÆ°á»i dÃ¹ng

class borrow_service {
private:
    linked_list<borrow> borrow_list;
    hash_table<borrow> borrow_index;
    std::string data_file;

    // HÃ m phá»¥ trá»£ tÃ¡ch chuá»—i vÃ  xá»­ lÃ½ Ä‘á»‹nh dáº¡ng ngÃ y DD/MM/YYYY tá»« file txt
    void parse_line(std::string line, borrow& b);

public:
    borrow_service(std::string filename = "borrow.txt");
    ~borrow_service();

    // 1. NhÃ³m thao tÃ¡c File (I/O)
    bool load_data();
    bool save_data();

    // 2. Nghiá»‡p vá»¥ cá»‘t lÃµi: MÆ°á»£n - Tráº£ sÃ¡ch
    bool create_borrow_ticket(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date, book_service& b_service, user_service& u_service);
    bool return_book(std::string borrow_id, book_service& b_service);
    bool delete_borrow_ticket(std::string borrow_id); // XÃ³a phiáº¿u mÆ°á»£n

    // 3. Nghiá»‡p vá»¥ kiá»ƒm tra tá»± Ä‘á»™ng quÃ¡ háº¡n (Táº­n dá»¥ng toÃ¡n tá»­ so sÃ¡nh ngÃ y thÃ¡ng)
    void check_and_update_overdue(date current_date);

    // 4. NhÃ³m Tra cá»©u & Hiá»ƒn thá»‹
    node<borrow>* get_borrow_by_id(std::string id);
    linked_list<borrow> get_borrows_by_user(std::string user_id, bool active_only);
    linked_list<borrow> filter_borrows_by_date(date start_date, date end_date);
    void display_all();

    // 5. RÃ ng buá»™c toÃ n váº¹n dá»¯ liá»‡u
    bool is_book_borrowed(std::string book_id) const;
    bool has_active_borrows(std::string user_id) const;
    bool is_user_borrowing_book(std::string user_id, std::string book_id) const;
    bool has_overdue_borrows(std::string user_id) const;

    int count_active_borrows_by_user(std::string user_id) const;
    int count_borrows_by_book(std::string book_id) const;
};
