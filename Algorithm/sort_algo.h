#pragma once
#include "../Structure/node.h"

class sort_algo {
private:
    // 1. Giai đoạn Tách (Split): Dùng Thỏ và Rùa tìm điểm giữa
    template <typename t>
    static node<t>* split(node<t>* head) {
        node<t>* fast = head;
        node<t>* slow = head;

        // Thỏ nhảy 2 bước, Rùa nhảy 1 bước nhờ hàm get_next() của class node
        while (fast != nullptr && fast->get_next() != nullptr && fast->get_next()->get_next() != nullptr) {
            fast = fast->get_next()->get_next();
            slow = slow->get_next();
        }

        // Cắt đứt liên kết để chia đôi
        node<t>* second_half = slow->get_next();
        slow->set_next(nullptr);
        if (second_half != nullptr) {
            second_half->set_prev(nullptr);
        }
        
        return second_half;
    }

    // 2. Giai đoạn Trộn (Merge): Khâu các node lại theo trật tự của hàm cmp
    template <typename t>
    static node<t>* merge(node<t>* first, node<t>* second, bool (*cmp)(t, t)) {
        // Nếu 1 trong 2 nửa rỗng, trả về nửa còn lại
        if (first == nullptr) return second;
        if (second == nullptr) return first;

        // So sánh theo quy tắc (cmp) được truyền vào
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
    // 3. Hàm gọi chính: Đệ quy chia để trị
    // Nhận vào tham chiếu của con trỏ head_ref để cập nhật lại danh sách gốc
    template <typename t>
    static void merge_sort(node<t>** head_ref, bool (*cmp)(t, t)) {
        node<t>* head = *head_ref;
        
        // Điều kiện dừng: Danh sách rỗng hoặc chỉ có 1 node
        if (head == nullptr || head->get_next() == nullptr) {
            return;
        }

        // Chia đôi
        node<t>* second_half = split(head);

        // Đệ quy sắp xếp 2 nửa
        merge_sort(&head, cmp);
        merge_sort(&second_half, cmp);

        // Trộn lại và gán vào head_ref
        *head_ref = merge(head, second_half, cmp);
    }
};