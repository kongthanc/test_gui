
#include"hash_table.h"

// --- Khá»Ÿi táº¡o Node phá»¥ ---
template <typename t>
hash_table<t>::hash_node::hash_node(std::string k, node<t>* addr) {
    key = k;
    address = addr;
    next = nullptr;
}

// --- Khá»Ÿi táº¡o Báº£ng bÄƒm ---
template <typename t>
hash_table<t>::hash_table(int cap) {
    capacity = cap;
    current_size = 0;
    table = new hash_node*[capacity];
    for (int i = 0; i < capacity; i++) table[i] = nullptr;
}

// --- Há»§y Báº£ng bÄƒm ---
template <typename t>
hash_table<t>::~hash_table() {
    for (int i = 0; i < capacity; i++) {
        hash_node* curr = table[i];
        while (curr != nullptr) {
            hash_node* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
    delete[] table;
}

// --- Thuáº­t toÃ¡n BÄƒm ---
template <typename t>
int hash_table<t>::hash_function(std::string key) const {
    long long hash_val = 0;
    long long p = 31;
    long long p_pow = 1;
    
    for (unsigned char c : key) {
        hash_val = (hash_val + c * p_pow) % capacity;
        p_pow = (p_pow * p) % capacity;
    }
    return (int)((hash_val % capacity + capacity) % capacity);
}

// --- Thuáº­t toÃ¡n GiÃ£n ná»Ÿ tá»± Ä‘á»™ng ---
template <typename t>
void hash_table<t>::rehash() {
    int old_capacity = capacity;
    hash_node** old_table = table;

    capacity *= 2;
    table = new hash_node*[capacity];
    for (int i = 0; i < capacity; i++) table[i] = nullptr;
    
    current_size = 0;

    for (int i = 0; i < old_capacity; i++) {
        hash_node* curr = old_table[i];
        while (curr != nullptr) {
            insert(curr->key, curr->address); 
            hash_node* temp = curr;
            curr = curr->next;
            delete temp;
        }
    }
    delete[] old_table;
}

// --- ThÃªm Node ---
template <typename t>
void hash_table<t>::insert(std::string key, node<t>* address) {
    if ((float)(current_size + 1) / capacity > MAX_LOAD_FACTOR) {
        rehash();
    }

    int index = hash_function(key);
    
    hash_node* new_node = new hash_node(key, address);
    new_node->next = table[index];
    table[index] = new_node;
    
    current_size++;
}

// --- TÃ¬m kiáº¿m Node ---
template <typename t>
node<t>* hash_table<t>::find(std::string key) const {
    int index = hash_function(key);
    hash_node* curr = table[index];
    
    while (curr != nullptr) {
        if (curr->key == key) {
            return curr->address;
        }
        curr = curr->next;
    }
    return nullptr;
}

// --- XÃ³a Node ---
template <typename t>
void hash_table<t>::remove(std::string key) {
    int index = hash_function(key);
    hash_node* curr = table[index];
    hash_node* prev = nullptr;

    while (curr != nullptr) {
        if (curr->key == key) {
            if (prev == nullptr) {
                table[index] = curr->next;
            } else {
                prev->next = curr->next;
            }
            delete curr;
            current_size--;
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}
