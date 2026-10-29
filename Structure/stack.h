#pragma once

#include "node.h"

template <typename t>
class stack {
private:
    node<t>* top;
    int size;

public:
    stack();
    ~stack();

    bool empty() const;
    int get_size() const;

    void push(t data);
    void pop();

    t peek() const;

    void clear();
};
    
#include "stack.cpp"