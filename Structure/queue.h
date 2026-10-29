#pragma once
#include "node.h"

template <typename t>
class queue {
private:
    node<t>* front_node; // Con trỏ trỏ đến đầu hàng đợi
    node<t>* rear_node;  // Con trỏ trỏ đến cuối hàng đợi
    int size;            // Kích thước hiện tại của hàng đợi

public:
    queue();
    ~queue();

    void push(t item);       // Thêm phần tử vào cuối hàng
    void pop();              // Xóa phần tử ở đầu hàng
    t front() const;         // Lấy giá trị phần tử ở đầu hàng
    bool is_empty() const;   // Kiểm tra hàng đợi rỗng
    int get_size() const;    // Lấy số lượng phần tử
    void clear();            // Xóa sạch hàng đợi
};
#include "queue.cpp"