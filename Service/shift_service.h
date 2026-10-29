#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "../Model/shift.h"
#include "file_helper.h"
#include "../Structure/linked_list.h"
#include "../Structure/hash_table.h"
#include "user_service.h" // Phối hợp kiểm tra danh tính Admin

class shift_service {
private:
    linked_list<shift> shift_list;
    hash_table<shift> shift_index;
    std::string data_file;

    // Hàm phụ trợ tạo khóa băm từ Ngày và Ca (VD: "01/10/2026_sang")
    std::string generate_key(date d, std::string session) const;
    void parse_line(std::string line, shift& s);

public:
    shift_service(std::string filename = "shift.txt");
    ~shift_service();

    // 1. Nhóm thao tác File (I/O)
    bool load_data();
    bool save_data();

    // 2. Nghiệp vụ cốt lõi: Đăng ký & Hủy ca trực
    bool register_shift(date shift_date, std::string session, std::string admin_id, user_service& u_service);
    bool cancel_shift(date shift_date, std::string session, std::string admin_id);

    // 3. Tra cứu ca trực
    node<shift>* get_shift(date shift_date, std::string session);
    void display_all();

    // 4. Các chức năng mở rộng (Tính lương, Xóa ca, Thống kê)
    bool delete_entire_shift(date shift_date, std::string session);
    int count_shifts_by_admin(std::string admin_id, int month = 0, int year = 0) const;
    long long calculate_salary(std::string admin_id, int month = 0, int year = 0, long long rate_per_shift = 200000) const;
    linked_list<shift> get_shifts_by_admin(std::string admin_id) const;
    linked_list<shift> get_all_shifts() const;
    bool has_shifts(std::string admin_id) const;

    std::string to_record_string() const;
};
