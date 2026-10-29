
#include "book_service.h"

book_service::book_service(std::string filename) {
    data_file = filename;
    load_data();
}

book_service::~book_service() {
    save_data(); // Tá»± Ä‘á»™ng lÆ°u file khi táº¯t chÆ°Æ¡ng trÃ¬nh
}

// --- 1. Äá»ŒC Dá»® LIá»†U Tá»ª FILE ---
bool book_service::load_data() {
    std::ifstream file(data_file);
    if (!file.is_open()) return false;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        
        book b;
        parse_line(line, b); // TÃ¡ch dÃ²ng thÃ nh Ä‘á»‘i tÆ°á»£ng book (tÃ¹y Ä‘á»‹nh dáº¡ng file cá»§a báº¡n)
        
        // Äáº©y vÃ o Danh sÃ¡ch liÃªn káº¿t
        book_list.push_back(b);
        
        // ÄÄƒng kÃ½ ngay Ä‘á»‹a chá»‰ node Ä‘Ã³ vÃ o Báº£ng bÄƒm
        node<book>* new_node = book_list.get_tail(); 
        book_index.insert(b.get_id(), new_node);
    }
    file.close();
    return true;
}

// --- 2. GHI Dá»® LIá»†U XUá»NG FILE ---
bool book_service::save_data() {
    return file_helper::save_list_to_file(data_file, book_list);
}

// --- 3. THÃŠM SÃCH ---
bool book_service::add_book(book new_book) {
    // Check O(1) xem mÃ£ sÃ¡ch Ä‘Ã£ tá»“n táº¡i chÆ°a
    if (book_index.find(new_book.get_id()) != nullptr) {
        return false; // BÃ¡o lá»—i trÃ¹ng ID
    }

    book_list.push_back(new_book);
    book_index.insert(new_book.get_id(), book_list.get_tail());

    save_data(); // LÆ°u ngay xuá»‘ng file
    return true;
}

// --- 4. XÃ“A SÃCH ---
bool book_service::delete_book(std::string id) {
    node<book>* target = book_index.find(id);
    if (target == nullptr) return false; // KhÃ´ng tÃ¬m tháº¥y

    // Há»§y má»¥c lá»¥c trÃªn Báº£ng bÄƒm trÆ°á»›c
    book_index.remove(id);
    
    // XÃ³a node khá»i DSLK (YÃªu cáº§u linked_list cÃ³ hÃ m remove_node(node*))
    book_list.remove_node(target); 

    save_data(); // LÆ°u ngay xuá»‘ng file
    return true;
}

// --- 5. Cáº¬P NHáº¬T THÃ”NG TIN (Äiá»ƒm nháº¡y cáº£m) ---
bool book_service::update_book(std::string old_id, book new_data) {
    node<book>* target = book_index.find(old_id);
    if (target == nullptr) return false;

    std::string new_id = new_data.get_id();

    // Náº¿u Admin Ä‘á»•i MÃ£ sÃ¡ch (ID)
    if (old_id != new_id) {
        // Kiá»ƒm tra xem ID má»›i cÃ³ bá»‹ trÃ¹ng vá»›i sÃ¡ch khÃ¡c khÃ´ng
        if (book_index.find(new_id) != nullptr) return false; 
        
        book_index.remove(old_id);             // XÃ³a chá»‰ má»¥c cÅ©
        target->set_data(new_data);            // Ghi Ä‘Ã¨ dá»¯ liá»‡u má»›i
        book_index.insert(new_id, target);     // Cáº¥p chá»‰ má»¥c má»›i
    } else {
        // Chá»‰ sá»­a tÃªn/tÃ¡c giáº£, giá»¯ nguyÃªn mÃ£ ID
        target->set_data(new_data);
    }

    save_data(); // LÆ°u ngay xuá»‘ng file
    return true;
}

// --- 6. TÃŒM KIáº¾M CHÃNH XÃC (Tá»‘c Ä‘á»™ O(1)) ---
node<book>* book_service::get_book_by_id(std::string id) {
    return book_index.find(id); // DÃ¹ng Báº£ng bÄƒm
}

// --- 7. TÃŒM KIáº¾M Má»ž Rá»˜NG (Tá»‘c Ä‘á»™ O(N)) ---
linked_list<book> book_service::search_books(std::string keyword) {
    linked_list<book> result;
    node<book>* curr = book_list.get_head();

    while (curr != nullptr) {
        book b = curr->get_data();
        
        // Gá»i thuáº­t toÃ¡n xá»­ lÃ½ chuá»—i Ä‘á»ƒ soi vÃ o TÃªn, TÃ¡c giáº£, vÃ  Thá»ƒ loáº¡i
        if (search_algo::contains(b.get_title(), keyword) || 
            search_algo::contains(b.get_author(), keyword) ||
            search_algo::contains(b.get_category(), keyword)) {
            
            result.push_back(b);
        }
        curr = curr->get_next();
    }
    return result;
}

// --- 8. Sáº®P Xáº¾P Báº°NG MERGE SORT ---
void book_service::sort_books(bool (*cmp)(book, book)) {
    node<book>* head_ptr = book_list.get_head();    

    sort_algo::merge_sort(&head_ptr, cmp);
    book_list.update_after_sort(head_ptr);
}

// --- TÃ¡ch dÃ²ng tá»« book.txt (Äá»‹nh dáº¡ng: ID;Title;Author;Category;Quantity;Year) ---
void book_service::parse_line(std::string line, book& b) {
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }   
    std::stringstream ss(line);
    std::string id, title, author, category;
    std::string qty_str, year_str;

    // Äá»c cÃ¡c trÆ°á»ng dáº¡ng chuá»—i phÃ¢n cÃ¡ch bá»Ÿi dáº¥u ';'
    std::getline(ss, id, ';');
    std::getline(ss, title, ';');
    std::getline(ss, author, ';');
    std::getline(ss, category, ';');
    std::getline(ss, qty_str, ';');
    std::getline(ss, year_str, ';');

    // Chuyá»ƒn Ä‘á»•i cÃ¡c trÆ°á»ng sá»‘ lÆ°á»£ng vÃ  nÄƒm xuáº¥t báº£n tá»« string sang int an toÃ n
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
    // Khá»Ÿi táº¡o Ä‘á»‘i tÆ°á»£ng book hoÃ n chá»‰nh
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
