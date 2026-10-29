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
        // Nếu hàng đợi rỗng, đầu và cuối đều trỏ vào node mới
        front_node = rear_node = new_node;
    } else {
        // Thêm vào cuối hàng đợi
        rear_node->set_next(new_node); // Liên kết phần tử cuối hiện tại với node mới
        new_node->set_prev(rear_node); // Tạo liên kết ngược về phần tử cuối cũ (nhờ node có prev)
        rear_node = new_node;          // Cập nhật lại con trỏ đuôi
    }
    size++;
}

template <typename t>
void queue<t>::pop() {
    if (is_empty()) return;
    
    node<t>* temp = front_node;
    front_node = front_node->get_next(); // Dịch con trỏ đầu sang phần tử kế tiếp
    
    if (front_node == nullptr) {
        // Nếu lấy phần tử cuối cùng ra, hàng đợi trở về rỗng
        rear_node = nullptr;
    } else {
        // Nếu vẫn còn phần tử, cắt đứt liên kết ngược của phần tử đầu mới
        front_node->set_prev(nullptr);
    }
    
    delete temp; // Giải phóng bộ nhớ của mắt xích vừa rút
    size--;
}

template <typename t>
t queue<t>::front() const {
    if (is_empty()) return t(); // Trả về giá trị mặc định của kiểu T nếu rỗng
    return front_node->get_data(); // Sử dụng hàm get_data() của lớp node
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