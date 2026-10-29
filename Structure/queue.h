
#pragma once
#include "node.h"

template <typename t>
class queue {
private:
    node<t>* front_node; // Con trá» trá» Ä‘áº¿n Ä‘áº§u hÃ ng Ä‘á»£i
    node<t>* rear_node;  // Con trá» trá» Ä‘áº¿n cuá»‘i hÃ ng Ä‘á»£i
    int size;            // KÃ­ch thÆ°á»›c hiá»‡n táº¡i cá»§a hÃ ng Ä‘á»£i

public:
    queue();
    ~queue();

    queue(const queue<t>&) = delete;
    queue<t>& operator=(const queue<t>&) = delete;

    void push(t item);       // ThÃªm pháº§n tá»­ vÃ o cuá»‘i hÃ ng
    void pop();              // XÃ³a pháº§n tá»­ á»Ÿ Ä‘áº§u hÃ ng
    t front() const;         // Láº¥y giÃ¡ trá»‹ pháº§n tá»­ á»Ÿ Ä‘áº§u hÃ ng
    bool is_empty() const;   // Kiá»ƒm tra hÃ ng Ä‘á»£i rá»—ng
    int get_size() const;    // Láº¥y sá»‘ lÆ°á»£ng pháº§n tá»­
    void clear();            // XÃ³a sáº¡ch hÃ ng Ä‘á»£i
};
#include "queue.cpp"
