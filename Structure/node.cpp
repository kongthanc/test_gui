#include "node.h"
template <typename t>
node<t>::node() {
    this->data = t();
    this->next = nullptr;
    this->prev = nullptr;
}

template <typename t>
node<t>::node(t data) {
    this->data = data;
    this->next = nullptr;
    this->prev = nullptr;
}

template <typename t>
t node<t>::get_data() const {
    return data;
}

template <typename t>
node<t>* node<t>::get_next() const {
    return next;
}

template <typename t>
node<t>* node<t>::get_prev() const {
    return prev;
}

template <typename t>
void node<t>::set_data(t data) {
    this->data = data;
}

template <typename t>
void node<t>::set_next(node<t>* next) {
    this->next = next;
}

template <typename t>
void node<t>::set_prev(node<t>* prev) {
    this->prev = prev;
}
template <typename t>
t& node<t>::get_Data()
{
    return data;
}