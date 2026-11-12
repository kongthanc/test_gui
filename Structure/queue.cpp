
#include"queue.h"

template <typename t>
queue<t>::queue() {
    this->front_node = nullptr;
    this->rear_node = nullptr;
    this->size = 0;
}

template <typename t>
queue<t>::~queue() {
    clear();
}

template <typename t>
void queue<t>::push(t item) {
    node<t>* new_node = new node<t>(item);
    
    if (is_empty()) {
        front_node = rear_node = new_node;
    } else {
        rear_node->set_next(new_node);
        new_node->set_prev(rear_node); 
        rear_node = new_node;    
    }
    size++;
}

template <typename t>
void queue<t>::pop() {
    if (is_empty()) return;
    
    node<t>* temp = front_node;
    front_node = front_node->get_next(); 
    
    if (front_node == nullptr) {
        rear_node = nullptr;
    } else {
        front_node->set_prev(nullptr);
    }
    
    delete temp;
    size--;
}

template <typename t>
t queue<t>::front() const {
    if (is_empty()) return t(); 
    return front_node->get_data();
}

template <typename t>
bool queue<t>::is_empty() const {
    return front_node == nullptr;
}

template <typename t>
int queue<t>::get_size() const {
    return size;
}

template <typename t>
void queue<t>::clear() {
    while (!is_empty()) {
        pop();
    }
}
