## 2026-11-10 - Undo User/Borrow, validation ngày và responsive Login

### Thay đổi được xác nhận trong source ZIP

- `Model/date.cpp`: constructor `date(int, int, int)` kiểm tra `is_valid()` và ném `std::invalid_argument` khi ngày không hợp lệ, thay vì âm thầm thay ngày bằng `01/01/2026`.
- `GUI/library_gui.cpp`: `library_gui::read_date()` bọc khởi tạo `date` bằng `try-catch`; bắt `std::invalid_argument` và trả về `false`.
- `GUI/login.cpp`: `login_screen::draw_input_box()` chuyển tọa độ vùng cắt bằng `GetWorldToScreen2D()` trước khi gọi `BeginScissorMode()`, đồng bộ với Camera2D.
- `Service/borrow_service.h/.cpp`: `create_borrow_ticket()` nhận `std::string& out_message` và trả lý do thất bại chi tiết thay vì chỉ cung cấp kết quả boolean.
- `Controller/library_controller.h/.cpp`: `borrow_book()` nhận và chuyển tiếp `out_message`; ghi log thao tác tạo phiếu vào `borrow_undo_stack`.
- `Controller/library_controller.cpp`: triển khai `undo_user_action()` cho thao tác ADD/DELETE/UPDATE; các luồng thêm/sửa/xóa tài khoản ghi log vào `user_undo_stack`.
- `Service/borrow_service.h/.cpp`: bổ sung `undo_create_borrow()` và `undo_delete_borrow()`; Undo tạo phiếu sẽ cộng lại số lượng sách, Undo xóa phiếu sẽ khôi phục phiếu.
- `Controller/library_controller.cpp` và `GUI/library_gui.cpp`: tích hợp Undo Borrow và Undo User với nút Hoàn tác tương ứng trên giao diện.

### Quyết định thiết kế

- Không triển khai Hash Password trong phạm vi hiện tại; mật khẩu tiếp tục được lưu dạng văn bản gốc theo báo cáo bàn giao.
- Giữ nguyên giới hạn nhập ASCII tại màn hình Login; không triển khai hỗ trợ gõ tiếng Việt có dấu.

### Trạng thái xác minh

- Các thay đổi trên được xác nhận có trong source ZIP.
- Chưa có bằng chứng build GUI hoặc kiểm thử runtime cho các thay đổi trong phiên cập nhật tài liệu này. Cần kiểm thử Undo từng loại thao tác, validation ngày, responsive Login và tính nhất quán dữ liệu sau Undo trước khi đánh dấu đã kiểm thử.

# Changelog

## 2026-11-08 - Kiểm thử thủ công hàng đợi yêu cầu mượn và ràng buộc sách/tài khoản

### Kết quả người dùng xác nhận

- TC01: Không cho xóa sách có yêu cầu mượn đang chờ duyệt.
- TC02: Không cho đổi ID sách khi có yêu cầu chờ; thông báo hiện tại chưa nêu rõ nguyên nhân.
- TC03: Cho phép sửa tên sách khi giữ nguyên ID; cần rà soát thêm logic nghiệp vụ.
- TC04: Không cho xóa tài khoản có yêu cầu chờ; thông báo chưa rõ nguyên nhân.
- TC05: Không cho đổi ID tài khoản có yêu cầu chờ; thông báo chưa rõ nguyên nhân.
- TC06: Cho phép sửa thông tin tài khoản khi giữ nguyên ID; cần rà soát thêm logic nghiệp vụ.
- TC07: Xóa được sách không có yêu cầu chờ và không có người mượn.
- TC08: Chưa kiểm thử do chưa có dữ liệu phù hợp.
- TC09: Chưa kiểm thử FIFO với nhiều yêu cầu liên tiếp.
- TC10: Hàng đợi rỗng không phát sinh lỗi trong trường hợp đã thử.
- TC11: Duyệt yêu cầu hợp lệ thành công.
- TC12: Duyệt yêu cầu thất bại thì yêu cầu vẫn ở đầu hàng đợi; thông báo bị tràn/cắt khỏi vùng hiển thị.
- TC13: Từ chối yêu cầu thành công; nội dung lý do dài không hiển thị đầy đủ trong ô nhập.
- TC14: Yêu cầu đang chờ được khôi phục sau khi khởi động lại ứng dụng.

### Việc chưa thực hiện

- Không sửa source code trong phiên này.
- Không thay đổi kiến trúc, cấu trúc dữ liệu hoặc định dạng tệp.
- Không chạy build hoặc kiểm thử độc lập từ môi trường trợ lý.
- Không xác nhận commit/branch của bản local đã được kiểm thử.

### Nhiệm vụ ưu tiên cao nhất cho phiên tiếp theo

1. Cải thiện thông báo nguyên nhân khi thao tác đổi ID/xóa sách hoặc tài khoản bị chặn.
2. Sửa cách hiển thị thông báo lỗi dài để không bị tràn/cắt.
3. Sửa cách nhập và xem lý do từ chối dài để nội dung không bị khuất hoặc mất.
4. Rà soát logic sửa sách/tài khoản khi giữ nguyên ID và ảnh hưởng đến yêu cầu đang chờ.
5. Sau khi sửa, kiểm thử lại các trường hợp TC01-TC14 liên quan và bổ sung test cho nội dung dài, UTF-8, ID trùng và cập nhật dữ liệu.

## 2026-11-07 - Chuẩn hóa mã yêu cầu mượn về REQ1, REQ2...

- Chuẩn hóa các mã trong `Data/request.txt` từ `REQ01`...`REQ04` thành `REQ1`...`REQ4`.
- Không cần sửa `Controller/library_controller.cpp`: `generate_request_id()` hiện đã trả về `"REQ" + std::to_string(max_id + 1)`, đúng định dạng không đệm số 0.
- Đã kiểm tra `Data/borrow.txt` trên GitHub tại thời điểm đối chiếu không chứa mã `REQ` bị đệm số 0 hoặc mã REQ trùng với bốn yêu cầu đang chờ.
- Sau khi áp dụng dữ liệu này, yêu cầu mới tiếp theo sẽ được sinh là `REQ5`.
- Chỉ chuẩn bị file thay đổi cục bộ để người dùng áp dụng; không commit, push hoặc sửa trực tiếp trên GitHub.
