PBL2_GUI_test - ban chinh sua

Da cap nhat:
1. Algorithm/search_algo.h: to_lower xu ly UTF-8 cho chu hoa Latin va tieng Viet; contains co the tim kiem khong phan biet hoa/thuong voi chu co dau.
2. Data/book.txt: chuyen ten sach, tac gia va the loai sang tieng Viet co dau.
3. Data/position.txt: cap nhat ten the loai dong bo voi book.txt.
4. Data/user.txt: chuan hoa ten nguoi dung sang tieng Viet co dau va loai bo dong AD01 bi lap y het (ID trung co the lam sai chi muc hash table).

Giu nguyen:
- ID, mat khau, role, trang thai muon/tra/qua han, ca lam, ngay thang va cau truc cac cot.
- GUI/library_gui.cpp da co xu ly codepoint -> UTF-8 va xoa mot ky tu UTF-8.
- GUI/login.cpp van chi nhan ASCII cho ID/mat khau, phu hop voi cac tai khoan hien tai.

Kiem tra:
- Da bien dich test rieng search_algo.h bang g++ -std=c++17.
- Da test 3 truong hop tim kiem tieng Viet khong phan biet hoa/thuong, ca 3 deu pass.
- Chua the bien dich toan bo GUI trong moi truong nay vi khong co thu vien Raylib (pkg-config khong tim thay raylib).
- Chua xac nhan duoc go Telex/UniKey truc tiep trong cua so Raylib tren Windows. Can test tren may Windows cua ban.
