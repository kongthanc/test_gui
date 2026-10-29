#pragma once
#include "linked_list.h" // Tận dụng lại chính DSLK đôi bạn đã code

template <typename t>
class graph {
private:
    int capacity;                 // Sức chứa tối đa của đồ thị
    int num_vertices;             // Số lượng đỉnh hiện tại
    t* vertices;                  // Mảng lưu dữ liệu các đỉnh (vd: mã sách, đối tượng book)
    linked_list<int>* adj_list;   // Mảng các DSLK, mỗi DSLK lưu chỉ số (index) của các đỉnh kề

public:
    graph(int cap = 100);
    ~graph();

    // Thao tác xây dựng đồ thị
    bool add_vertex(t data);
    bool add_edge(int src_index, int dest_index, bool is_directed = false);

    // Truy xuất thông tin
    int get_num_vertices() const;
    t get_vertex_data(int index) const;
    
   // Lấy ra danh sách các đỉnh kề của một đỉnh
    linked_list<int>& get_adj_list(int index) const; 

    // CHÈN VÀO ĐÂY:
    linked_list<t> get_neighbors(t vertex_data) const;

    void clear();
};

#include "graph.cpp"