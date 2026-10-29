
#pragma once
#include "../Structure/graph.h"
#include "../Structure/queue.h"
#include "../Structure/stack.h"
#include "../Structure/linked_list.h"

class graph_algo {
private:
    // HÃ m phá»¥ trá»£: Kiá»ƒm tra xem má»™t Ä‘á»‰nh Ä‘Ã£ Ä‘Æ°á»£c thÄƒm hay chÆ°a
    template <typename t>
    static bool is_visited(linked_list<t>& visited_list, t vertex) {
        node<t>* curr = visited_list.get_head();
        while (curr != nullptr) {
            // YÃªu cáº§u Ä‘á»‘i tÆ°á»£ng t (vÃ­ dá»¥: book) pháº£i náº¡p chá»“ng toÃ¡n tá»­ ==
            if (curr->get_data() == vertex) {
                return true; 
            }
            curr = curr->get_next();
        }
        return false;
    }

public:
    // 1. Thuáº­t toÃ¡n BFS (Breadth-First Search) - Gá»£i Ã½ sÃ¡ch theo lá»›p
    template <typename t>
    static linked_list<t> BFS(graph<t>& g, t start_vertex) {
        linked_list<t> result;      // LÆ°u thá»© tá»± duyá»‡t (Ä‘á»ƒ lÃ m list gá»£i Ã½)
        linked_list<t> visited;     // ÄÃ¡nh dáº¥u cÃ¡c sÃ¡ch Ä‘Ã£ quÃ©t
        queue<t> q;                 // HÃ ng Ä‘á»£i

        // Báº¯t Ä‘áº§u tá»« Ä‘á»‰nh gá»‘c
        q.push(start_vertex);
        visited.push_back(start_vertex);

        while (!q.is_empty()) {
            t current = q.front();  // Láº¥y ra pháº§n tá»­ Ä‘áº§u (lÆ°u Ã½ Ä‘á»•i thÃ nh phÆ°Æ¡ng thá»©c tÆ°Æ¡ng á»©ng náº¿u queue cá»§a báº¡n tÃªn khÃ¡c, vd: peek)
            q.pop();

            result.push_back(current);

            // Láº¥y danh sÃ¡ch hÃ ng xÃ³m cá»§a Ä‘á»‰nh hiá»‡n táº¡i (giáº£ Ä‘á»‹nh graph cÃ³ hÃ m get_neighbors)
            linked_list<t> neighbors = g.get_neighbors(current);
            node<t>* neighbor_node = neighbors.get_head();
            
            while (neighbor_node != nullptr) {
                t neighbor_data = neighbor_node->get_data();
                
                // Náº¿u hÃ ng xÃ³m chÆ°a Ä‘Æ°á»£c thÄƒm -> Ä‘Ã¡nh dáº¥u vÃ  Ä‘Æ°a vÃ o hÃ ng Ä‘á»£i
                if (!is_visited(visited, neighbor_data)) {
                    visited.push_back(neighbor_data);
                    q.push(neighbor_data);
                }
                neighbor_node = neighbor_node->get_next();
            }
        }
        return result;
    }

    // 2. Thuáº­t toÃ¡n DFS (Depth-First Search) - PhÃ¢n cá»¥m nhÃ³m sÃ¡ch
    template <typename t>
    static linked_list<t> DFS(graph<t>& g, t start_vertex) {
        linked_list<t> result;
        linked_list<t> visited;
        stack<t> s;                 // NgÄƒn xáº¿p

        s.push(start_vertex);

        while (!s.empty()) {
            t current = s.peek();    // Láº¥y pháº§n tá»­ trÃªn cÃ¹ng
            s.pop();

            // KhÃ¡c vá»›i BFS, DFS nhÃ©t vÃ o stack rá»“i má»›i kiá»ƒm tra visited lÃºc láº¥y ra
            if (!is_visited(visited, current)) {
                visited.push_back(current);
                result.push_back(current);

                linked_list<t> neighbors = g.get_neighbors(current);
                node<t>* neighbor_node = neighbors.get_head();
                
                // NhÃ©t toÃ n bá»™ hÃ ng xÃ³m chÆ°a thÄƒm vÃ o ngÄƒn xáº¿p Ä‘á»ƒ Ä‘i sÃ¢u
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
