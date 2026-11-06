#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Model/borrow.h"
#include "file_helper.h"
#include "../Structure/linked_list.h"
#include "../Structure/hash_table.h"
#include "book_service.h" 
#include "user_service.h" 

class borrow_service {
private:
    linked_list<borrow> borrow_list;
    hash_table<borrow> borrow_index;
    std::string data_file;
    void parse_line(std::string line, borrow& b);

public:
    borrow_service(std::string filename = "borrow.txt");
    ~borrow_service();

    bool load_data();
    bool save_data();

    bool create_borrow_ticket(std::string borrow_id, std::string book_id, std::string user_id, date borrow_date, date due_date, book_service& b_service, user_service& u_service);
    bool return_book(std::string borrow_id, book_service& b_service);
    bool delete_borrow_ticket(std::string borrow_id); 

    // ĐÃ THÊM HÀM NÀY VÀO ĐÂY:
    void log_rejected_request(borrow b);

    void check_and_update_overdue(date current_date);
    node<borrow>* get_borrow_by_id(std::string id);
    linked_list<borrow> get_borrows_by_user(std::string user_id, bool active_only);
    linked_list<borrow> filter_borrows_by_date(date start_date, date end_date);
    void display_all();

    bool is_book_borrowed(std::string book_id) const;
    bool has_active_borrows(std::string user_id) const;
    bool is_user_borrowing_book(std::string user_id, std::string book_id) const;
    bool has_overdue_borrows(std::string user_id) const;

    int count_active_borrows_by_user(std::string user_id) const;
    int count_borrows_by_book(std::string book_id) const;
};