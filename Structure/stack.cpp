#include "stack.h"

template <typename t>
stack<t>::stack() {
    top = nullptr;
    size = 0;
}

template <typename t>
stack<t>::~stack() {
    clear();
}

template <typename t>
bool stack<t>::empty() const {
    return top == nullptr;
}

template <typename t>
int stack<t>::get_size() const {
    return size;
}

template <typename t>
void stack<t>::push(t data) {
    node<t>* new_node = new node<t>(data);

    new_node->set_next(top);
    top = new_node;

    size++;
}

template <typename t>
void stack<t>::pop() {
    if (empty())
        return;

    node<t>* temp = top;

    top = top->get_next();

    delete temp;
    size--;
}

template <typename t>
t stack<t>::peek() const {
    if (empty())
        return t();

    return top->get_data();
}

template <typename t>
void stack<t>::clear() {
    while (!empty())
        pop();
}