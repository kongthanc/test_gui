# PBL2 - Gói bàn giao cho cuộc trò chuyện mới

> Dùng file này khi mở cuộc trò chuyện mới để tiếp tục dự án mà không phải kể lại từ đầu.

## 1. Bản nguồn

- Dự án: `PBL2_GUI_test`.
- Snapshot đã rà soát: `PBL2_GUI_test(8).zip`.
- Ngày snapshot: 06/11/2026.
- Git HEAD trong ZIP: `5679f10` (`Nút thông báo + Lí do từ chối duyệt`).
- Trạng thái Git trong ZIP: branch `main` đang ở `origin/main`, nhưng working tree có nhiều file sửa đổi và `Project_Docs/` chưa được Git theo dõi tại snapshot. Không reset/checkout hoặc ghi đè các thay đổi này.

## 2. Quy tắc khi hỗ trợ code

1. Đọc source trong ZIP/repo mới nhất trước khi kết luận hoặc sửa.
2. Chỉ rõ file, hàm và vị trí.
3. Ưu tiên đưa nguyên hàm/đoạn code hoàn chỉnh để thay thế.
4. Giữ comment hiện có, chỉ thêm/xóa khi thật sự cần.
5. Không tự ý thay đổi quy tắc nghiệp vụ đã thống nhất.
6. Không yêu cầu người dùng gửi lại toàn bộ code nếu ZIP hiện tại đã chứa file cần thiết.
7. Sau sửa đổi, đưa lệnh build và các bước kiểm thử cụ thể.
8. Không đánh dấu hoàn thành khi chưa có kết quả kiểm thử.
9. Tuyệt đối không reset hoặc ghi đè thay đổi chưa commit.

## 3. Quyết định nghiệp vụ quan trọng đã thống nhất

- Yêu cầu mượn có mã tự sinh dạng `REQ...`.
- Yêu cầu được lưu trong `Data/request.txt`.
- Khi duyệt thành công, yêu cầu được chuyển thành phiếu mượn trong `Data/borrow.txt`.
- Khi từ chối, yêu cầu được ghi nhận trạng thái `Tu choi` trong dữ liệu borrow/history.
- Lý do từ chối chỉ cần nhập khi người quản trị bấm nút từ chối.
- Độc giả xem riêng sách đang mượn và lịch sử:
  - Đang mượn: `Dang muon`, `Qua han`.
  - Lịch sử: `Da tra`, `Tu choi`.
- Thông báo duyệt/từ chối được gửi đến đúng độc giả và lưu trong `Data/notification.txt`.
- Thông báo có trạng thái đã đọc/chưa đọc.
- Chuông thông báo dành cho vai trò `user`; huy hiệu thể hiện số chưa đọc.
- Danh sách yêu cầu đang chờ duyệt nằm bên trong mục `Đăng ký mượn sách`, không tạo mục sidebar riêng. Nút `Danh sách yêu cầu (N)` đếm các yêu cầu `Cho duyet` của tài khoản hiện tại; phần danh sách chỉ hiển thị các yêu cầu này và có nút quay lại đăng ký. Không tạo file dữ liệu hoặc Model mới.

## 4. Công việc mới đã triển khai trong source

- `Controller/library_controller.h/.cpp`: thêm `get_my_pending_requests(user_id)`, lọc trạng thái `Cho duyet` theo đúng User ID và duyệt một vòng queue để giữ nguyên thứ tự FIFO.
- `GUI/library_gui.h/.cpp`: thêm trạng thái hiển thị danh sách trong chính `USER_REQUEST`; nút `Danh sách yêu cầu (N)`; bảng mã yêu cầu, tên sách, ngày mượn, hạn trả; phân trang và nút quay lại đăng ký.
- Kiểm thử độc lập tầng Controller đã PASS với dữ liệu mẫu: lọc đúng yêu cầu theo User ID, chỉ lấy `Cho duyet`, giữ nguyên FIFO và không làm thay đổi kích thước queue.
- Chưa xác nhận build/chạy GUI do môi trường hiện tại thiếu `raylib.h`. Cần kiểm tra sau khi tích hợp vào môi trường phát triển Windows/MSYS2 UCRT64.

## 5. Vấn đề đang cần xác minh

- Lỗi chuông thông báo biến mất sau khi đăng ký hai yêu cầu cùng một sách đã được người dùng xác nhận là đã sửa; không tiếp tục điều tra nếu chưa có báo lỗi mới.
- `register_book_request()` chưa chặn yêu cầu đang chờ trùng cùng user/sách.
- `approve_next_request()` có thể giữ yêu cầu lỗi ở đầu queue nếu `borrow_book()` thất bại.
- Chức năng danh sách yêu cầu chờ duyệt vừa được thêm vào source nhưng chưa build/chạy GUI xác minh do môi trường hiện tại thiếu `raylib.h`.

## 6. Cách bắt đầu phiên làm việc

Đính kèm ZIP mới nhất và các file trong `Project_Docs/`, sau đó yêu cầu:

> Tiếp tục dự án PBL2 theo `HANDOFF.md`, `PROJECT_OVERVIEW.md` và `PROGRESS.md`. Hãy đọc đúng source trong ZIP mới nhất trước khi đề xuất sửa. Không reset hoặc ghi đè thay đổi chưa commit. Nhiệm vụ hiện tại của tôi là: [ghi nhiệm vụ].

## 7. Lưu ý dữ liệu

`Data/user.txt` hiện chứa mật khẩu dạng văn bản thuần. Chỉ dùng dữ liệu giả lập. Không đưa mật khẩu cá nhân hoặc thông tin đăng nhập thật vào ZIP chia sẻ.
