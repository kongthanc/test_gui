
#pragma once
#include "linked_list.h" // Táº­n dá»¥ng láº¡i chÃ­nh DSLK Ä‘Ã´i báº¡n Ä‘Ã£ code

template <typename t>
class graph {
private:
    int capacity;                 // Sá»©c chá»©a tá»‘i Ä‘a cá»§a Ä‘á»“ thá»‹
    int num_vertices;             // Sá»‘ lÆ°á»£ng Ä‘á»‰nh hiá»‡n táº¡i
    t* vertices;                  // Máº£ng lÆ°u dá»¯ liá»‡u cÃ¡c Ä‘á»‰nh (vd: mÃ£ sÃ¡ch, Ä‘á»‘i tÆ°á»£ng book)
    linked_list<int>* adj_list;   // Máº£ng cÃ¡c DSLK, má»—i DSLK lÆ°u chá»‰ sá»‘ (index) cá»§a cÃ¡c Ä‘á»‰nh ká»

public:
    graph(int cap = 100);
    ~graph();

    // Thao tÃ¡c xÃ¢y dá»±ng Ä‘á»“ thá»‹
    bool add_vertex(t data);
    bool add_edge(int src_index, int dest_index, bool is_directed = false);

    // Truy xuáº¥t thÃ´ng tin
    int get_num_vertices() const;
    t get_vertex_data(int index) const;
    
   // Láº¥y ra danh sÃ¡ch cÃ¡c Ä‘á»‰nh ká» cá»§a má»™t Ä‘á»‰nh
    linked_list<int>& get_adj_list(int index) const; 

    // CHÃˆN VÃ€O ÄÃ‚Y:
    linked_list<t> get_neighbors(t vertex_data) const;

    void clear();
};

#include "graph.cpp"
