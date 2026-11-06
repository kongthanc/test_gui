# Tiến độ dự án PBL2

> Snapshot đối chiếu: `PBL2_GUI_test(8).zip`, ngày 06/11/2026.
> Trạng thái bên dưới phân biệt rõ: có mã nguồn, đã kiểm thử, và còn thiếu. Có mã nguồn không đồng nghĩa đã kiểm thử hoàn chỉnh.

## 1. Trạng thái tổng quan

- Ứng dụng GUI Raylib đã có luồng đăng nhập, sidebar theo vai trò và nhiều màn hình nghiệp vụ.
- Các tầng Model, Service, Controller, cấu trúc dữ liệu và thuật toán đã được tổ chức riêng.
- Dữ liệu được lưu bằng các tệp trong `Data/`.
- Chưa có bằng chứng trong ZIP về một bộ kiểm thử tự động hoặc biên bản kiểm thử toàn hệ thống.

## 2. Chức năng đã có mã nguồn

### Tài khoản và phân quyền
- [x] Đăng nhập qua ID/mật khẩu.
- [x] Phân nhánh giao diện theo `manager`, `admin`, `user`.
- [x] Đăng xuất có hộp thoại xác nhận.

### Sách
- [x] Tìm kiếm sách.
- [x] Thêm, sửa, xóa sách.
- [x] Sắp xếp/lọc ở tầng nghiệp vụ.
- [x] Gợi ý sách liên quan.
- [x] Tra cứu vị trí kệ.

### Mượn/trả
- [x] Đăng ký yêu cầu mượn từ độc giả.
- [x] Duyệt yêu cầu và tạo phiếu mượn.
- [x] Từ chối yêu cầu, lưu trạng thái `Tu choi`.
- [x] Trả sách và hoàn kho.
- [x] Kiểm tra quá hạn.
- [x] Lọc phiếu mượn theo khoảng ngày.
- [x] Tách truy vấn sách đang mượn và lịch sử ở tầng Service.

### Thông báo
- [x] Model và Service thông báo.
- [x] Lưu/đọc `Data/notification.txt`.
- [x] Tạo thông báo khi duyệt yêu cầu.
- [x] Tạo thông báo khi từ chối yêu cầu, có thể kèm lý do.
- [x] Đếm thông báo chưa đọc.
- [x] Đánh dấu thông báo đã đọc.
- [x] Có trang thông báo và chuông trên thanh đầu trang cho độc giả.

### Ca trực và lương
- [x] Đăng ký/hủy ca.
- [x] Xóa ca.
- [x] Xem lịch ca.
- [x] Tính lương theo số ca.

### Cấu trúc dữ liệu
- [x] Linked list.
- [x] Queue.
- [x] Stack.
- [x] Hash table.
- [x] Graph và thuật toán BFS.
- [x] Các thuật toán tìm kiếm/sắp xếp.

## 3. Chưa hoàn thiện hoặc cần xác minh

- [ ] Kiểm thử toàn bộ luồng trên GUI với cả ba vai trò.
- [ ] Kiểm thử biên cho đăng ký, duyệt, từ chối, trả sách, quá hạn và xóa dữ liệu.
- [ ] Chặn đăng ký trùng yêu cầu đang chờ cho cùng độc giả và cùng đầu sách. `register_book_request()` hiện chưa kiểm tra yêu cầu trùng trong hàng đợi.
- [ ] Xử lý trường hợp yêu cầu đầu hàng đợi không thể duyệt. `approve_next_request()` hiện trả `false` mà không lấy yêu cầu lỗi khỏi đầu hàng, có thể khiến yêu cầu sau bị chặn cho đến khi xử lý yêu cầu đầu.
- [ ] Kiểm tra tính nhất quán giữa `request.txt`, `borrow.txt` và thông báo sau các tình huống lỗi hoặc thoát đột ngột.
- [ ] Triển khai Undo người dùng và Undo phiếu mượn. Hai hàm hiện trả `false`.
- [ ] Rà soát kiểm tra quyền ở tầng Controller/Service, không chỉ ẩn/hiện menu GUI.
- [ ] Rà soát bảo vệ dữ liệu đăng nhập. `user.txt` hiện lưu mật khẩu dạng văn bản thuần.
- [ ] Kiểm thử nhập tiếng Việt có dấu trên môi trường Windows thực tế.
- [ ] Xác minh build sạch trên máy/môi trường mục tiêu.
- [ ] Kiểm tra xử lý dữ liệu lỗi, dòng trống, ngày sai định dạng và file không mở được.

## 4. Vấn đề kỹ thuật đã xác định từ mã nguồn

