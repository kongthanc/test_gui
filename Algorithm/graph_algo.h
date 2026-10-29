#pragma once
#include "../Structure/graph.h"
#include "../Structure/queue.h"
#include "../Structure/stack.h"
#include "../Structure/linked_list.h"

class graph_algo {
private:
    // Hàm phụ trợ: Kiểm tra xem một đỉnh đã được thăm hay chưa
    template <typename t>
    static bool is_visited(linked_list<t>& visited_list, t vertex) {
        node<t>* curr = visited_list.get_head();
        while (curr != nullptr) {
            // Yêu cầu đối tượng t (ví dụ: book) phải nạp chồng toán tử ==
            if (curr->get_data() == vertex) {
                return true; 
            }
            curr = curr->get_next();
        }
        return false;
    }

public:
    // 1. Thuật toán BFS (Breadth-First Search) - Gợi ý sách theo lớp
    template <typename t>
    static linked_list<t> BFS(graph<t>& g, t start_vertex) {
        linked_list<t> result;      // Lưu thứ tự duyệt (để làm list gợi ý)
        linked_list<t> visited;     // Đánh dấu các sách đã quét
        queue<t> q;                 // Hàng đợi

        // Bắt đầu từ đỉnh gốc
        q.push(start_vertex);
        visited.push_back(start_vertex);

        while (!q.is_empty()) {
            t current = q.front();  // Lấy ra phần tử đầu (lưu ý đổi thành phương thức tương ứng nếu queue của bạn tên khác, vd: peek)
            q.pop();

            result.push_back(current);

            // Lấy danh sách hàng xóm của đỉnh hiện tại (giả định graph có hàm get_neighbors)
            linked_list<t> neighbors = g.get_neighbors(current);
            node<t>* neighbor_node = neighbors.get_head();
            
            while (neighbor_node != nullptr) {
                t neighbor_data = neighbor_node->get_data();
                
                // Nếu hàng xóm chưa được thăm -> đánh dấu và đưa vào hàng đợi
                if (!is_visited(visited, neighbor_data)) {
                    visited.push_back(neighbor_data);
                    q.push(neighbor_data);
                }
                neighbor_node = neighbor_node->get_next();
            }
        }
        return result;
    }

    // 2. Thuật toán DFS (Depth-First Search) - Phân cụm nhóm sách
    template <typename t>
    static linked_list<t> DFS(graph<t>& g, t start_vertex) {
        linked_list<t> result;
        linked_list<t> visited;
        stack<t> s;                 // Ngăn xếp

        s.push(start_vertex);

        while (!s.empty()) {
            t current = s.peek();    // Lấy phần tử trên cùng
            s.pop();

            // Khác với BFS, DFS nhét vào stack rồi mới kiểm tra visited lúc lấy ra
            if (!is_visited(visited, current)) {
                visited.push_back(current);
                result.push_back(current);

                linked_list<t> neighbors = g.get_neighbors(current);
                node<t>* neighbor_node = neighbors.get_head();
                
                // Nhét toàn bộ hàng xóm chưa thăm vào ngăn xếp để đi sâu
                while (neighbor_node != nullptr) {
                    t neighbor_data = neighbor_node->get_data();
                    if (!is_visited(visited, neighbor_data)) {
                        s.push(neighbor_data);
                    }
                    neighbor_node = neighbor_node->get_next();
                }
            }
        }
        return result;
    }
};