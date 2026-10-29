#pragma once

#include "node.h"

template <typename t>
class linked_list {
private:
    node<t>* head;
    node<t>* tail;
    int size;

public:
    linked_list();
    linked_list(const linked_list<t>& other); //tạo danh sách mới bằng cách sao chép dữ liệu từ danh sách khác.
    linked_list<t>& operator=(const linked_list<t>& other); //sao chép dữ liệu khi gán một danh sách có sẵn cho danh sách khác.
    ~linked_list();

    bool empty() const;
    int get_size() const;

    node<t>* get_head() const;
    node<t>* get_tail() const;

    void push_front(t data);
    void push_back(t data);

    void pop_front();
    void pop_back();

    void insert(int position, t data);
    void remove(int position);

    void clear();

    void set_head(node<t>* new_head);
    void set_tail(node<t>* new_tail);

    void update_after_sort(node<t>* new_head);
    void remove_node(node<t>* target);
};

#include "linked_list.cpp"