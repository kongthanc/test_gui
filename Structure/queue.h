
#pragma once
#include "node.h"

template <typename t>
class queue {
private:
    node<t>* front_node; 
    node<t>* rear_node; 
    int size;     

public:
    queue();
    ~queue();

    queue(const queue<t>&) = delete;
    queue<t>& operator=(const queue<t>&) = delete;

    void push(t item);     
    void pop();            
    t front() const;       
    bool is_empty() const;   
    int get_size() const;    
    void clear();            
};
#include "queue.cpp"
