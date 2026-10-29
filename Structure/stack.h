
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

    stack(const stack<t>&) = delete;
    stack<t>& operator=(const stack<t>&) = delete;

    bool empty() const;
    int get_size() const;

    void push(t data);
    void pop();

    t peek() const;

    void clear();
};
    
#include "stack.cpp"
