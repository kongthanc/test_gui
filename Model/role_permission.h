
#pragma once
#include <string>
#include <iostream>

// INTERFACE TRá»ªU TÆ¯á»¢NG CHO PHÃ‚N QUYá»€N (STRATEGY PATTERN)
class role_permission {
public:
    virtual ~role_permission() {}

    // CÃ¡c hÃ m thuáº§n áº£o Ä‘a hÃ¬nh
    virtual std::string get_role_name() const = 0;
    virtual int get_max_menu_choice() const = 0;
    virtual bool can_manage_users() const = 0;
    virtual void show_menu(const std::string& user_name) const = 0;
};

// 1. Káº¾ THá»ªA: Quyá»n Manager (ToÃ n quyá»n)
class manager_permission : public role_permission {
public:
    std::string get_role_name() const override { return "manager"; }
    int get_max_menu_choice() const override { return 17; }
    bool can_manage_users() const override { return true; }

    void show_menu(const std::string& user_name) const override {
        std::cout << "\n==================== MANAGER MENU (" << user_name << " - TOAN QUYEN) ====================\n";
        std::cout << "--- QUAN LY SACH ---\n";
        std::cout << " 1. Tra cuu sach (Goi y sach lien quan)\n";
        std::cout << " 2. Them sach moi vao kho\n";
        std::cout << " 3. Sua thong tin sach\n";
        std::cout << " 4. Xoa sach khoi kho\n";
        std::cout << "--- QUAN LY MUON / TRA & DUYET HANG DOI ---\n";
        std::cout << " 5. Lap phieu muon sach truc tiep\n";
        std::cout << " 6. Xac nhan tra sach\n";
        std::cout << " 7. Xoa phieu muon\n";
        std::cout << " 8. Duyet yeu cau dang ky muon sach (Hang doi FIFO Queue)\n";
        std::cout << "--- QUAN LY CA TRUC & LUONG ---\n";
        std::cout << " 9. Xem toan bo lich truc thu vien\n";
        std::cout << "10. Dang ky ca truc cho nhan vien\n";
        std::cout << "11. Rut ten nhan vien khoi ca truc\n";
        std::cout << "12. Xoa toan bo mot ca truc\n";
        std::cout << "13. Tinh luong ca truc cua Admin (200.000 VND / buoi)\n";
        std::cout << "--- QUAN LY NGUOI DUNG ---\n";
        std::cout << "14. Them tai khoan moi (Admin / User / Manager)\n";
        std::cout << "15. Xoa tai khoan\n";
        std::cout << "16. Cap nhat tai khoan\n";
        std::cout << "17. Tra cuu vi tri ke cua sach theo Ma ID\n";
        std::cout << " 0. Dang xuat\n";
        std::cout << "====================================================================\n";
    }
};

// 2. Káº¾ THá»ªA: Quyá»n Admin (Quáº£n trá»‹ viÃªn)
class admin_permission : public role_permission {
public:
    std::string get_role_name() const override { return "admin"; }
    int get_max_menu_choice() const override { return 14; }
    bool can_manage_users() const override { return false; }

    void show_menu(const std::string& user_name) const override {
        std::cout << "\n================ ADMIN MENU (QUAN TRI VIEN: " << user_name << ") ================\n";
        std::cout << "--- QUAN LY SACH ---\n";
        std::cout << " 1. Tra cuu sach (Xem ca sach goi y cung the loai)\n";
        std::cout << " 2. Them sach moi vao kho\n";
        std::cout << " 3. Sua thong tin sach\n";
        std::cout << " 4. Xoa sach khoi kho\n";
        std::cout << "--- QUAN LY MUON / TRA & DUYET HANG DOI ---\n";
        std::cout << " 5. Lap phieu muon sach truc tiep\n";
        std::cout << " 6. Xac nhan tra sach (Hoan lai kho)\n";
        std::cout << " 7. Xoa phieu muon\n";
        std::cout << " 8. Duyet yeu cau dang ky muon sach tu Doc gia (Queue FIFO)\n";
        std::cout << " 9. Loc phieu muon theo ngay\n";
        std::cout << "--- CA TRUC & LUONG CA NHAN ---\n";
        std::cout << "10. Xem lich truc cua ban than\n";
        std::cout << "11. Dang ky ca truc cho ban than\n";
        std::cout << "12. Rut ten khoi ca truc cua ban than\n";
        std::cout << "13. Xem tien luong truc ca nhan (200.000 VND / buoi)\n";
        std::cout << "14. Tra cuu vi tri ke cua sach theo Ma ID\n";
        std::cout << " 0. Dang xuat\n";
        std::cout << "===========================================================================\n";
    }
};

// 3. Káº¾ THá»ªA: Quyá»n Äá»™c giáº£ (User)
class reader_permission : public role_permission {
public:
    std::string get_role_name() const override { return "user"; }
    int get_max_menu_choice() const override { return 5; }
    bool can_manage_users() const override { return false; }

    void show_menu(const std::string& user_name) const override {
        std::cout << "\n================ DOC GIA (USER: " << user_name << ") ================\n";
        std::cout << "1. Tim kiem sach & xem sach goi y cung the loai\n";
        std::cout << "2. Dang ky muon sach\n";
        std::cout << "3. Tra cuu vi tri ke sach de den lay\n";
        std::cout << "4. Xem sach dang muon\n";
        std::cout << "5. Xem lich su muon sach\n";
        std::cout << "0. Dang xuat\n";
        std::cout << "====================================================================\n";
    }
};

// FACTORY METHOD PATTERN: Khá»Ÿi táº¡o Ä‘á»‘i tÆ°á»£ng quyá»n háº¡n tÆ°Æ¡ng á»©ng
class permission_factory {
public:
    static role_permission* create(const std::string& role_str) {
        if (role_str == "manager") return new manager_permission();
        if (role_str == "admin")   return new admin_permission();
        return new reader_permission();
    }
};
