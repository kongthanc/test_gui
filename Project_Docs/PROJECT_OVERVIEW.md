# PBL2 - Hệ thống quản lý thư viện

> Đối chiếu ngày 07/11/2026: ZIP người dùng gửi có Git HEAD `9e14749`; nhánh GitHub `main` hiện ở `bd3e598` (`full manf hinh`).
> Kiến trúc tổng thể không thay đổi so với ZIP. Các chỉnh sửa sau ZIP được ghi nhận riêng trong `PROGRESS.md` và `CHANGELOG.md`; chưa coi là đã kiểm thử nếu chưa có kết quả chạy thực tế.

## 1. Mục tiêu và phạm vi

Dự án xây dựng ứng dụng quản lý thư viện bằng C++ với giao diện đồ họa Raylib và dữ liệu lưu bằng các tệp văn bản. Ứng dụng có ba nhóm tài khoản:

- `manager`: quản lý toàn bộ nghiệp vụ.
- `admin`: quản trị viên, xử lý sách, mượn/trả, yêu cầu mượn, ca trực và lương cá nhân.
- `user`: độc giả, tìm sách, đăng ký mượn, xem sách đang mượn, lịch sử và thông báo.

Ứng dụng hiện chạy theo luồng đăng nhập trước, sau đó mở giao diện tương ứng với vai trò.

## 2. Công nghệ và cách chạy

- Ngôn ngữ: C++.
- Chuẩn biên dịch đang dùng: C++17.
- Thư viện giao diện: Raylib.
- Lưu trữ: tệp `.txt` trong thư mục `Data/`.
- Môi trường phát triển được dùng trong quá trình làm dự án: Windows và MinGW-w64/MSYS2 UCRT64.

Lệnh build GUI đã dùng trong dự án:

```bash
g++ -std=c++17 gui_main.cpp GUI/login.cpp GUI/library_gui.cpp Controller/*.cpp Service/*.cpp Model/*.cpp -o library_gui.exe -lraylib -lopengl32 -lgdi32 -lwinmm -lcomctl32
```

Lệnh trên không biên dịch `GUI/raylib_test.cpp` vì tệp này là mã thử nghiệm riêng và có thể chứa `main()` thứ hai. Cần bảo đảm Raylib và trình biên dịch đã được cấu hình đúng trước khi chạy. Lệnh được ghi lại từ quy trình phát triển, chưa phải kết quả kiểm thử build độc lập của lần rà soát tài liệu này.

## 3. Cấu trúc thư mục

```text
PBL2_GUI_test/
├── Algorithm/
│   ├── graph_algo.h
│   ├── search_algo.h
│   └── sort_algo.h
├── Controller/
│   ├── library_controller.h
│   └── library_controller.cpp
├── Data/
│   ├── book.txt
│   ├── borrow.txt
│   ├── notification.txt
│   ├── position.txt
│   ├── request.txt
│   ├── shift.txt
│   └── user.txt
├── GUI/
│   ├── library_gui.h
│   ├── library_gui.cpp
│   ├── login.h
│   ├── login.cpp
│   ├── ui_font.h
│   ├── ui_layout.h
│   ├── windows_text_input.h
│   └── raylib_test.cpp
├── Model/
│   ├── action_log.h
│   ├── book.h / book.cpp
│   ├── borrow.h / borrow.cpp
│   ├── date.h / date.cpp
│   ├── library_entity.h
│   ├── library_exception.h
│   ├── notification.h / notification.cpp
│   ├── role_permission.h
│   ├── shift.h / shift.cpp
│   └── user.h / user.cpp
├── Project_Docs/
├── Service/
│   ├── book_service.h / book_service.cpp
│   ├── borrow_service.h / borrow_service.cpp
│   ├── file_helper.h
│   ├── notification_service.h / notification_service.cpp
│   ├── shift_service.h / shift_service.cpp
│   └── user_service.h / user_service.cpp
├── Structure/
│   ├── graph.h / graph.cpp
│   ├── hash_table.h / hash_table.cpp
│   ├── linked_list.h / linked_list.cpp
│   ├── node.h / node.cpp
│   ├── queue.h / queue.cpp
│   └── stack.h / stack.cpp
├── gui_main.cpp
├── all_code.txt
└── .gitignore
```

### Vai trò từng lớp thư mục

| Thư mục | Trách nhiệm |
|---|---|
| `GUI` | Màn hình đăng nhập, giao diện Raylib, nhập liệu, điều hướng trang |
| `Controller` | Điều phối nghiệp vụ, kết nối GUI với Service và quản lý hàng đợi yêu cầu |
| `Service` | Nghiệp vụ và đọc/ghi dữ liệu |
| `Model` | Các thực thể và kiểu dữ liệu của hệ thống |
| `Structure` | Cấu trúc dữ liệu tự cài đặt |
| `Algorithm` | Thuật toán tìm kiếm, sắp xếp và xử lý đồ thị |
| `Data` | Dữ liệu chạy thực tế của ứng dụng |
| `Project_Docs` | Tài liệu dự án và bàn giao giữa các phiên làm việc |

