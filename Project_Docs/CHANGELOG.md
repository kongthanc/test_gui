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
