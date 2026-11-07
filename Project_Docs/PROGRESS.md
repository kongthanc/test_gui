# Tiến độ dự án PBL2

> Đối chiếu ngày 07/11/2026: ZIP HEAD `9e14749`; GitHub `main` HEAD `bd3e598`.
> `[x]` = đã được xác nhận/kiểm thử phù hợp; `[~]` = đã có chỉnh sửa source nhưng chưa xác minh runtime; `[ ]` = còn thiếu.

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
- [x] Chặn cùng một độc giả gửi nhiều yêu cầu đang chờ cho cùng một đầu sách (người dùng đã xác nhận hoạt động ổn).
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

- [ ] **P1:** Tra cứu người dùng và hiển thị sách người đó đang mượn kèm trạng thái từng phiếu (`Dang muon`, `Qua han`). Hiện `draw_users()` chỉ hiển thị ID, tên và vai trò.
- [ ] **P1:** Đổi thông báo khi đăng ký mượn trùng để nêu rõ nguyên nhân. Logic chặn trùng đã có, nhưng GUI vẫn báo chung: `Gửi yêu cầu thất bại. Kiểm tra thông tin mượn sách.`
- [~] **P1:** Layout: `GUI/library_gui.cpp` và `GUI/ui_layout.h` đã được chỉnh để kéo header/sidebar/khung nội dung lấp vùng trống. Chưa build/chạy để xác nhận ở nhiều tỷ lệ cửa sổ; chưa xác minh toàn bộ màn hình đăng nhập.
- [~] **P1:** Ngày không hợp lệ: `Model/date.cpp` đã thay ngày sai bằng ngày dự phòng; `library_gui::read_date()` kiểm tra lại và trả `false` khi giá trị không khớp. Chưa chạy test ngày 31/02, tháng 13, năm 0, ngày nhuận; constructor hiện có thể âm thầm thay ngày sai ở luồng khác.
- [ ] **P2:** Tinh chỉnh đăng nhập để chuỗi Telex ASCII như `thawngs` luôn được giữ nguyên trong lúc gõ, đồng thời không nhận Unicode có dấu. `GUI/login.cpp` hiện vẫn lọc codepoint ngoài ASCII, chưa chứng minh đáp ứng hành vi Telex yêu cầu.
- [ ] Kiểm thử toàn bộ luồng trên GUI với cả ba vai trò.
- [ ] Kiểm thử biên cho đăng ký, duyệt, từ chối, trả sách, quá hạn và xóa dữ liệu.
- [ ] Xử lý trường hợp yêu cầu đầu hàng đợi không thể duyệt; cần quy tắc rõ ràng để tránh yêu cầu lỗi chặn FIFO.
- [ ] Kiểm tra tính nhất quán giữa `request.txt`, `borrow.txt` và thông báo sau các tình huống lỗi hoặc thoát đột ngột.
- [ ] Triển khai Undo người dùng và Undo phiếu mượn. Hai hàm hiện trả `false`.
- [ ] Rà soát kiểm tra quyền ở tầng Controller/Service, không chỉ ẩn/hiện menu GUI.
- [ ] Rà soát bảo vệ dữ liệu đăng nhập. `user.txt` hiện lưu mật khẩu dạng văn bản thuần.
- [ ] Xác minh build sạch trên máy/môi trường mục tiêu.
- [ ] Kiểm tra xử lý dữ liệu lỗi, dòng trống và file không mở được.

## 4. Vấn đề kỹ thuật đã xác định từ mã nguồn

### P1. Thông báo khi đăng ký trùng chưa đủ rõ
Vị trí: `Controller/library_controller.cpp`, hàm `register_book_request()`; `GUI/library_gui.cpp`, hàm `draw_user_request()`.

Controller đã duyệt hàng đợi và chặn cùng `user_id` + `book_id` khi yêu cầu trước vẫn ở trạng thái `Cho duyet`. Tuy nhiên hàm chỉ trả về `false`; GUI gộp nhiều nguyên nhân thất bại thành một thông báo chung. Cần trả mã/kết quả nghiệp vụ cụ thể để GUI nói đúng lý do.

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

Người dùng đã xác nhận các chức năng sau hoạt động ổn:
- Chặn một độc giả gửi nhiều yêu cầu đang chờ cho cùng một đầu sách.
- Không nhận ký tự tiếng Việt có dấu trong ô đăng nhập.
- Danh sách yêu cầu đang chờ duyệt hiển thị ngay trong mục `Đăng ký mượn sách`.

Đối chiếu GitHub mới nhất:
- ZIP được gửi có HEAD `9e14749`; GitHub `main` hiện ở `bd3e598`, đi trước 2 commit.
- Đã có thay đổi source cho layout và xử lý ngày không hợp lệ, nhưng chưa có kết quả build/chạy GUI nên chỉ ghi nhận là đã chỉnh source, chưa xác minh.
- Tra cứu người dùng hiện mới hiển thị thông tin tài khoản, chưa liệt kê sách đang mượn.
- Thông báo lỗi đăng ký trùng vẫn là thông báo chung.
- Bộ nhập đăng nhập vẫn lọc codepoint ngoài ASCII; yêu cầu giữ nguyên chuỗi Telex ASCII chưa được xác minh/đáp ứng.

**Nhiệm vụ ưu tiên tiếp theo: tra cứu người dùng và sách đang mượn.** Khi tra cứu một người dùng, giao diện cần hiển thị từng sách người đó đang mượn cùng trạng thái hiện tại (`Dang muon`/`Qua han`).
## 6. Thứ tự ưu tiên thực hiện

1. **Tra cứu người dùng và sách đang mượn:** hiển thị sách cùng trạng thái `Dang muon`/`Qua han` theo người dùng được chọn.
2. **Thông báo đăng ký trùng:** đổi nội dung phản hồi để nêu rõ yêu cầu/sách đã tồn tại và lý do không thể gửi thêm.
3. **Bố cục giao diện:** đã có chỉnh sửa source để kéo dài header/sidebar/khung nội dung; cần build và kiểm tra nhiều kích thước cửa sổ.
4. **Ngày không hợp lệ:** đã có chỉnh sửa ở `Model/date.cpp` và `library_gui::read_date()`; cần test biên và rà soát việc constructor tự thay ngày sai bằng ngày mặc định.
5. **Nhập Telex ở đăng nhập:** giữ nguyên chuỗi ASCII đang gõ như `thawngs`, đồng thời không nhận ký tự Unicode có dấu.
6. Kiểm thử toàn bộ luồng, xử lý lỗi dữ liệu/file và rà soát quyền.
7. Hoàn thiện Undo, nhất quán dữ liệu và bảo mật mật khẩu.

## 7. Quy tắc cập nhật tiến độ

- Chỉ đánh dấu `[x]` khi chức năng có mã nguồn và đã được kiểm tra theo luồng phù hợp.
- Nếu mới thấy hàm hoặc màn hình trong source nhưng chưa chạy thử, ghi “đã có mã nguồn, chưa xác minh”.
- Mỗi lỗi cần ghi: bước tái hiện, kết quả mong đợi, kết quả thực tế, file/hàm liên quan.
- Sau mỗi nhiệm vụ, cập nhật `Current Task`, `Known Issues` và `CHANGELOG.md`.
