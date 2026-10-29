
#pragma once
#include "../Structure/node.h"

class sort_algo {
private:
    // 1. Giai Ä‘oáº¡n TÃ¡ch (Split): DÃ¹ng Thá» vÃ  RÃ¹a tÃ¬m Ä‘iá»ƒm giá»¯a
    template <typename t>
    static node<t>* split(node<t>* head) {
        node<t>* fast = head;
        node<t>* slow = head;

        // Thá» nháº£y 2 bÆ°á»›c, RÃ¹a nháº£y 1 bÆ°á»›c nhá» hÃ m get_next() cá»§a class node
        while (fast != nullptr && fast->get_next() != nullptr && fast->get_next()->get_next() != nullptr) {
            fast = fast->get_next()->get_next();
            slow = slow->get_next();
        }

        // Cáº¯t Ä‘á»©t liÃªn káº¿t Ä‘á»ƒ chia Ä‘Ã´i
        node<t>* second_half = slow->get_next();
        slow->set_next(nullptr);
        if (second_half != nullptr) {
            second_half->set_prev(nullptr);
        }
        
        return second_half;
    }

    // 2. Giai Ä‘oáº¡n Trá»™n (Merge): KhÃ¢u cÃ¡c node láº¡i theo tráº­t tá»± cá»§a hÃ m cmp
    template <typename t>
    static node<t>* merge(node<t>* first, node<t>* second, bool (*cmp)(t, t)) {
        // Náº¿u 1 trong 2 ná»­a rá»—ng, tráº£ vá» ná»­a cÃ²n láº¡i
        if (first == nullptr) return second;
        if (second == nullptr) return first;

        // So sÃ¡nh theo quy táº¯c (cmp) Ä‘Æ°á»£c truyá»n vÃ o
        if (cmp(first->get_data(), second->get_data())) {
            first->set_next(merge(first->get_next(), second, cmp));
            if (first->get_next() != nullptr) {
                first->get_next()->set_prev(first); 
            }
            first->set_prev(nullptr);
            return first;
        } else {
            second->set_next(merge(first, second->get_next(), cmp));
            if (second->get_next() != nullptr) {
                second->get_next()->set_prev(second);
            }
            second->set_prev(nullptr);
            return second;
        }
    }

public:
    // 3. HÃ m gá»i chÃ­nh: Äá»‡ quy chia Ä‘á»ƒ trá»‹
    // Nháº­n vÃ o tham chiáº¿u cá»§a con trá» head_ref Ä‘á»ƒ cáº­p nháº­t láº¡i danh sÃ¡ch gá»‘c
    template <typename t>
    static void merge_sort(node<t>** head_ref, bool (*cmp)(t, t)) {
        node<t>* head = *head_ref;
        
        // Äiá»u kiá»‡n dá»«ng: Danh sÃ¡ch rá»—ng hoáº·c chá»‰ cÃ³ 1 node
        if (head == nullptr || head->get_next() == nullptr) {
            return;
        }

        // Chia Ä‘Ã´i
        node<t>* second_half = split(head);

        // Äá»‡ quy sáº¯p xáº¿p 2 ná»­a
        merge_sort(&head, cmp);
        merge_sort(&second_half, cmp);

        // Trá»™n láº¡i vÃ  gÃ¡n vÃ o head_ref
        *head_ref = merge(head, second_half, cmp);
    }
};