## 4. Kiến trúc và luồng xử lý

Luồng tổng quát:

```text
Người dùng
   |
   v
GUI (Raylib)
   |
   v
library_controller
   |
   +--> book_service ------> book.txt
   +--> user_service ------> user.txt
   +--> borrow_service ----> borrow.txt
   +--> shift_service -----> shift.txt
   +--> notification_service -> notification.txt
   |
   +--> request queue -----> request.txt
   +--> vị trí kệ ---------> position.txt
```

- `gui_main.cpp` khởi tạo cửa sổ Raylib, font, bộ nhập văn bản Windows, `library_controller`, màn hình đăng nhập và giao diện chính.
- `login_screen` xác thực thông qua `library_controller::login()` và chuyển vai trò cho `library_gui`.
- `library_gui` vẽ giao diện và gọi các hàm Controller.
- `library_controller` điều phối các Service, đồng thời quản lý hàng đợi yêu cầu mượn và danh sách vị trí kệ.
- Các Service duy trì dữ liệu trong bộ nhớ và đọc/ghi tệp tương ứng.

## 5. Các chức năng hiện có theo mã nguồn

### 5.1. Đăng nhập và phân quyền

- Đăng nhập bằng ID và mật khẩu thông qua `user_service`.
- Ô đăng nhập chỉ nhận ASCII; cần bảo đảm lọc Unicode có dấu không làm mất chuỗi Telex ASCII đang nhập, ví dụ `thawngs`.
- Vai trò được lưu bằng chuỗi `manager`, `admin`, `user`.
- Sidebar GUI được tạo theo vai trò.
- `Model/role_permission.h` có các lớp quyền và `permission_factory` cho luồng menu console/kiến trúc quyền; GUI hiện phân nhánh menu trực tiếp theo chuỗi vai trò.

### 5.2. Sách

- Tìm kiếm sách.
- Thêm, sửa, xóa sách.
- Sắp xếp và lọc sách ở tầng Service/Controller.
- Gợi ý sách liên quan và danh mục nổi bật.
- Tra cứu vị trí kệ.
- Vị trí kệ được quản lý riêng qua `position.txt`.

### 5.3. Mượn và trả

- Tạo phiếu mượn trực tiếp ở phía quản trị.
- Đăng ký yêu cầu mượn từ tài khoản độc giả.
- Controller chặn cùng một độc giả gửi nhiều yêu cầu `Cho duyet` cho cùng một đầu sách; GUI hiện vẫn trả thông báo thất bại chung khi đăng ký không thành công.
- Duyệt yêu cầu để tạo phiếu mượn.
- Từ chối yêu cầu và ghi vào dữ liệu phiếu mượn với trạng thái từ chối.
- Trả sách, cập nhật trạng thái và hoàn lại số lượng sách trong kho.
- Tự cập nhật trạng thái quá hạn khi hệ thống khởi tạo/đăng nhập và kiểm tra ngày.
- Lọc phiếu mượn theo khoảng ngày.
- Độc giả có trang riêng cho sách đang mượn và lịch sử.
- Trong màn hình `Đăng ký mượn sách`, có nút `Danh sách yêu cầu (N)` để mở danh sách các yêu cầu `Cho duyet` của tài khoản hiện tại; danh sách dùng chung màn hình, không thêm mục sidebar hoặc tệp dữ liệu. Chức năng đã được bổ sung vào source, cần build/chạy GUI để xác minh.

Theo `borrow_service::get_borrows_by_user()`:
- Trang đang mượn lấy trạng thái `Dang muon` và `Qua han`.
- Trang lịch sử lấy trạng thái `Da tra` và `Tu choi`.

### 5.4. Hàng đợi yêu cầu mượn

- Dùng `queue<borrow>` tự cài đặt, xử lý theo FIFO.
- Yêu cầu đang chờ được lưu ở `Data/request.txt`.
- Mã yêu cầu được sinh theo dạng `REQ` + số tăng dựa trên ID lớn nhất tìm thấy trong `request.txt` và `borrow.txt`.
- Khi duyệt thành công, yêu cầu bị lấy khỏi hàng đợi và phiếu mượn được tạo.
- Khi từ chối, yêu cầu bị lấy khỏi hàng đợi, được ghi vào dữ liệu mượn với trạng thái `Tu choi`, đồng thời tạo thông báo cho độc giả.

### 5.5. Thông báo

- Có Model `notification` và Service riêng.
- Thông báo được lưu trong `Data/notification.txt`.
- Khi duyệt hoặc từ chối yêu cầu, Controller tạo thông báo cho đúng độc giả.
- Có chức năng lấy danh sách thông báo, đếm chưa đọc và đánh dấu đã đọc.
- GUI có trang `USER_NOTIFICATIONS` và nút chuông trên thanh đầu trang đối với vai trò `user`.
- Chuông được vẽ theo vai trò, không phụ thuộc việc số thông báo chưa đọc có lớn hơn 0 hay không; số chưa đọc chỉ điều khiển huy hiệu số.

