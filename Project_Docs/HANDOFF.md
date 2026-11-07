# PBL2 - Gói bàn giao cho cuộc trò chuyện mới

> Dùng file này khi mở cuộc trò chuyện mới để tiếp tục dự án mà không phải kể lại từ đầu.

## 1. Bản nguồn

- Dự án: `PBL2_GUI_test`.
- Snapshot người dùng vừa gửi: `PBL2_GUI_test.zip`.
- Git HEAD bên trong ZIP: `9e1474906b635f1f07cae4c2e04c3303b53ea7a2` (`Chống đăng ký trùng sách + Login no Vietnamese`).
- GitHub repository: [`kongthanc/test_gui`](https://github.com/kongthanc/test_gui).
- Nhánh mặc định: `main`; source mới nhất tại lần đối chiếu ngày 07/11/2026: `bd3e598b91fed16b0ee8d517ac6f9a29134a8230` (`full manf hinh`). Sau commit này có 4 commit chỉ cập nhật tài liệu.
- So với ZIP HEAD `9e1474906b635f1f07cae4c2e04c3303b53ea7a2`, GitHub có 2 commit source mới. Thay đổi ròng nằm ở `Data/borrow.txt`, `Data/request.txt`, `GUI/library_gui.cpp`, `GUI/ui_layout.h`, `Model/date.cpp`; dữ liệu mẫu cũng thay đổi.
- Không suy đoán trạng thái working tree local từ snapshot; không reset/checkout hoặc ghi đè thay đổi local chưa commit.

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

## 5. Trạng thái và backlog tiếp theo

Người dùng đã xác nhận các chức năng sau hoạt động ổn:
- Chặn một độc giả gửi nhiều yêu cầu đang chờ cho cùng một đầu sách.
- Không nhận ký tự tiếng Việt có dấu trong ô đăng nhập.
- Danh sách yêu cầu đang chờ duyệt hiển thị trong mục `Đăng ký mượn sách`.

Kết quả rà soát GitHub mới nhất:
- **Tra cứu người dùng:** hiện chỉ tra cứu ID và hiển thị tên/vai trò; chưa hiển thị sách đang mượn và trạng thái từng phiếu.
- **Thông báo đăng ký trùng:** Controller đã chặn yêu cầu trùng, nhưng GUI vẫn báo lỗi chung. Cần tách nguyên nhân nghiệp vụ để hiển thị đúng.
- **Layout:** `GUI/library_gui.cpp` kéo dài header/sidebar/panel để lấp vùng trống; `GUI/ui_layout.h` có cập nhật định dạng/comment quanh cơ chế scale/offset. Chưa build/chạy xác minh; cần kiểm tra màn hình đăng nhập và nhiều tỷ lệ cửa sổ.
- **Ngày sai:** `Model/date.cpp` đã gán ngày dự phòng khi constructor nhận ngày không hợp lệ; `library_gui::read_date()` phát hiện giá trị bị thay và trả lỗi. Chưa test runtime; xem xét tránh âm thầm thay ngày ở các luồng khác.
- **Telex đăng nhập:** `GUI/login.cpp` vẫn lọc codepoint ngoài ASCII; chưa đáp ứng/kiểm chứng yêu cầu gõ `thawngs` phải giữ nguyên từng ký tự ASCII.

Thứ tự công việc tiếp theo:
1. Tra cứu người dùng và hiển thị các sách người đó đang mượn cùng trạng thái `Dang muon`/`Qua han`.
2. Hiển thị đúng nguyên nhân khi đăng ký mượn trùng.
3. Build/chạy và hoàn thiện layout toàn cửa sổ.
4. Kiểm thử ngày sai, ngày nhuận và rà soát cách xử lý ngày dự phòng.
5. Sửa bộ nhập đăng nhập để giữ nguyên chuỗi Telex ASCII, không nhận Unicode có dấu.

Các việc kỹ thuật dài hạn vẫn còn: xử lý yêu cầu lỗi ở đầu FIFO, kiểm tra nhất quán dữ liệu, Undo người dùng/phiếu mượn, rà soát quyền và bảo mật mật khẩu.

## 6. Cách bắt đầu phiên làm việc

Đính kèm ZIP mới nhất và các file trong `Project_Docs/`, sau đó yêu cầu:

> Tiếp tục dự án PBL2 theo `HANDOFF.md`, `PROJECT_OVERVIEW.md` và `PROGRESS.md`. Hãy đọc đúng source trong ZIP mới nhất trước khi đề xuất sửa. Không reset hoặc ghi đè thay đổi chưa commit. Nhiệm vụ hiện tại của tôi là: [ghi nhiệm vụ].

## 7. Lưu ý dữ liệu

`Data/user.txt` hiện chứa mật khẩu dạng văn bản thuần. Chỉ dùng dữ liệu giả lập. Không đưa mật khẩu cá nhân hoặc thông tin đăng nhập thật vào ZIP chia sẻ.
