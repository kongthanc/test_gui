#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Model/book.h"        // Class chứa thông tin thực thể sách
#include "file_helper.h" // định dạng dữ liệu
#include "../Structure/linked_list.h" // Cấu trúc lưu trữ chính
#include "../Structure/hash_table.h"  // Bảng băm tra cứu nhanh
#include "../Algorithm/search_algo.h" // Thuật toán tìm kiếm chuỗi
#include "../Algorithm/sort_algo.h"        // Thuật toán sắp xếp

class book_service {
private:
    linked_list<book> book_list;
    hash_table<book> book_index;
    std::string data_file;

    // Hàm phụ trợ tách chuỗi (split) để đọc file txt
    void parse_line(std::string line, book& b);

public:
    book_service(std::string filename = "book.txt");
    ~book_service();

    // 1. Nhóm thao tác File (I/O)
    bool load_data();
    bool save_data();

    // 2. Nhóm Nghiệp vụ CRUD
    bool add_book(book new_book);
    bool delete_book(std::string id);
    bool update_book(std::string old_id, book new_data);

    // 3. Nhóm Tra cứu & Cấu trúc
    node<book>* get_book_by_id(std::string id);
    linked_list<book> search_books(std::string keyword);
    
    // Hàm truyền con trỏ hàm so sánh để Sort (VD: cmp_by_year, cmp_by_title)
    void sort_books(bool (*cmp)(book, book)); 
    void display_all();
};