
#include "graph.h"

template <typename t>
graph<t>::graph(int cap) {
    this->capacity = cap;
    this->num_vertices = 0;
    
    // Khá»Ÿi táº¡o máº£ng Ä‘á»™ng chá»©a dá»¯ liá»‡u Ä‘á»‰nh
    this->vertices = new t[capacity];
    
    // Khá»Ÿi táº¡o máº£ng Ä‘á»™ng chá»©a cÃ¡c danh sÃ¡ch liÃªn káº¿t
    this->adj_list = new linked_list<int>[capacity]; 
}

template <typename t>
graph<t>::~graph() {
    clear();
}

template <typename t>
bool graph<t>::add_vertex(t data) {
    if (num_vertices >= capacity) return false; // Äá»“ thá»‹ Ä‘Ã£ Ä‘áº§y
    
    vertices[num_vertices] = data;
    num_vertices++;
    return true;
}

template <typename t>
bool graph<t>::add_edge(int src_index, int dest_index, bool is_directed) {
    // Kiá»ƒm tra tÃ­nh há»£p lá»‡ cá»§a index
    if (src_index < 0 || src_index >= num_vertices || 
        dest_index < 0 || dest_index >= num_vertices) {
        return false;
    }
    
    // DÃ¹ng hÃ m push_back cá»§a linked_list Ä‘á»ƒ thÃªm cáº¡nh ná»‘i
    adj_list[src_index].push_back(dest_index);
    
    // Náº¿u lÃ  Ä‘á»“ thá»‹ vÃ´ hÆ°á»›ng (mÆ°á»£n A cÃ³ B, thÃ¬ mÆ°á»£n B cÅ©ng cÃ³ A)
    if (!is_directed) {
        adj_list[dest_index].push_back(src_index);
    }
    
    return true;
}

template <typename t>
int graph<t>::get_num_vertices() const {
    return num_vertices;
}

template <typename t>
t graph<t>::get_vertex_data(int index) const {
    return vertices[index];
}

template <typename t>
linked_list<int>& graph<t>::get_adj_list(int index) const {
    return adj_list[index];
}

template <typename t>
linked_list<t> graph<t>::get_neighbors(t vertex_data) const {
    linked_list<t> neighbors;
    int src_index = -1;
    
    // TÃ¬m vá»‹ trÃ­ cá»§a Ä‘á»‰nh trong máº£ng vertices
    for (int i = 0; i < num_vertices; i++) {
        if (vertices[i] == vertex_data) {
            src_index = i;
            break;
        }
    }
    // Láº¥y toÃ n bá»™ dá»¯ liá»‡u Ä‘á»‰nh ká» Ä‘áº©y vÃ o danh sÃ¡ch káº¿t quáº£
    if (src_index != -1) {
        node<int>* curr = adj_list[src_index].get_head();
        while (curr != nullptr) {
            int neighbor_idx = curr->get_data();
            neighbors.push_back(vertices[neighbor_idx]);
            curr = curr->get_next();
        }
    }
    return neighbors;
}

template <typename t>
void graph<t>::clear() {
    if (vertices != nullptr) {
        delete[] vertices;
        vertices = nullptr;
    }
    if (adj_list != nullptr) {
        delete[] adj_list; // Gá»i destructor cá»§a linked_list Ä‘á»ƒ giáº£i phÃ³ng tá»«ng node
        adj_list = nullptr;
    }
    num_vertices = 0;
    capacity = 0;
}
