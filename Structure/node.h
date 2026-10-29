#pragma once

template <typename t>
class node {
private:
    t data;
    node<t>* next;
    node<t>* prev;

public:
    node();
    node(t data);

    t get_data() const;
    t& get_Data();
    node<t>* get_next() const;
    node<t>* get_prev() const;

    void set_data(t data);
    void set_next(node<t>* next);
    void set_prev(node<t>* prev);
    
};

#include "node.cpp"