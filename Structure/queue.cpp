
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
        // Náº¿u hÃ ng Ä‘á»£i rá»—ng, Ä‘áº§u vÃ  cuá»‘i Ä‘á»u trá» vÃ o node má»›i
        front_node = rear_node = new_node;
    } else {
        // ThÃªm vÃ o cuá»‘i hÃ ng Ä‘á»£i
        rear_node->set_next(new_node); // LiÃªn káº¿t pháº§n tá»­ cuá»‘i hiá»‡n táº¡i vá»›i node má»›i
        new_node->set_prev(rear_node); // Táº¡o liÃªn káº¿t ngÆ°á»£c vá» pháº§n tá»­ cuá»‘i cÅ© (nhá» node cÃ³ prev)
        rear_node = new_node;          // Cáº­p nháº­t láº¡i con trá» Ä‘uÃ´i
    }
    size++;
}

template <typename t>
void queue<t>::pop() {
    if (is_empty()) return;
    
    node<t>* temp = front_node;
    front_node = front_node->get_next(); // Dá»‹ch con trá» Ä‘áº§u sang pháº§n tá»­ káº¿ tiáº¿p
    
    if (front_node == nullptr) {
        // Náº¿u láº¥y pháº§n tá»­ cuá»‘i cÃ¹ng ra, hÃ ng Ä‘á»£i trá»Ÿ vá» rá»—ng
        rear_node = nullptr;
    } else {
        // Náº¿u váº«n cÃ²n pháº§n tá»­, cáº¯t Ä‘á»©t liÃªn káº¿t ngÆ°á»£c cá»§a pháº§n tá»­ Ä‘áº§u má»›i
        front_node->set_prev(nullptr);
    }
    
    delete temp; // Giáº£i phÃ³ng bá»™ nhá»› cá»§a máº¯t xÃ­ch vá»«a rÃºt
    size--;
}

template <typename t>
t queue<t>::front() const {
    if (is_empty()) return t(); // Tráº£ vá» giÃ¡ trá»‹ máº·c Ä‘á»‹nh cá»§a kiá»ƒu T náº¿u rá»—ng
    return front_node->get_data(); // Sá»­ dá»¥ng hÃ m get_data() cá»§a lá»›p node
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