### P1. Cho phép đăng ký yêu cầu trùng
Vị trí: `Controller/library_controller.cpp`, hàm `register_book_request()`.

Hiện tại hàm kiểm tra sách tồn tại, số lượng, ngày, đang mượn trùng, quá hạn và giới hạn 7 phiếu đang mượn. Chưa thấy bước kiểm tra hàng đợi có yêu cầu `Cho duyet` cùng `user_id` và `book_id`.

### P1. Yêu cầu lỗi có thể chặn FIFO
Vị trí: `Controller/library_controller.cpp`, hàm `approve_next_request()`.

Nếu `borrow_book()` trả về `false`, hàm trả `false` và giữ nguyên phần tử đầu hàng đợi. Cần thống nhất cách xử lý: báo lỗi để quản trị viên có thể từ chối/xóa yêu cầu, hoặc cung cấp thao tác bỏ qua có kiểm soát. Không tự động pop khi chưa xác định quy tắc nghiệp vụ.

### P2. Undo chưa hoàn chỉnh
Vị trí: `Controller/library_controller.cpp`.

- `undo_user_action()` trả về `false`.
- `undo_borrow_action()` trả về `false`.
- Chỉ Undo sách có phần xử lý cụ thể.

### P2. Bảo mật dữ liệu mẫu
Vị trí: `Data/user.txt`.

Mật khẩu được ghi trực tiếp vào file. Không dùng mật khẩu cá nhân hoặc mật khẩu dùng lại ở dịch vụ khác. Nếu dự án cần mức bảo mật cao hơn, cần thiết kế lưu hash mật khẩu và quy trình chuyển đổi dữ liệu.

### P2. Kiểm tra quyền nghiệp vụ
GUI phân nhánh menu theo role, nhưng cần rà soát các hàm Controller để đảm bảo thao tác nhạy cảm không thể được gọi trái quyền từ một luồng khác.

## 5. Nhiệm vụ đang làm

**Bổ sung danh sách yêu cầu mượn đang chờ duyệt ngay trong mục “Đăng ký mượn sách” của role `user`.**

Quyết định nghiệp vụ/UI đã thống nhất:
- Không tạo mục sidebar hoặc page enum mới.
- Thêm nút phụ `Danh sách yêu cầu (N)` trong màn hình `USER_REQUEST`; `N` là số yêu cầu `Cho duyet` của đúng tài khoản đang đăng nhập.
- Nhấn nút sẽ chuyển nội dung trong cùng màn hình sang bảng danh sách yêu cầu; có nút quay lại phần đăng ký.
- Chỉ hiển thị yêu cầu đang chờ admin duyệt. Yêu cầu đã duyệt/từ chối tiếp tục được theo dõi ở các khu vực mượn, lịch sử và thông báo hiện có.
- Tái sử dụng `book_request_queue` và `Data/request.txt`, không tạo Model hay tệp dữ liệu mới.

Đã bổ sung mã nguồn trong `Controller/library_controller.{h,cpp}` và `GUI/library_gui.{h,cpp}`. Kiểm thử độc lập tầng Controller đã PASS: lọc đúng 3 yêu cầu của `US06`, không trả yêu cầu cho `US01`, giữ thứ tự `REQ6 → REQ7 → REQ8` và không làm thay đổi tổng số phần tử Queue. Chưa build được GUI trong môi trường hiện tại vì thiếu `raylib.h`; cần build/chạy trên Windows/MSYS2 UCRT64 trước khi đánh dấu hoàn tất toàn chức năng.

## 6. Thứ tự ưu tiên đề xuất

1. Xác minh luồng chuông/thông báo và trạng thái role bằng cách tái hiện lỗi.
2. Chốt quy tắc chống đăng ký trùng yêu cầu.
3. Chốt cách xử lý yêu cầu đầu hàng đợi không thể duyệt.
4. Kiểm thử nhất quán `request.txt` / `borrow.txt` / `notification.txt`.
5. Hoàn thiện các chức năng còn thiếu và kiểm thử toàn hệ thống.
6. Rà soát bảo mật, xử lý lỗi file và dữ liệu đầu vào.

## 7. Quy tắc cập nhật tiến độ

- Chỉ đánh dấu `[x]` khi chức năng có mã nguồn và đã được kiểm tra theo luồng phù hợp.
- Nếu mới thấy hàm hoặc màn hình trong source nhưng chưa chạy thử, ghi “đã có mã nguồn, chưa xác minh”.
- Mỗi lỗi cần ghi: bước tái hiện, kết quả mong đợi, kết quả thực tế, file/hàm liên quan.
- Sau mỗi nhiệm vụ, cập nhật `Current Task`, `Known Issues` và `CHANGELOG.md`.
