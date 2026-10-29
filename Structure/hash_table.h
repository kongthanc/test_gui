#pragma once
#include <string>
#include "node.h"

template <typename t>
class hash_table {
private:
    // Khai báo cấu trúc node phụ xử lý đụng độ
    struct hash_node {
        std::string key;
        node<t>* address;
        hash_node* next;
        
        hash_node(std::string k, node<t>* addr);
    };

    hash_node** table;
    int capacity;
    int current_size;
    const float MAX_LOAD_FACTOR = 0.75;

    int hash_function(std::string key) const;
    void rehash();

public:
    hash_table(int cap = 101);
    ~hash_table();

    void insert(std::string key, node<t>* address);
    node<t>* find(std::string key) const;
    void remove(std::string key);
};

#include "hash_table.cpp"