### 5.6. Ca trực và lương

- Đăng ký ca, hủy đăng ký ca, xóa toàn bộ một ca.
- Xem lịch ca của cá nhân hoặc toàn bộ tùy vai trò.
- Tính lương theo số ca, mức mặc định 200.000 VND/ca.

### 5.7. Cấu trúc dữ liệu và thuật toán

Các cấu trúc tự cài đặt có trong dự án:
- Danh sách liên kết đôi: `linked_list`, `node`.
- Hàng đợi FIFO: `queue`.
- Ngăn xếp: `stack`.
- Bảng băm: `hash_table`.
- Đồ thị: `graph`.
- Thuật toán tìm kiếm, sắp xếp và BFS/đồ thị trong `Algorithm`.

### 5.8. Hoàn tác (Undo)

- Có `action_log<T>` và các Stack dành cho sách, người dùng, phiếu mượn.
- Undo sách có xử lý ADD/DELETE/UPDATE.
- Hai hàm `undo_user_action()` và `undo_borrow_action()` hiện trả về `false`, do đó chưa triển khai nghiệp vụ hoàn tác người dùng và phiếu mượn.

## 6. Định dạng dữ liệu

Các tệp dùng dấu `;` để phân tách trường. Cấu trúc được xác định từ hàm đọc/ghi và dữ liệu mẫu hiện có:

| Tệp | Cấu trúc bản ghi |
|---|---|
| `book.txt` | `BookID;Title;Author;Category;Quantity;Year` |
| `borrow.txt` | `BorrowID;BookID;UserID;BorrowDate;DueDate;Status` |
| `request.txt` | `RequestID;BookID;UserID;BorrowDate;DueDate;Status` |
| `notification.txt` | `NotificationID;UserID;Title;Content;CreatedAt;IsRead` |
| `position.txt` | `BookID;Category;Shelf` |
| `shift.txt` | `Date;Session;AdminID[,AdminID...]` |
| `user.txt` | `UserID;Name;Password;Role` |

Lưu ý:
- Ngày trong các tệp nghiệp vụ được ghi theo dạng ngày/tháng/năm.
- `notification.txt`: `IsRead` dùng `0` cho chưa đọc và `1` cho đã đọc.
- `borrow.txt` có thể chứa phiếu mượn và bản ghi yêu cầu bị từ chối. Mã `REQ...` được dùng để nhận diện bản ghi yêu cầu trong một số xử lý.
- Mật khẩu hiện được lưu trực tiếp trong `user.txt`, chưa thấy cơ chế băm mật khẩu trong phần mã đã rà soát. Không dùng dữ liệu tài khoản thật hoặc mật khẩu đang sử dụng ở nơi khác trong bộ dữ liệu mẫu.

## 7. Backlog ưu tiên tiếp theo

1. **Tra cứu người dùng kèm sách đang mượn:** màn hình hiện chỉ tra cứu ID và hiển thị tên/vai trò; chưa hiển thị danh sách sách và trạng thái từng phiếu.
2. **Thông báo đăng ký trùng:** Controller đã chặn yêu cầu trùng, nhưng GUI vẫn hiện thông báo thất bại chung, chưa nêu rõ nguyên nhân.
3. **Layout toàn cửa sổ:** source mới đã mở rộng phần header, sidebar và khung nội dung theo vùng trống do `ui_layout`; cần build/chạy ở nhiều kích thước cửa sổ để xác minh, đặc biệt cả màn hình đăng nhập.
4. **Ngày không hợp lệ:** `Model/date.cpp` đã đổi sang ngày dự phòng khi constructor nhận ngày sai; `library_gui::read_date()` so sánh lại giá trị để từ chối input không hợp lệ. Chưa có kiểm thử runtime; cần cân nhắc tránh việc constructor âm thầm thay ngày sai bằng ngày mặc định ở các luồng khác.
5. **Telex ở đăng nhập:** `GUI/login.cpp` vẫn chỉ nhận codepoint ASCII (32-126); chưa có thay đổi chứng minh chuỗi Telex như `thawngs` được giữ nguyên khi IME đang hoạt động.

## 8. Quy ước phát triển

- Khi sửa code, phải nêu chính xác file, hàm và vị trí.
- Ưu tiên cung cấp nguyên hàm hoặc đoạn thay thế đầy đủ để dễ áp dụng.
- Giữ lại comment đang có nếu không cần thay đổi.
- Không tự ý đổi kiến trúc hoặc quy ước dữ liệu đã thống nhất.
- Trước khi sửa, phải đọc phiên bản file mới nhất trong ZIP/repository hiện tại, không dựa vào đoạn code cũ trong hội thoại.
- Sau khi sửa, cần build và kiểm tra luồng liên quan; chỉ đánh dấu hoàn thành khi đã có kết quả kiểm thử.
- Không tự ý reset, checkout hoặc ghi đè các thay đổi chưa commit.
