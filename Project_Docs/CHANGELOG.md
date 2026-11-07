# Changelog

## 2026-11-07 - Đối chiếu source GitHub với ZIP và đồng bộ tài liệu

- ZIP có HEAD `9e1474906b635f1f07cae4c2e04c3303b53ea7a2`; source mới nhất trên `main` là `bd3e598b91fed16b0ee8d517ac6f9a29134a8230`, gồm 2 commit source sau ZIP.
- Sau commit source đó, các commit tiếp theo trên nhánh chỉ cập nhật `Project_Docs/`, không thêm chức năng ứng dụng.
- Xác nhận các thay đổi source: kéo dài phần header/sidebar/panel để lấp vùng trống; xử lý ngày nhập không hợp lệ bằng ngày dự phòng và kiểm tra lại trong `library_gui::read_date()`.
- Chưa build/chạy GUI hoặc kiểm thử runtime. Vì vậy layout và xử lý ngày chỉ được ghi nhận là đã chỉnh source, chưa xác nhận hoàn thành.
- Các mục tra cứu người dùng kèm sách đang mượn, thông báo lỗi trùng rõ nguyên nhân và nhập Telex `thawngs` vẫn còn thiếu.
- Kiến trúc tổng thể không thay đổi; `PROJECT_OVERVIEW.md` chỉ cập nhật mốc đối chiếu, không mô tả kiến trúc mới.

## 2026-11-07 - Đối chiếu GitHub sau snapshot ZIP

- ZIP `PBL2_GUI_test.zip` có Git HEAD `9e1474906b635f1f07cae4c2e04c3303b53ea7a2`; GitHub `main` hiện ở `bd3e598b91fed16b0ee8d517ac6f9a29134a8230` (`full manf hinh`), đi trước 2 commit.
- So sánh hai mốc cho thấy khác biệt ròng ở `Data/borrow.txt`, `Data/request.txt`, `GUI/library_gui.cpp`, `GUI/ui_layout.h` và `Model/date.cpp`. Dữ liệu mẫu yêu cầu/phiếu mượn cũng đã thay đổi; không xem thay đổi dữ liệu mẫu là chức năng nghiệp vụ mới.
- Layout: `GUI/library_gui.cpp` kéo dài header, sidebar và panel theo phần không gian thừa; `GUI/ui_layout.h` tiếp tục dùng tỷ lệ scale và offset để giữ tỷ lệ bản vẽ. Đã có chỉnh sửa source, chưa build/chạy GUI để xác minh kết quả ở các kích thước cửa sổ.
- Ngày: `Model/date.cpp` đổi constructor để gán ngày dự phòng `1/1/2026` khi đầu vào không hợp lệ; `library_gui::read_date()` so sánh lại ngày/tháng/năm và trả `false` nếu constructor đã thay giá trị. Chưa chạy test runtime; cần chú ý constructor có thể che giấu dữ liệu ngày sai ở các luồng khác.
- Tra cứu người dùng: `draw_users()` vẫn chỉ hiển thị ID, tên và vai trò; chưa có danh sách sách đang mượn kèm trạng thái.
- Đăng ký trùng: Controller đã chặn yêu cầu đang chờ trùng theo người dùng và sách, nhưng GUI vẫn báo thất bại chung, chưa nêu đúng nguyên nhân.
- Đăng nhập: `GUI/login.cpp` vẫn chỉ nhận codepoint ASCII 32-126; chưa có thay đổi đáp ứng/kiểm chứng yêu cầu giữ nguyên chuỗi Telex ASCII `thawngs`.
- Không có thay đổi kiến trúc tổng thể. Lần đối chiếu này không chạy build hoặc test GUI; không đánh dấu các chỉnh sửa mới là đã kiểm thử.

## 2026-11-07 - Xác nhận chức năng và cập nhật backlog

- Người dùng xác nhận chức năng chặn yêu cầu mượn trùng và lọc ký tự tiếng Việt có dấu ở đăng nhập hoạt động ổn.
- Ghi nhận yêu cầu tinh chỉnh bộ nhập: chuỗi Telex ASCII như `thawngs` phải được giữ nguyên; không nhận ký tự Unicode có dấu.
- Cập nhật thứ tự ưu tiên: (1) tra cứu người dùng kèm sách đang mượn và trạng thái, (2) thông báo khi đăng ký trùng, (3) layout ôm trọn khung hình, (4) xử lý ngày sai không crash, (5) tinh chỉnh nhập Telex.
- Đây là cập nhật tài liệu/backlog; chưa có thay đổi mã nguồn cho các nhiệm vụ mới.

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
