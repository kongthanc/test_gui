WINDOWS IME INPUT (UNIKEy / EVKEY)

Da bo bo go Telex tu viet khoi GUI. Cac o nhap trong library_gui.cpp hien doc ky tu Unicode da duoc Windows/IME commit thong qua GetCharPressed() cua Raylib.

Ly do:
- Tren Windows, IME xu ly qua trinh ghep chu va gui ket qua da commit cho ung dung.
- GLFW/Raylib nhan ky tu WM_CHAR va dua vao hang doi ky tu Unicode cua Raylib.
- Vi vay khong nen tu doc phim A-Z de tu ghep Telex trong khi UniKey/EVKey dang bat.

Kiem tra tren Windows:
1. Mo UniKey/EVKey va chon bang ma Unicode, kieu go Telex.
2. Chay app, vao mot o nhap trong trang tim sach.
3. Go "caaus" va kiem tra o nhap hien "cấu"; go "huowngs" va kiem tra hien "hướng".
4. Thu Backspace khi dang go va sau khi chu da hien trong o.
5. Thu Ctrl+V voi mot chuoi tieng Viet.

Luu y:
- Raylib/GLFW khong cung cap bo go Telex rieng. Ban nay dung IME mac dinh cua Windows.
- Chua the xac nhan hanh vi IME tren may Windows cua nguoi dung trong moi truong build hien tai.
- Neu UniKey/EVKey van khong commit ky tu vao o nhap, can lay them thong tin Windows version, Raylib/GLFW build va cach go; khi do moi can can nhac xu ly native Win32 IME rieng.
