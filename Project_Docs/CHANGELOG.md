# Changelog

## 2026-11-07 - Danh sách yêu cầu chờ duyệt trong mục đăng ký mượn

- `Controller/library_controller.h/.cpp`: thêm `get_my_pending_requests(user_id)` để lọc các yêu cầu `Cho duyet` của một độc giả; duyệt queue đúng một vòng và giữ nguyên FIFO.
- `GUI/library_gui.h/.cpp`: thêm nút `Danh sách yêu cầu (N)` ngay trong `USER_REQUEST`; N lấy từ số yêu cầu chờ duyệt của tài khoản hiện tại.
- Khi nhấn nút, hiển thị danh sách mã yêu cầu, tên sách, ngày mượn và hạn trả; hỗ trợ phân trang và quay lại màn hình đăng ký.
- Không thêm mục sidebar, page enum, Model hoặc tệp dữ liệu mới.
- Kiểm thử độc lập tầng Controller: PASS với lọc theo User ID, trạng thái `Cho duyet`, giữ nguyên FIFO và kích thước queue.
- Chưa build/chạy GUI được xác minh trong môi trường hiện tại vì thiếu `raylib.h`; cần kiểm thử với Raylib trên Windows/MSYS2 UCRT64.

## 2026-11-06 - Rà soát snapshot `PBL2_GUI_test(8).zip`

- Đối chiếu tài liệu cũ với cấu trúc thư mục, header, các hàm Controller, Service và GUI.
- Ghi rõ luồng đăng nhập, phân quyền, sách, mượn/trả, hàng đợi yêu cầu, thông báo, ca trực, lương và vị trí kệ.
- Bổ sung định dạng bản ghi của các tệp dữ liệu.
- Ghi nhận thông báo đã được nối vào luồng duyệt/từ chối yêu cầu và có đánh dấu đã đọc.
- Ghi nhận `undo_user_action()` và `undo_borrow_action()` hiện chưa triển khai.
- Ghi nhận `register_book_request()` chưa chặn yêu cầu trùng trong hàng đợi.
- Ghi nhận `approve_next_request()` giữ yêu cầu ở đầu hàng nếu không tạo được phiếu mượn.
- Ghi nhận mật khẩu trong `Data/user.txt` đang được lưu dạng văn bản thuần.
- Phân biệt chức năng đã có mã nguồn với chức năng đã được kiểm thử thực tế.

## 2026-11-06 - Các thay đổi nghiệp vụ đã thể hiện trong mã nguồn snapshot

- Có hàng đợi yêu cầu mượn được lưu qua `Data/request.txt`.
- Mã yêu cầu được sinh tự động theo dạng `REQ` + số.
- Khi duyệt yêu cầu thành công, hệ thống tạo phiếu mượn và thông báo cho độc giả.
- Khi từ chối, hệ thống ghi lý do (nếu có) vào nội dung thông báo và lưu yêu cầu bị từ chối trong lịch sử phiếu mượn.
- Trang sách đang mượn lọc `Dang muon` và `Qua han`; trang lịch sử lọc `Da tra` và `Tu choi`.
- Có trang thông báo và nút chuông cho tài khoản độc giả.

## Quy tắc ghi changelog từ nay

Mỗi lần cập nhật, thêm mục mới ở đầu file theo mẫu:

```md
## YYYY-MM-DD - Tên nhiệm vụ
- Thay đổi cụ thể (file/hàm nếu hữu ích).
- Quy tắc nghiệp vụ được thêm hoặc thay đổi.
- Kiểm thử đã chạy và kết quả.
- Vấn đề còn tồn tại.
```

Không ghi một chức năng là “hoàn thành” nếu mới chỉ chỉnh code mà chưa chạy kiểm thử.
