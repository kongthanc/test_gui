
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Model/book.h"        // Class chá»©a thÃ´ng tin thá»±c thá»ƒ sÃ¡ch
#include "file_helper.h" // Ä‘á»‹nh dáº¡ng dá»¯ liá»‡u
#include "../Structure/linked_list.h" // Cáº¥u trÃºc lÆ°u trá»¯ chÃ­nh
#include "../Structure/hash_table.h"  // Báº£ng bÄƒm tra cá»©u nhanh
#include "../Algorithm/search_algo.h" // Thuáº­t toÃ¡n tÃ¬m kiáº¿m chuá»—i
#include "../Algorithm/sort_algo.h"        // Thuáº­t toÃ¡n sáº¯p xáº¿p

class book_service {
private:
    linked_list<book> book_list;
    hash_table<book> book_index;
    std::string data_file;

    // HÃ m phá»¥ trá»£ tÃ¡ch chuá»—i (split) Ä‘á»ƒ Ä‘á»c file txt
    void parse_line(std::string line, book& b);

public:
    book_service(std::string filename = "book.txt");
    ~book_service();

    // 1. NhÃ³m thao tÃ¡c File (I/O)
    bool load_data();
    bool save_data();

    // 2. NhÃ³m Nghiá»‡p vá»¥ CRUD
    bool add_book(book new_book);
    bool delete_book(std::string id);
    bool update_book(std::string old_id, book new_data);

    // 3. NhÃ³m Tra cá»©u & Cáº¥u trÃºc
    node<book>* get_book_by_id(std::string id);
    linked_list<book> search_books(std::string keyword);
    
    // HÃ m truyá»n con trá» hÃ m so sÃ¡nh Ä‘á»ƒ Sort (VD: cmp_by_year, cmp_by_title)
    void sort_books(bool (*cmp)(book, book)); 
    void display_all();

    // Lá»c káº¿t há»£p nhiá»u tiÃªu chÃ­ (truyá»n chuá»—i rá»—ng "" hoáº·c year = 0 náº¿u bá» qua tiÃªu chÃ­ Ä‘Ã³)
    linked_list<book> filter_books(std::string title_kw, std::string author_kw, std::string category_kw, int year = 0);
};
