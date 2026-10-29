#include <iostream>
#include <string>
#include <limits>
#include "Controller/library_controller.h"

using namespace std;

// ================= CÁC HÀM NHẬP LIỆU CHỐNG TREO & RÀNG BUỘC TẠI CHỖ =================
int input_int(string prompt, int min_val = 0, int max_val = 2147483647) {
    int value;
    while (true) {
        cout << prompt;
        if (cin >> value && value >= min_val && value <= max_val) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return value;
        }
        cout << "[Loi] Nhap sai! Phai la so tu " << min_val << " den " << max_val << ". Vui long nhap lai.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

string input_string(string prompt) {
    string str;
    while (true) {
        cout << prompt;
        getline(cin >> ws, str);
        if (!str.empty()) return str;
        cout << "[Loi] Khong duoc de trong! Vui long nhap lai.\n";
    }
}

string input_session(string prompt) {
    string session;
    while (true) {
        cout << prompt;
        cin >> session;
        if (session == "sang" || session == "chieu") {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return session;
        }
        cout << "[Loi] Ca truc chi duoc la 'sang' hoac 'chieu'! Vui long nhap lai.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

date input_valid_date(string prompt) {
    while (true) {
        cout << prompt;
        int d, m, y;
        if (cin >> d >> m >> y) {
            date dt(d, m, y);
            if (dt.is_valid()) {
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                return dt;
            }
        }
        cout << "[Loi] Ngay khong hop le hoac khong ton tai! Vui long nhap lai.\n";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }
}

int input_borrow_duration() {
    cout << "\n--- CHON THOI HAN MUON ---\n";
    cout << "1. 1 tuan (7 ngay)\n";
    cout << "2. 2 tuan (14 ngay)\n";
    cout << "3. 1 thang (30 ngay)\n";

    int choice = input_int("Lua chon: ", 1, 3);

    if (choice == 1) return 7;
    if (choice == 2) return 14;
    return 30;
}

// ================= HÀM XÁC NHẬN HÀNH ĐỘNG (CONFIRMATION PROMPT) =================
bool confirm_action(string message) {
    cout << "\n[XAC NHAN] " << message << " (1: Dong y | 0: Huy bo): ";
    int choice = input_int("", 0, 1);
    return (choice == 1);
}

// ================= GIAO DIỆN MENU PHÂN QUYỀN =================
void show_manager_menu() {
    cout << "\n==================== MANAGER MENU (TOAN QUYEN) ====================\n";
    cout << "--- QUAN LY SACH ---\n";
    cout << " 1. Tra cuu sach (Goi y sach lien quan)\n";
    cout << " 2. Them sach moi vao kho\n";
    cout << " 3. Sua thong tin sach\n";
    cout << " 4. Xoa sach khoi kho\n";
    cout << "--- QUAN LY MUON / TRA & DUYET HANG DOI ---\n";
    cout << " 5. Lap phieu muon sach truc tiep\n";
    cout << " 6. Xac nhan tra sach\n";
    cout << " 7. Xoa phieu muon (chi xoa phieu da tra xong)\n";
    cout << " 8. Duyet yeu cau dang ky muon sach (Hang doi FIFO Queue)\n";
    cout << "--- QUAN LY CA TRUC & LUONG ---\n";
    cout << " 9. Xem toan bo lich truc thu vien\n";
    cout << "10. Dang ky ca truc cho nhan vien\n";
    cout << "11. Rut ten nhan vien khoi ca truc\n";
    cout << "12. Xoa toan bo mot ca truc\n";
    cout << "13. Tinh luong ca truc cua Admin (200.000 VND / buoi)\n";
    cout << "--- QUAN LY NGUOI DUNG ---\n";
    cout << "14. Them tai khoan moi (Admin / User / Manager)\n";
    cout << "15. Xoa tai khoan\n";
    cout << "16. Cap nhat tai khoan\n";
    cout << "17. Tra cuu vi tri ke cua sach theo Ma ID\n";
    cout << " 0. Dang xuat\n";
    cout << "====================================================================\n";
}

void show_admin_menu(string admin_name) {
    cout << "\n================ ADMIN MENU (QUAN TRI VIEN: " << admin_name << ") ================\n";
    cout << "--- QUAN LY SACH ---\n";
    cout << " 1. Tra cuu sach (Xem ca sach goi y cung the loai)\n";
    cout << " 2. Them sach moi vao kho\n";
    cout << " 3. Sua thong tin sach\n";
    cout << " 4. Xoa sach khoi kho\n";
    cout << "--- QUAN LY MUON / TRA & DUYET HANG DOI ---\n";
    cout << " 5. Lap phieu muon sach truc tiep\n";
    cout << " 6. Xac nhan tra sach (Hoan lai kho)\n";
    cout << " 7. Xoa phieu muon (chi xoa phieu da tra xong)\n";
    cout << " 8. Duyet yeu cau dang ky muon sach tu Doc gia (Queue FIFO)\n";
    cout << " 9. Loc phieu muon theo ngay\n";
    cout << "--- CA TRUC & LUONG CA NHAN ---\n";
    cout << "10. Xem lich truc cua ban than\n";
    cout << "11. Dang ky ca truc cho ban than\n";
    cout << "12. Rut ten khoi ca truc cua ban than\n";
    cout << "13. Xem tien luong truc ca nhan (200.000 VND / buoi)\n";
    cout << "14. Tra cuu vi tri ke cua sach theo Ma ID\n";
    cout << " 0. Dang xuat\n";
    cout << "===========================================================================\n";
}

void show_user_menu(string user_name) {
    cout << "\n================ DOC GIA (USER: " << user_name << ") ================\n";
    cout << "1. Tim kiem sach & xem sach goi y cung the loai\n";
    cout << "2. Dang ky muon sach (Hang doi Queue)\n";
    cout << "3. Xem sach dang muon\n";
    cout << "4. Xem lich su muon sach\n";
    cout << "5. Tra cuu vi tri ke sach de den lay\n";
    cout << "0. Dang xuat\n";
    cout << "====================================================================\n";
}

// ================= CHƯƠNG TRÌNH CHÍNH =================
int main() {
    library_controller controller;

    cout << "=====================================================\n";
    cout << "       HE THONG QUAN LY THU VIEN PHAN QUYEN         \n";
    cout << "=====================================================\n";

    while (true) {
        cout << "\n-------------------- DANG NHAP --------------------\n";
        string id = input_string("Nhap Ma tai khoan (ID) [Go 'thoat' de tat chuong trinh]: ");
        if (id == "thoat" || id == "exit") {
            if (confirm_action("Ban co chac chan muon dong ung dung va thoat he thong?")) {
                cout << "Dang dong he thong va luu toan bo du lieu. Tam biet!\n";
                break;
            } else {
                cout << "[Thong bao] Da huy thoat chuong trinh.\n";
                continue;
            }
        }

        string password = input_string("Nhap Mat khau: ");

        if (controller.login(id, password)) {
            user* u = controller.get_current_user();
            string role = controller.get_current_role();
            cout << "\n[Thanh cong] Dang nhap voi vai tro: " << role 
                 << " | Xin chao: " << u->get_name() << "!\n";

            bool is_logged_in = true;

            // ================= VÒNG LẶP CHO MANAGER =================
            if (role == "manager") {
                while (is_logged_in) {
                    show_manager_menu();
                    int choice = input_int("Chon chuc nang (0 - 17): ", 0, 17);

                    if (choice == 1) {
                        string keyword = input_string("Nhap tu khoa tim kiem (Ten/Tac gia/The loai): ");
                        linked_list<book> results = controller.search_books(keyword);
                        cout << "\n--- KET QUA TIM KIEM ---\n";
                        node<book>* curr = results.get_head();
                        string found_cat = "";
                        string found_id = "";
                        if (curr == nullptr) {
                            cout << "Khong tim thay sach phu hop!\n";
                        }
                        while (curr != nullptr) {
                            book b = curr->get_data();
                            if (found_cat.empty()) {
                                found_cat = b.get_category();
                                found_id = b.get_id();
                            }
                            cout << "- [" << b.get_id() << "] " << b.get_title() 
                                    << " | The loai: " << b.get_category() 
                                    << " | Tac gia: " << b.get_author() 
                                    << " | Ton kho: " << b.get_quantity() 
                                    << " | Vi tri: Ke " << controller.get_book_shelf(b.get_id()) << "\n";
                            curr = curr->get_next();
                        }
                        // Gợi ý sách liên quan
                        if (!found_cat.empty()) {
                            linked_list<book> related = controller.get_related_books(found_cat, found_id);
                            if (!related.empty()) {
                                cout << "\n--- SACH GOI Y LIEN QUAN (Cung the loai: " << found_cat << ") ---\n";
                                node<book>* rel_curr = related.get_head();
                                while (rel_curr != nullptr) {
                                    book rb = rel_curr->get_data();
                                    cout << " * [" << rb.get_id() << "] " << rb.get_title() 
                                         << " | Tac gia: " << rb.get_author() 
                                         << " | Ton kho: " << rb.get_quantity() << "\n";
                                    rel_curr = rel_curr->get_next();
                                }
                            }
                        }
                    }
                    else if (choice == 2) {
                        string b_id = input_string("Nhap Ma sach (ID): ");
                        string title = input_string("Nhap ten sach: ");
                        string author = input_string("Nhap tac gia: ");
                        string cat = input_string("Nhap the loai: ");
                        int qty = input_int("So luong ton kho: ", 0, 10000);
                        int year = input_int("Nam xuat ban: ", 1000, 2026);
                        if (controller.add_new_book(b_id, title, author, cat, qty, year))
                            cout << "[Thanh cong] Da them sach moi!\n";
                        else
                            cout << "[Loi] Trung ma sach hoac khong the them!\n";
                    }
                    else if (choice == 3) {
                        string old_id = input_string("Nhap Ma sach cu can sua: ");
                        cout << "--- Nhap thong tin moi cho sach ---\n";
                        book new_b;
                        cin >> new_b;
                        if (controller.update_book_info(old_id, new_b))
                            cout << "[Thanh cong] Da cap nhat sach!\n";
                        else
                            cout << "[Loi] Cap nhat that bai! (Khong tim thay ma cu, ma moi bi trung, hoac sach dang co nguoi muon nen khong the doi ma ID)\n";
                    }
                    else if (choice == 4) {
                        string b_id = input_string("Nhap Ma sach can xoa: ");
                        node<book>* b_node = controller.get_book_by_id(b_id);
                        if (b_node == nullptr) {
                            cout << "[Loi] Khong tim thay ma sach trong he thong!\n";
                        } else {
                            book b = b_node->get_data();
                            cout << "=> Sach can xoa: [" << b.get_id() << "] " << b.get_title() << " (Tac gia: " << b.get_author() << ")\n";
                            if (confirm_action("Ban co chac chan muon xoa sach nay khoi kho?")) {
                                if (controller.delete_book(b_id))
                                    cout << "[Thanh cong] Da xoa sach khoi he thong!\n";
                                else
                                    cout << "[Loi] Khong the xoa! Sach DANG CO NGUOI MUON chua tra!\n";
                            } else {
                                cout << "[Huy bo] Da huy thao tac xoa sach.\n";
                            }
                        }
                    }
                    else if (choice == 5) {
                        string br_id = input_string("Nhap Ma phieu muon: ");
                        string b_id = input_string("Nhap Ma sach: ");
                        string u_id = input_string("Nhap Ma doc gia: ");
                        date b_date = input_valid_date("Ngay muon (DD MM YYYY): ");
                        int days = input_borrow_duration();
                        date due_date = b_date.add_days(days);
                        cout << "Ngay hen tra du kien: " << due_date << "\n";
                        if (controller.borrow_book(br_id, b_id, u_id, b_date, due_date))
                            cout << "[Thanh cong] Lap phieu muon thanh cong!\n";
                        else
                            cout << "[Loi] Trung ma phieu, sai thong tin, het sach, ban doc da muon cuon nay hoac dang no sach qua han!\n";
                    }
                    else if (choice == 6) {
                        string br_id = input_string("Nhap Ma phieu muon can tra: ");
                        node<borrow>* br_node = controller.get_borrow_by_id(br_id);
                        if (br_node == nullptr) {
                            cout << "[Loi] Khong tim thay ma phieu muon nay trong he thong!\n";
                        } else {
                            borrow ticket = br_node->get_data();
                            if (ticket.get_status() == "Da tra") {
                                cout << "[Thong bao] Phieu muon nay da duoc tra tu truoc roi!\n";
                            } else {
                                cout << "=> Thong tin phieu: Ma sach: " << ticket.get_book_id() 
                                     << " | Doc gia: " << ticket.get_user_id() 
                                     << " | Han tra: " << ticket.get_due_date() 
                                     << " | Trang thai: " << ticket.get_status() << "\n";
                                
                                if (ticket.get_status() == "Qua han") {
                                    cout << "[CANH BAO] Phieu muon nay da bi QUA HAN! Vui long kiem tra va thu tien phat theo quy dinh!\n";
                                    if (confirm_action("Ban da xu ly nop phat va chac chan muon xac nhan tra sach vao kho?")) {
                                        if (controller.return_book(br_id))
                                            cout << "[Thanh cong] Tra sach thanh cong, da hoan lai kho!\n";
                                    } else {
                                        cout << "[Huy bo] Da huy xac nhan tra sach.\n";
                                    }
                                } else {
                                    if (confirm_action("Xac nhan nhan tra sach va hoan lai kho?")) {
                                        if (controller.return_book(br_id))
                                            cout << "[Thanh cong] Tra sach thanh cong, da hoan kho!\n";
                                    } else {
                                        cout << "[Huy bo] Da huy xac nhan tra sach.\n";
                                    }
                                }
                            }
                        }
                    }
                    else if (choice == 7) {
                        string br_id = input_string("Nhap Ma phieu muon can xoa: ");
                        node<borrow>* br_node = controller.get_borrow_by_id(br_id);
                        if (br_node == nullptr) {
                            cout << "[Loi] Khong tim thay ma phieu muon!\n";
                        } else {
                            borrow b = br_node->get_data();
                            cout << "=> Phieu can xoa: [" << b.get_id() << "] Doc gia: " << b.get_user_id() << " | Trang thai: " << b.get_status() << "\n";
                            if (b.get_status() != "Da tra") {
                                cout << "[Loi] Khong the xoa! Sach chua duoc tra (" << b.get_status() << "). Chi duoc xoa phieu da hoan tat tra sach!\n";
                            } else {
                                if (confirm_action("Ban co chac chan muon xoa lich su phieu muon nay?")) {
                                    if (controller.delete_borrow(br_id))
                                        cout << "[Thanh cong] Da xoa phieu muon!\n";
                                    else
                                        cout << "[Loi] Khong the xoa phieu muon!\n";
                                } else {
                                    cout << "[Huy bo] Da huy thao tac xoa phieu muon.\n";
                                }
                            }
                        }
                    }
                    else if (choice == 8) {
                        if (!controller.has_pending_requests()) {
                            cout << "Hien khong co yeu cau dang ky nao trong Hang doi!\n";
                        } else {
                            cout << "Tong so yeu cau dang cho: " << controller.get_pending_request_count() << "\n";
                            borrow req = controller.peek_next_request();
                            cout << "\n[YEU CAU DAU TIEN - FIFO]\n"
                                 << "- Ma phieu: " << req.get_id() << "\n"
                                 << "- Ma sach: " << req.get_book_id() << "\n"
                                 << "- Ma doc gia: " << req.get_user_id() << "\n"
                                 << "- Ngay yeu cau: " << req.get_borrow_date() << "\n"
                                 << "- Ngay hen tra: " << req.get_due_date() << "\n";
                            int opt = input_int("Chon: (1) Duyet cap phieu | (2) Tu choi huy bo | (0) Bo qua: ", 0, 2);
                            if (opt == 1) {
                                if (controller.approve_next_request())
                                    cout << "[Thanh cong] Da duyet yeu cau va lap phieu muon chinh thuc!\n";
                                else
                                    cout << "[Loi] Duyet that bai (Het sach trong kho, trung ma phieu, hoac doc gia dang no qua han)!\n";
                            } else if (opt == 2) {
                                if (confirm_action("Ban co chac chan muon TU CHOI va XOA yeu cau nay khoi hang doi?")) {
                                    controller.reject_next_request();
                                    cout << "[Thanh cong] Da tu choi va xoa yeu cau khoi hang doi!\n";
                                } else {
                                    cout << "[Huy bo] Da giu nguyen yeu cau trong hang doi.\n";
                                }
                            }
                        }
                    }
                    else if (choice == 9) {
                        controller.show_all_shifts();
                    }
                    else if (choice == 10) {
                        date s_date = input_valid_date("Nhap ngay truc (DD MM YYYY): ");
                        string session = input_session("Nhap ca truc ('sang' hoac 'chieu'): ");
                        string adm_id = input_string("Nhap Ma Admin dang ky: ");
                        if (controller.register_shift(s_date, session, adm_id))
                            cout << "[Thanh cong] Da dang ky ca truc cho " << adm_id << "!\n";
                        else
                            cout << "[Loi] Ca truc da day hoac ma nhan vien khong hop le!\n";
                    }
                    else if (choice == 11) {
                        date s_date = input_valid_date("Nhap ngay truc (DD MM YYYY): ");
                        string session = input_session("Nhap ca truc ('sang' hoac 'chieu'): ");
                        string adm_id = input_string("Nhap Ma Admin can rut: ");
                        if (confirm_action("Xac nhan rut ten Admin " + adm_id + " khoi ca truc nay?")) {
                            if (controller.cancel_shift(s_date, session, adm_id))
                                cout << "[Thanh cong] Da rut ten Admin " << adm_id << " khoi ca truc!\n";
                            else
                                cout << "[Loi] Khong tim thay lich truc hoac Admin khong truc ca nay!\n";
                        } else {
                            cout << "[Huy bo] Da huy thao tac rut ten.\n";
                        }
                    }
                    else if (choice == 12) {
                        date s_date = input_valid_date("Nhap ngay truc can xoa (DD MM YYYY): ");
                        string session = input_session("Nhap ca truc can xoa ('sang' hoac 'chieu'): ");
                        if (confirm_action("CANH BAO: Xoa ca truc se huy lich cua tat ca nhan vien trong ca nay. Ban co chac chan?")) {
                            if (controller.delete_shift(s_date, session))
                                cout << "[Thanh cong] Da xoa toan bo ca truc!\n";
                            else
                                cout << "[Loi] Khong tim thay ca truc nay!\n";
                        } else {
                            cout << "[Huy bo] Da huy thao tac xoa ca truc.\n";
                        }
                    }
                    else if (choice == 13) {
                        string adm_id = input_string("Nhap Ma Admin can tinh luong: ");
                        int month = input_int("Nhap thang (0: tat ca cac thang, 1 - 12): ", 0, 12);
                        int year = input_int("Nhap nam (0: tat ca cac nam, 1900 - 2026): ", 0, 2026);
                        long long salary = controller.get_admin_salary(adm_id, month, year);
                        cout << "=> TONG LUONG CUA " << adm_id << ": " << salary << " VND (" << salary / 200000 << " buoi)\n";
                    }
                    else if (choice == 14) {
                        string u_id = input_string("Nhap Ma tai khoan: ");
                        string u_name = input_string("Nhap Ho va ten: ");
                        string u_pass = input_string("Nhap Mat khau: ");
                        string u_role;
                        while (true) {
                            u_role = input_string("Nhap Vai tro ('manager', 'admin', 'user'): ");
                            if (u_role == "manager" || u_role == "admin" || u_role == "user") break;
                            cout << "[Loi] Vai tro khong hop le! Vui long nhap 'manager', 'admin' hoac 'user'.\n";
                        }
                        if (controller.add_new_user(u_id, u_name, u_pass, u_role))
                            cout << "[Thanh cong] Da tao tai khoan moi!\n";
                        else
                            cout << "[Loi] Trung ma tai khoan!\n";
                    }
                    else if (choice == 15) {
                        string u_id = input_string("Nhap Ma tai khoan can xoa: ");
                        node<user>* u_node = controller.get_user_by_id(u_id);
                        if (u_node == nullptr) {
                            cout << "[Loi] Khong tim thay tai khoan!\n";
                        } else {
                            user usr = u_node->get_data();
                            cout << "=> Tai khoan can xoa: [" << usr.get_id() << "] " << usr.get_name() << " (Vai tro: " << usr.get_role() << ")\n";
                            if (confirm_action("CANH BAO: Tai khoan nay se bi xoa vinh vien! Ban co chac chan muon xoa?")) {
                                if (controller.delete_user(u_id))
                                    cout << "[Thanh cong] Da xoa tai khoan!\n";
                                else
                                    cout << "[Loi] Khong the xoa! Tai khoan dang co sach muon chua tra, co ca truc hoac la tai khoan ban dang dang nhap!\n";
                            } else {
                                cout << "[Huy bo] Da huy thao tac xoa tai khoan.\n";
                            }
                        }
                    }
                    else if (choice == 16) {
                        string old_id = input_string("Nhap Ma tai khoan cu can sua: ");
                        user updated_u;
                        cin >> updated_u;
                        if (controller.update_user_info(old_id, updated_u))
                            cout << "[Thanh cong] Da cap nhat tai khoan!\n";
                        else
                            cout << "[Loi] Cap nhat that bai!\n";
                    }
                    else if (choice == 17) {
                        string b_id = input_string("Nhap Ma sach (ID) can tim vi tri: ");
                        node<book>* b_node = controller.get_book_by_id(b_id);
                        if (b_node != nullptr) {
                            book b = b_node->get_data();
                            cout << "\n[THONG TIN VI TRI]\n"
                                << "- Sach: " << b.get_title() << "\n"
                                << "- The loai: " << b.get_category() << "\n"
                                << "=> VI TRI LUU TRU: KE [" << controller.get_book_shelf(b_id) << "]\n";
                        } else {
                            cout << "[Loi] Khong tim thay ma sach nay trong kho!\n";
                        }
                    }
                    else if (choice == 0) {
                        if (confirm_action("Ban co thuc su muon dang xuat khoi tai khoan Manager?")) {
                            controller.logout();
                            is_logged_in = false;
                            cout << "[Thanh cong] Da dang xuat.\n";
                        } else {
                            cout << "[Thong bao] Da huy dang xuat. Ban van o trong phien lam viec.\n";
                        }
                    }
                }
            }
            // ================= VÒNG LẶP DÀNH CHO ADMIN =================
            else if (role == "admin") {
                string my_id = u->get_id();
                while (is_logged_in) {
                    show_admin_menu(u->get_name());
                    int choice = input_int("Chon chuc nang (0 - 14): ", 0, 14);

                    // --- 1. Tra cứu sách & gợi ý ---
                    if (choice == 1) {
                        string keyword = input_string("Nhap tu khoa tim kiem (Ten/Tac gia/The loai): ");
                        linked_list<book> results = controller.search_books(keyword);
                        cout << "\n--- KET QUA TIM KIEM ---\n";
                        node<book>* curr = results.get_head();
                        string found_cat = "";
                        string found_id = "";
                        if (curr == nullptr) {
                            cout << "Khong tim thay sach phu hop!\n";
                        }
                        while (curr != nullptr) {
                            book b = curr->get_data();
                            if (found_cat.empty()) {
                                found_cat = b.get_category();
                                found_id = b.get_id();
                            }
                            cout << "- [" << b.get_id() << "] " << b.get_title() 
                                    << " | The loai: " << b.get_category() 
                                    << " | Tac gia: " << b.get_author() 
                                    << " | Ton kho: " << b.get_quantity() 
                                    << " | Vi tri: Ke " << controller.get_book_shelf(b.get_id()) << "\n";
                            curr = curr->get_next();
                        }
                        if (!found_cat.empty()) {
                            linked_list<book> related = controller.get_related_books(found_cat, found_id);
                            if (!related.empty()) {
                                cout << "\n--- SACH GOI Y LIEN QUAN (Cung the loai: " << found_cat << ") ---\n";
                                node<book>* rel_curr = related.get_head();
                                while (rel_curr != nullptr) {
                                    book rb = rel_curr->get_data();
                                    cout << " * [" << rb.get_id() << "] " << rb.get_title() 
                                         << " | Tac gia: " << rb.get_author() 
                                         << " | Ton kho: " << rb.get_quantity() << "\n";
                                    rel_curr = rel_curr->get_next();
                                }
                            }
                        }
                    }
                    // --- 2. Thêm sách mới ---
                    else if (choice == 2) {
                        string b_id = input_string("Nhap Ma sach (ID): ");
                        string title = input_string("Nhap ten sach: ");
                        string author = input_string("Nhap tac gia: ");
                        string cat = input_string("Nhap the loai: ");
                        int qty = input_int("So luong ton kho: ", 0, 10000);
                        int year = input_int("Nam xuat ban: ", 1000, 2026);
                        if (controller.add_new_book(b_id, title, author, cat, qty, year))
                            cout << "[Thanh cong] Da them sach moi vao kho!\n";
                        else
                            cout << "[Loi] Trung ma sach hoac khong the them!\n";
                    }
                    // --- 3. Sửa thông tin sách ---
                    else if (choice == 3) {
                        string old_id = input_string("Nhap Ma sach cu can sua: ");
                        cout << "--- Nhap thong tin moi cho sach ---\n";
                        book new_b;
                        cin >> new_b;
                        if (controller.update_book_info(old_id, new_b))
                            cout << "[Thanh cong] Da cap nhat thong tin sach!\n";
                        else
                            cout << "[Loi] Cap nhat that bai! (Khong tim thay ma cu, ma moi bi trung, hoac sach dang co nguoi muon nen khong the doi ma ID)\n";
                    }
                    // --- 4. Xóa sách ---
                    else if (choice == 4) {
                        string b_id = input_string("Nhap Ma sach can xoa: ");
                        node<book>* b_node = controller.get_book_by_id(b_id);
                        if (b_node == nullptr) {
                            cout << "[Loi] Khong tim thay ma sach can xoa!\n";
                        } else {
                            book b = b_node->get_data();
                            cout << "=> Sach can xoa: [" << b.get_id() << "] " << b.get_title() << " (Tac gia: " << b.get_author() << ")\n";
                            if (confirm_action("Ban co chac chan muon xoa sach nay khoi kho?")) {
                                if (controller.delete_book(b_id))
                                    cout << "[Thanh cong] Da xoa sach khoi he thong!\n";
                                else
                                    cout << "[Loi] Khong the xoa! Ma sach khong ton tai hoac SACH DANG CO NGUOI MUON chua tra!\n";
                            } else {
                                cout << "[Huy bo] Da huy thao tac xoa sach.\n";
                            }
                        }
                    }
                    // --- 5. Lập phiếu mượn sách ---
                    else if (choice == 5) {
                        string br_id = input_string("Nhap Ma phieu muon: ");
                        string b_id = input_string("Nhap Ma sach: ");
                        string u_id = input_string("Nhap Ma doc gia: ");
                        date b_date = input_valid_date("Ngay muon (DD MM YYYY): ");
                        int days = input_borrow_duration();
                        date due_date = b_date.add_days(days);
                        cout << "Ngay hen tra du kien: " << due_date << "\n";
                        if (controller.borrow_book(br_id, b_id, u_id, b_date, due_date))
                            cout << "[Thanh cong] Lap phieu muon thanh cong (Da tru kho)!\n";
                        else
                            cout << "[Loi] Trung ma phieu, sai thong tin, het sach, ban doc da muon cuon nay hoac dang no sach qua han!\n";
                    }
                    // --- 6. Xác nhận trả sách ---
                    else if (choice == 6) {
                        string br_id = input_string("Nhap Ma phieu muon can tra: ");
                        node<borrow>* br_node = controller.get_borrow_by_id(br_id);
                        if (br_node == nullptr) {
                            cout << "[Loi] Khong tim thay ma phieu muon nay trong he thong!\n";
                        } else {
                            borrow ticket = br_node->get_data();
                            if (ticket.get_status() == "Da tra") {
                                cout << "[Thong bao] Phieu muon nay da duoc tra tu truoc roi!\n";
                            } else {
                                cout << "=> Thong tin phieu: Ma sach: " << ticket.get_book_id() 
                                     << " | Doc gia: " << ticket.get_user_id() 
                                     << " | Han tra: " << ticket.get_due_date() 
                                     << " | Trang thai: " << ticket.get_status() << "\n";
                                
                                if (ticket.get_status() == "Qua han") {
                                    cout << "[CANH BAO] Phieu muon nay da bi QUA HAN! Vui long kiem tra va thu tien phat theo quy dinh!\n";
                                    if (confirm_action("Ban da xu ly nop phat va chac chan muon xac nhan tra sach vao kho?")) {
                                        if (controller.return_book(br_id))
                                            cout << "[Thanh cong] Tra sach thanh cong (Da hoan lai vao kho)!\n";
                                    } else {
                                        cout << "[Huy bo] Da huy xac nhan tra sach.\n";
                                    }
                                } else {
                                    if (confirm_action("Xac nhan nhan tra sach va hoan lai vao kho?")) {
                                        if (controller.return_book(br_id))
                                            cout << "[Thanh cong] Tra sach thanh cong (Da hoan lai vao kho)!\n";
                                    } else {
                                        cout << "[Huy bo] Da huy xac nhan tra sach.\n";
                                    }
                                }
                            }
                        }
                    }
                    // --- 7. Xóa phiếu mượn ---
                    else if (choice == 7) {
                        string br_id = input_string("Nhap Ma phieu muon can xoa: ");
                        node<borrow>* br_node = controller.get_borrow_by_id(br_id);
                        if (br_node == nullptr) {
                            cout << "[Loi] Khong tim thay ma phieu muon can xoa!\n";
                        } else {
                            borrow b = br_node->get_data();
                            cout << "=> Phieu can xoa: [" << b.get_id() << "] Doc gia: " << b.get_user_id() << " | Trang thai: " << b.get_status() << "\n";
                            if (b.get_status() != "Da tra") {
                                cout << "[Loi] Khong the xoa! Sach chua duoc tra (" << b.get_status() << "). Chi duoc xoa phieu da hoan tat tra sach!\n";
                            } else {
                                if (confirm_action("Ban co chac chan muon xoa lich su phieu muon nay?")) {
                                    if (controller.delete_borrow(br_id))
                                        cout << "[Thanh cong] Da xoa phieu muon khoi he thong!\n";
                                    else
                                        cout << "[Loi] Khong the xoa phieu muon!\n";
                                } else {
                                    cout << "[Huy bo] Da huy thao tac xoa phieu muon.\n";
                                }
                            }
                        }
                    }
                    // --- 8. Duyệt hàng đợi đăng ký sách từ User ---
                    else if (choice == 8) {
                        if (!controller.has_pending_requests()) {
                            cout << "Hien khong co yeu cau dang ky nao trong Hang doi!\n";
                        } else {
                            cout << "Tong so yeu cau dang cho: " << controller.get_pending_request_count() << "\n";
                            borrow req = controller.peek_next_request();
                            cout << "\n[YEU CAU DAU TIEN - FIFO]\n"
                                 << "- Ma phieu: " << req.get_id() << "\n"
                                 << "- Ma sach: " << req.get_book_id() << "\n"
                                 << "- Ma doc gia: " << req.get_user_id() << "\n"
                                 << "- Ngay yeu cau: " << req.get_borrow_date() << "\n"
                                 << "- Ngay hen tra: " << req.get_due_date() << "\n";
                            int opt = input_int("Chon: (1) Duyet cap phieu | (2) Tu choi huy bo | (0) Bo qua: ", 0, 2);
                            if (opt == 1) {
                                if (controller.approve_next_request())
                                    cout << "[Thanh cong] Da duyet yeu cau va lap phieu muon (Da tru kho)!\n";
                                else
                                    cout << "[Loi] Duyet that bai (Het sach trong kho, trung ma phieu, hoac doc gia dang no qua han)!\n";
                            } else if (opt == 2) {
                                if (confirm_action("Ban co chac chan muon TU CHOI va XOA yeu cau nay khoi hang doi?")) {
                                    controller.reject_next_request();
                                    cout << "[Thanh cong] Da tu choi va xoa yeu cau!\n";
                                } else {
                                    cout << "[Huy bo] Da giu nguyen yeu cau trong hang doi.\n";
                                }
                            }
                        }
                    }
                    // --- 9. Lọc ngày phiếu mượn ---
                    else if (choice == 9) {
                        date start_date = input_valid_date("Nhap ngay bat dau (DD MM YYYY): ");
                        date end_date;
                        while (true) {
                            end_date = input_valid_date("Nhap ngay ket thuc (DD MM YYYY): ");
                            if (end_date >= start_date) {
                                break;
                            }
                            cout << "[Loi] Ngay ket thuc khong the truoc ngay bat dau!\n";
                        }
                        linked_list<borrow> records = controller.filter_borrows_by_date(start_date, end_date);
                        node<borrow> *curr = records.get_head();
                        cout << "\n--- KET QUA LOC PHIEU MUON ---\n";
                        if (curr == nullptr) {
                            cout << "Khong co phieu muon trong khoang ngay nay!\n";
                        }
                        while (curr != nullptr) {
                            borrow b = curr->get_data();
                            cout << "- Ma phieu: " << b.get_id() << "\n";
                            cout << "  Ma sach: " << b.get_book_id() << "\n";
                            cout << "  Ma doc gia: " << b.get_user_id() << "\n";
                            cout << "  Ngay muon: " << b.get_borrow_date() << "\n";
                            cout << "  Han tra: " << b.get_due_date() << "\n";
                            cout << "  Trang thai: " << b.get_status() << "\n";
                            curr = curr->get_next();
                        }
                    }
                    // --- 10. Xem lịch trực bản thân ---
                    else if (choice == 10) {
                        linked_list<shift> my_shifts = controller.get_my_shifts(my_id);
                        node<shift>* curr = my_shifts.get_head();
                        if (curr == nullptr) {
                            cout << "Ban hien chua dang ky ca truc nao!\n";
                        } else {
                            cout << "\n--- LICH TRUC CA NHAN ---\n";
                            while (curr != nullptr) {
                                shift s = curr->get_data();
                                date d = s.get_shift_date();
                                cout << "- Ngay: " << d << " (" << d.get_day_of_week() << ")"
                                     << " | Ca: " << s.get_session() 
                                     << " | Tong so nguoi truc ca nay: " << s.get_current_count() << "/4\n";
                                curr = curr->get_next();
                            }
                        }
                    }
                    // --- 11. Đăng ký ca trực bản thân ---
                    else if (choice == 11) {
                        date s_date = input_valid_date("Nhap ngay muon dang ky (DD MM YYYY): ");
                        string session = input_session("Nhap ca muon truc ('sang' hoac 'chieu'): ");
                        if (controller.register_shift(s_date, session, my_id))
                            cout << "[Thanh cong] Da dang ky ca truc cho ban than!\n";
                        else
                            cout << "[Loi] Ca truc da day 4 nguoi hoac ban da dang ky ca nay roi!\n";
                    }
                    // --- 12. Rút tên khỏi ca trực bản thân ---
                    else if (choice == 12) {
                        date s_date = input_valid_date("Nhap ngay muon rut ten (DD MM YYYY): ");
                        string session = input_session("Nhap ca muon rut ('sang' hoac 'chieu'): ");
                        if (confirm_action("Ban co chac chan muon rut ten khoi ca truc nay khong?")) {
                            if (controller.cancel_shift(s_date, session, my_id))
                                cout << "[Thanh cong] Da rut ten khoi ca truc!\n";
                            else
                                cout << "[Loi] Ban khong co trong ca truc nay!\n";
                        } else {
                            cout << "[Huy bo] Da huy thao tac rut khoi ca truc.\n";
                        }
                    }
                    // --- 13. Xem tiền lương bản thân ---
                    else if (choice == 13) {
                        int month = input_int("Nhap thang muon xem (0: tat ca cac thang, 1 - 12): ", 0, 12);
                        int year = input_int("Nhap nam muon xem (0: tat ca cac nam, 1900 - 2026): ", 0, 2026);
                        long long salary = controller.get_admin_salary(my_id, month, year);
                        cout << "\n--- BANG LUONG CA NHAN ---\n";
                        cout << "- So buoi da truc: " << salary / 200000 << " buoi\n";
                        cout << "- Don gia: 200.000 VND / buoi\n";
                        cout << "=> TONG TIEN: " << salary << " VND\n";
                    }
                    else if (choice == 14) {
                        string b_id = input_string("Nhap Ma sach (ID) can tim vi tri: ");
                        node<book>* b_node = controller.get_book_by_id(b_id);
                        if (b_node != nullptr) {
                            book b = b_node->get_data();
                            cout << "\n[THONG TIN VI TRI]\n"
                                << "- Sach: " << b.get_title() << "\n"
                                << "- The loai: " << b.get_category() << "\n"
                                << "=> VI TRI LUU TRU: KE [" << controller.get_book_shelf(b_id) << "]\n";
                        } else {
                            cout << "[Loi] Khong tim thay ma sach nay trong kho!\n";
                        }
                    }
                    else if (choice == 0) {
                        if (confirm_action("Ban co thuc su muon dang xuat khoi tai khoan Admin?")) {
                            controller.logout();
                            is_logged_in = false;
                            cout << "[Thanh cong] Da dang xuat.\n";
                        } else {
                            cout << "[Thong bao] Da huy dang xuat. Ban van o trong phien lam viec.\n";
                        }
                    }
                }
            }
            // ================= VÒNG LẶP CHO ĐỘC GIẢ (USER) =================
            else if (role == "user") {
                string my_id = u->get_id();
                while (is_logged_in) {
                    show_user_menu(u->get_name());
                    int choice = input_int("Chon chuc nang (0 - 5): ", 0, 5);

                    if (choice == 1) {
                        // Tìm kiếm sách & hiện sách liên quan
                        string keyword = input_string("Nhap ten sach, the loai hoac tac gia muon tim: ");
                        linked_list<book> results = controller.search_books(keyword);
                        cout << "\n--- KET QUA TIM KIEM ---\n";
                        node<book>* curr = results.get_head();
                        string found_cat = "";
                        string found_id = "";
                        if (curr == nullptr) {
                            cout << "Khong tim thay sach!\n";
                        }
                        while (curr != nullptr) {
                            book b = curr->get_data();
                            if (found_cat.empty()) {
                                found_cat = b.get_category();
                                found_id = b.get_id();
                            }
                            cout << "- [" << b.get_id() << "] " << b.get_title() 
                                    << " | The loai: " << b.get_category() 
                                    << " | Tac gia: " << b.get_author() 
                                    << " | Ton kho: " << b.get_quantity() 
                                    << " | Vi tri: Ke " << controller.get_book_shelf(b.get_id()) << "\n";
                            curr = curr->get_next();
                        }
                        // Hiển thị sách gợi ý liên quan cùng thể loại
                        if (!found_cat.empty()) {
                            linked_list<book> related = controller.get_related_books(found_cat, found_id);
                            if (!related.empty()) {
                                cout << "\n--- CO THE BAN CUNG QUAN TAM (Cung the loai: " << found_cat << ") ---\n";
                                node<book>* rel_curr = related.get_head();
                                while (rel_curr != nullptr) {
                                    book rb = rel_curr->get_data();
                                    cout << " * [" << rb.get_id() << "] " << rb.get_title() 
                                         << " | Tac gia: " << rb.get_author() 
                                         << " | Con lai: " << rb.get_quantity() << "\n";
                                    rel_curr = rel_curr->get_next();
                                }
                            }
                        }
                    }
                    else if (choice == 2) {
                        // Đăng ký mượn sách (Queue FIFO)
                        string req_id = input_string("Nhap Ma yeu cau (VD: REQ01): ");
                        string b_id = input_string("Nhap Ma sach muon dang ky muon: ");
                        date b_date = input_valid_date("Ngay mong muon muon (DD MM YYYY): ");
                        int days = input_borrow_duration();
                        date due_date = b_date.add_days(days);
                        cout << "Ngay hen tra du kien: " << due_date << "\n";
                        if (controller.register_book_request(req_id, b_id, my_id, b_date, due_date)) {
                            cout << "[Thanh cong] Yeu cau cua ban da duoc dua vao Hang doi (Queue)! Vui long cho Manager/Admin duyet.\n";
                        } else {
                            cout << "[Loi] Dang ky that bai! Kiem tra lai:\n"
                                 << "  - Ma sach khong ton tai hoac sach da het trong kho (so luong = 0)\n"
                                 << "  - Trung ma yeu cau (ma ID da ton tai trong he thong)\n"
                                 << "  - Ban dang muon cuon sach nay chua tra hoac dang co sach no qua han!\n";
                        }
                    }
                    else if (choice == 3) {
                        // Xem sách đang mượn
                        linked_list<borrow> records = controller.get_my_borrows(my_id, true);
                        node<borrow>* curr = records.get_head();
                        cout << "\n--- SACH DANG MUON ---\n";
                        if (curr == nullptr) {
                            cout << "Ban hien khong co sach nao dang muon!\n";
                        }
                        while (curr != nullptr) {
                            borrow b = curr->get_data();
                            node<book>* b_node = controller.get_book_by_id(b.get_book_id());
                            cout << "- Ma phieu: " << b.get_id() << "\n";
                            if (b_node != nullptr) {
                                book bk = b_node->get_data();
                                cout << "  Ten sach: " << bk.get_title() << "\n";
                            } else {
                                cout << "  Ma sach: " << b.get_book_id() << "\n";
                            }
                            cout << "  Ngay muon: " << b.get_borrow_date() << "\n";
                            cout << "  Han tra: " << b.get_due_date() << "\n";
                            cout << "  Trang thai: " << b.get_status() << "\n";
                            curr = curr->get_next();
                        }
                    }
                    else if (choice == 4) {
                        // Xem lịch sử mượn sách
                        linked_list<borrow> records = controller.get_my_borrows(my_id, false);
                        node<borrow> *curr = records.get_head();
                        cout << "\n--- LICH SU MUON SACH ---\n";
                        if (curr == nullptr) {
                            cout << "Ban chua co lich su muon sach!\n";
                        }
                        while (curr != nullptr) {
                            borrow b = curr->get_data();
                            node<book> *b_node = controller.get_book_by_id(b.get_book_id());
                            cout << "- Ma phieu: " << b.get_id() << "\n";
                            if (b_node != nullptr) {
                                book bk = b_node->get_data();
                                cout << "  Ten sach: " << bk.get_title() << "\n";
                            }
                            else {
                                cout << "  Ma sach: " << b.get_book_id() << "\n";
                            }
                            cout << "  Ngay muon: " << b.get_borrow_date() << "\n";
                            cout << "  Han tra: " << b.get_due_date() << "\n";
                            cout << "  Trang thai: " << b.get_status() << "\n";
                            curr = curr->get_next();
                        }
                    }
                    else if (choice == 5) {
                        // Tra cứu vị trí kệ sách để đến lấy
                        string b_id = input_string("Nhap Ma sach (ID) can tim vi tri: ");
                        node<book>* b_node = controller.get_book_by_id(b_id);
                        if (b_node != nullptr) {
                            book b = b_node->get_data();
                            cout << "\n[SO DO VI TRI SACH]\n"
                                << "- Ten sach: " << b.get_title() << "\n"
                                << "- The loai: " << b.get_category() << "\n"
                                << "=> HAY DEN KHU VUC: KE [" << controller.get_book_shelf(b_id) << "]\n";
                        } else {
                            cout << "[Loi] Ma sach khong ton tai trong thu vien!\n";
                        }
                    }
                    else if (choice == 0) {
                        if (confirm_action("Ban co thuc su muon dang xuat khoi tai khoan Doc gia?")) {
                            controller.logout();
                            is_logged_in = false;
                            cout << "[Thanh cong] Da dang xuat.\n";
                        } else {
                            cout << "[Thong bao] Da huy dang xuat. Ban van o trong phien lam viec.\n";
                        }
                    }
                }
            }
        } else {
            cout << "[Loi] Dang nhap that bai! Sai Ma ID hoac Mat khau.\n";
        }
    }

    return 0;
}
