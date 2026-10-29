
#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Model/user.h"
#include "file_helper.h"
#include "../Structure/linked_list.h"
#include "../Structure/hash_table.h"
#include "../Algorithm/search_algo.h"

class user_service {
private:
    linked_list<user> user_list;
    hash_table<user> user_index;
    std::string data_file;

    void parse_line(std::string line, user& u);

public:
    user_service(std::string filename = "user.txt");
    ~user_service();

    // 1. NhÃ³m thao tÃ¡c File (I/O)
    bool load_data();
    bool save_data();

    // 2. Nghiá»‡p vá»¥ XÃ¡c thá»±c & PhÃ¢n quyá»n (Há»— trá»£ Admin, Manager, User)
    user* login(std::string id, std::string password);
    bool check_permission_level(user* u, std::string required_role);

    // 3. NhÃ³m Nghiá»‡p vá»¥ CRUD
    bool add_user(user new_user);
    bool delete_user(std::string id);
    bool update_user(std::string old_id, user new_data);

    // 4. NhÃ³m Tra cá»©u
    node<user>* get_user_by_id(std::string id);
    void display_all();
};
