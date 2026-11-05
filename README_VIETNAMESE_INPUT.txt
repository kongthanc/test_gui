SUA NHAN KY TU TIENG VIET TREN WINDOWS

Ban nay thu xu ly input theo thu tu thong diep Windows gui den cua so Raylib:
- Bat WM_CHAR / WM_IME_CHAR / WM_UNICHAR de nhan Unicode.
- Bat WM_KEYDOWN cua Backspace de xu ly thao tac sua Telex dung thu tu.
- Khong doc GetCharPressed() cho cac o nhap lieu nua, tranh nhan trung ky tu.

Bien dich tren MSYS2 UCRT64:
g++ -std=c++17 gui_main.cpp GUI/login.cpp GUI/library_gui.cpp Controller/*.cpp Service/*.cpp Model/*.cpp -o library_gui.exe -lraylib -lopengl32 -lgdi32 -lwinmm -lcomctl32

Luu y:
- Can Raylib 6.x tren Windows.
- Neu SetWindowSubclass that bai, chuong trinh van mo nhung bo xu ly input rieng khong hoat dong.
- Day la ban thu nghiem can kiem tra truc tiep tren Windows voi UniKey/EVKey.
