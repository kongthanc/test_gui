# Tiến độ dự án PBL2

> Cập nhật ngày 10/11/2026 theo ZIP source và báo cáo bàn giao kỹ thuật. Các thay đổi source trong phiên 10/11 được đối chiếu trực tiếp trong ZIP; chưa có kết quả build hoặc kiểm thử runtime độc lập từ môi trường trợ lý.
> `[x]` = đã được xác nhận qua kiểm thử phù hợp; `[~]` = đã có mã nguồn/chỉnh sửa nhưng chưa xác minh đầy đủ; `[ ]` = chưa thực hiện hoặc chưa kiểm thử.

## 1. Trạng thái tổng quan

- Ứng dụng GUI Raylib có luồng đăng nhập, sidebar theo vai trò và các màn hình nghiệp vụ.
- Các tầng Model, Service, Controller, cấu trúc dữ liệu và thuật toán được tổ chức riêng.
- Dữ liệu được lưu bằng các tệp trong `Data/`.
- Chưa có bằng chứng về bộ kiểm thử tự động hoặc kiểm thử toàn hệ thống.
- Trong phiên 08/11, người dùng đã chạy kiểm thử thủ công các trường hợp liên quan đến yêu cầu mượn, thao tác sách/tài khoản và hàng đợi.
- Trong phiên 10/11, ZIP source có các thay đổi về Undo User/Borrow, validation ngày, thông báo lỗi tạo phiếu mượn và responsive Login. Các thay đổi này được xác nhận ở mức source, chưa có kết quả kiểm thử runtime trong phiên cập nhật tài liệu.

## 2. Chức năng và trạng thái kiểm thử

### Tài khoản và phân quyền
- [x] Đăng nhập qua ID/mật khẩu.
- [x] Phân nhánh giao diện theo `manager`, `admin`, `user`.
- [x] Đăng xuất có hộp thoại xác nhận.

### Sách
- [x] Tìm kiếm, thêm, sửa, xóa sách.
- [x] Sắp xếp/lọc ở tầng nghiệp vụ; gợi ý sách liên quan; tra cứu vị trí kệ.
- [x] TC01: Không cho xóa sách đang có yêu cầu mượn chờ duyệt.
- [x] TC02: Không cho đổi ID sách khi còn yêu cầu mượn chờ duyệt; thông báo chưa nêu rõ lý do.
- [x] TC03: Cho phép sửa tên sách khi giữ nguyên ID; cần rà soát thêm logic và tham chiếu trước khi chốt hoàn toàn quy tắc.

### Mượn/trả và hàng đợi
- [x] Đăng ký yêu cầu mượn từ độc giả.
- [x] Chặn cùng một độc giả gửi nhiều yêu cầu đang chờ cho cùng một đầu sách (đã được xác nhận ở phiên trước).
- [x] TC11: Duyệt yêu cầu hợp lệ và tạo phiếu mượn.
- [x] TC12: Khi duyệt thất bại do điều kiện không đáp ứng, yêu cầu vẫn nằm ở đầu hàng đợi.
- [x] TC13: Từ chối yêu cầu thành công và chuyển sang yêu cầu tiếp theo.
- [x] TC10: Hàng đợi rỗng không gây lỗi trong trường hợp đã kiểm tra.
- [x] TC14: Yêu cầu đang chờ được khôi phục sau khi đóng và mở lại chương trình.
- [ ] TC09: Chưa kiểm thử FIFO với nhiều yêu cầu liên tiếp.
- [ ] Chưa có kết quả kiểm thử toàn bộ tình huống lỗi ghi file/thoát đột ngột.

### Tài khoản
- [x] TC04: Không cho xóa tài khoản có yêu cầu đang chờ; thông báo chưa rõ lý do.
- [x] TC05: Không cho đổi ID tài khoản có yêu cầu đang chờ; thông báo chưa rõ lý do.
- [x] TC06: Cho phép sửa thông tin tài khoản khi giữ nguyên ID; cần rà soát thêm logic và tham chiếu.
- [x] TC07: Xóa được sách không có yêu cầu chờ và không có người mượn.
- [ ] TC08: Chưa kiểm thử xóa tài khoản không có yêu cầu chờ, phiếu mượn hoặc ca trực ràng buộc vì chưa có dữ liệu phù hợp.

### Thông báo
- [x] Model và Service thông báo; lưu/đọc `Data/notification.txt`.
- [x] Tạo thông báo khi duyệt hoặc từ chối yêu cầu, có thể kèm lý do.
- [x] Đếm thông báo chưa đọc và đánh dấu đã đọc.
- [x] Có trang thông báo và chuông trên thanh đầu trang cho độc giả.

### Ca trực, lương và cấu trúc dữ liệu
- [x] Đăng ký/hủy ca, xóa ca, xem lịch ca và tính lương theo số ca.
- [x] Linked list, Queue, Stack, Hash table, Graph và các thuật toán tìm kiếm/sắp xếp đã có trong source.

## 3. Kết quả kiểm thử thủ công ngày 08/11/2026

Các kết quả dưới đây được ghi theo báo cáo của người dùng, không phải kết quả chạy lại từ phía trợ lý.

| Mã | Tình huống | Kết quả người dùng báo cáo | Trạng thái |
|---|---|---|---|
| TC01 | Xóa sách có yêu cầu mượn đang chờ duyệt | Không cho xóa, đúng mong đợi | [x] |
| TC02 | Đổi ID sách có yêu cầu đang chờ | Không cho đổi; thông báo chỉ ghi “Cập nhật thất bại”, chưa nêu lý do | [x] hành vi chặn; [ ] thông báo |
| TC03 | Sửa tên sách, giữ nguyên ID | Cho phép sửa; cần rà soát logic nghiệp vụ và tham chiếu | [x] hành vi; [ ] rà soát |
| TC04 | Xóa tài khoản có yêu cầu đang chờ | Không cho xóa; thông báo chưa rõ lý do | [x] hành vi; [ ] thông báo |
| TC05 | Đổi ID tài khoản có yêu cầu đang chờ | Không cho đổi; thông báo chưa rõ lý do | [x] hành vi; [ ] thông báo |
| TC06 | Sửa thông tin tài khoản, giữ nguyên ID | Cho phép sửa; cần rà soát logic nghiệp vụ và tham chiếu | [x] hành vi; [ ] rà soát |
| TC07 | Xóa sách không có yêu cầu chờ và không có người mượn | Xóa thành công | [x] |
| TC08 | Xóa tài khoản không có yêu cầu chờ, phiếu mượn hoặc ca trực ràng buộc | Chưa kiểm thử vì chưa có dữ liệu phù hợp | [ ] |
| TC09 | Nhiều yêu cầu trong hàng đợi, duyệt/từ chối liên tiếp | Chưa có kết quả; cần hướng dẫn/kịch bản kiểm thử | [ ] |
| TC10 | Hàng đợi rỗng | Không phát sinh lỗi | [x] |
| TC11 | Duyệt yêu cầu hợp lệ | Thành công | [x] |
| TC12 | Duyệt yêu cầu không thể xử lý | Thất bại; yêu cầu vẫn ở đầu hàng đợi. Thông báo dài bị tràn/cắt ở cạnh dưới giao diện | [x] hành vi giữ yêu cầu; [ ] thông báo |
| TC13 | Từ chối yêu cầu và nhập lý do | Từ chối thành công; khi nhập nội dung dài, chữ tràn và nội dung không còn nhìn thấy đầy đủ trong ô nhập | [x] thao tác; [ ] hiển thị nội dung dài |
| TC14 | Khởi động lại khi còn yêu cầu chờ | Yêu cầu vẫn được khôi phục theo kết quả người dùng kiểm tra | [x] |

### Rà soát quy tắc cập nhật sách/tài khoản

- Việc giữ nguyên ID khi sửa tên/thông tin không làm thay đổi khóa định danh mà yêu cầu đang chờ sử dụng.
- Cần kiểm tra source để xác nhận lúc duyệt yêu cầu, hệ thống truy xuất đúng sách/tài khoản theo ID hiện tại và không lưu bản sao thông tin cũ gây sai lệch.
- Chưa kết luận đây là lỗi hay cần thay đổi nghiệp vụ; giữ lại làm mục rà soát trước khi chỉnh sửa.

## 4. Vấn đề đã xác định và thứ tự ưu tiên

### P0 - Ưu tiên cao nhất cho phiên làm việc tiếp theo: phản hồi lỗi và hiển thị nội dung dài

Chưa sửa trong phiên 08/11. Phải xử lý mục này trước các nhiệm vụ khác ở phiên tiếp theo.

1. **Thông báo khi đổi ID sách/tài khoản hoặc xóa tài khoản bị chặn:** nêu rõ nguyên nhân nghiệp vụ thay vì chỉ hiển thị “Cập nhật thất bại” hoặc “Xóa tài khoản thất bại”.
2. **Thông báo khi duyệt yêu cầu thất bại:** yêu cầu vẫn được giữ ở đầu FIFO, nhưng thông báo dài bị cắt/tràn khỏi vùng hiển thị. Cần bố trí lại thông báo, tự xuống dòng hoặc dùng vùng hiển thị phù hợp.
3. **Ô nhập lý do từ chối:** nội dung dài không hiển thị đầy đủ trong ô. Cần thiết kế cách xem/chỉnh sửa nội dung dài mà không làm mất dữ liệu nhập.
4. **Thông báo xóa sách:** phân biệt trường hợp sách có người đang mượn với trường hợp sách có yêu cầu mượn đang chờ, nếu hai điều kiện được kiểm tra riêng.

Yêu cầu khi thực hiện:
- Đọc source mới nhất trước khi sửa; không dựa vào bản GitHub cũ nếu không khớp với bản local người dùng đã kiểm thử.
- Nêu chính xác file, hàm và vị trí; ưu tiên cung cấp đoạn/hàm hoàn chỉnh để thay thế.
- Không thay đổi nghiệp vụ FIFO hoặc tự động loại yêu cầu khỏi hàng đợi khi chưa thống nhất quy tắc.
- Sau khi sửa, cung cấp test case cho trường hợp bình thường, biên, lỗi, dữ liệu dài, UTF-8 và kiểm thử hồi quy.
- Chỉ đánh dấu hoàn thành sau khi người dùng chạy và xác nhận kết quả.

### P1 - Rà soát logic cập nhật dữ liệu liên quan đến yêu cầu chờ

- Kiểm tra `update_book_info()` và luồng sửa sách: sửa tên/tác giả/thể loại/số lượng/vị trí kệ nhưng giữ ID có an toàn với yêu cầu chờ hay không.
- Kiểm tra `update_user_info()` và luồng sửa tài khoản: sửa tên/mật khẩu/vai trò nhưng giữ ID có an toàn với yêu cầu chờ hay không.
- Xác nhận lúc duyệt yêu cầu, Controller lấy đúng dữ liệu sách và tài khoản theo ID hiện tại.
- Kiểm tra trường hợp đổi ID bị chặn và trường hợp ID mới đã tồn tại; thông báo phải phản ánh đúng nguyên nhân.

### P1 - Kiểm thử FIFO nhiều yêu cầu (TC09)

Kịch bản đề xuất, chưa thực hiện:
1. Tạo ba yêu cầu theo thứ tự REQ1, REQ2, REQ3.
2. Mở màn hình quản lý yêu cầu và xác nhận REQ1 đứng đầu, tổng số là 3.
3. Duyệt REQ1, xác nhận REQ2 đứng đầu và tổng số còn 2.
4. Từ chối REQ2, xác nhận REQ3 đứng đầu và tổng số còn 1.
5. Duyệt REQ3, xác nhận hàng đợi rỗng.
6. Khởi động lại ứng dụng và kiểm tra dữ liệu sau xử lý.
7. Đối chiếu lịch sử, phiếu mượn và thông báo để bảo đảm không mất, trùng hoặc đảo thứ tự yêu cầu.

### P2 - Các công việc còn lại
- [ ] Tra cứu người dùng và hiển thị sách người đó đang mượn cùng trạng thái `Dang muon`/`Qua han`.
- [ ] Hiển thị đúng nguyên nhân khi đăng ký mượn trùng.
- [ ] Build/chạy và hoàn thiện layout toàn cửa sổ ở nhiều kích thước.
- [ ] Kiểm thử ngày sai, ngày nhuận và rà soát các luồng khởi tạo `date`.
- [ ] Kiểm thử Undo User (ADD/UPDATE/DELETE) và Undo Borrow (ADD/DELETE), bao gồm đồng bộ dữ liệu sau Undo.
- [ ] Kiểm thử responsive Login ở nhiều kích thước cửa sổ.
- [ ] Kiểm thử toàn bộ luồng trên GUI với cả ba vai trò.
- [ ] Kiểm tra nhất quán giữa `request.txt`, `borrow.txt` và thông báo sau lỗi/thoát đột ngột.
- [ ] Rà soát quyền ở Controller/Service.
- [ ] Hash Password: không triển khai theo quyết định hiện tại; chỉ xem xét lại nếu có yêu cầu thay đổi.


## 6. Cập nhật source ngày 10/11/2026

Các mục dưới đây được đối chiếu trực tiếp với ZIP `PBL2_GUI_test(20261109-183056).zip`. Trạng thái `[~]` nghĩa là có mã nguồn nhưng chưa xác minh bằng build/kiểm thử runtime trong phiên này.

### Undo User
- [~] `Controller/library_controller.cpp`: `undo_user_action()` lấy thao tác gần nhất từ `user_undo_stack`, xử lý các loại `ADD`, `DELETE`, `UPDATE` bằng `user_service`.
- [~] `Controller/library_controller.cpp`: các luồng thêm/sửa/xóa tài khoản có push log vào `user_undo_stack`.
- [~] `GUI/library_gui.cpp`: `draw_users()` có nút `Hoàn tác` gọi `controller.undo_user_action()`.

### Undo Borrow
- [~] `Controller/library_controller.cpp`: `borrow_book()` ghi log `ADD`; `delete_borrow()` ghi log `DELETE`; `undo_borrow_action()` xử lý các loại thao tác này.
- [~] `Service/borrow_service.cpp`: `undo_create_borrow()` xóa phiếu đang mượn và cộng lại số lượng sách; `undo_delete_borrow()` khôi phục phiếu đã xóa.
- [~] `GUI/library_gui.cpp`: màn hình quản lý phiếu mượn có nút `Hoàn tác` gọi `controller.undo_borrow_action()`.

### Validation ngày tháng
- [~] `Model/date.cpp`: constructor có tham số ném `std::invalid_argument` khi `is_valid()` trả về false; không còn tự gán ngày sai về `01/01/2026`.
- [~] `GUI/library_gui.cpp`: `read_date()` bắt `std::invalid_argument` và trả về `false` khi ngày không hợp lệ.

### Thông báo lỗi tạo phiếu mượn
- [~] `Service/borrow_service.h/.cpp`: `create_borrow_ticket()` nhận `std::string& out_message`, gán lý do thất bại cho các trường hợp như mã phiếu trùng, không tìm thấy độc giả/sách, độc giả quá hạn, đạt giới hạn 7 cuốn, đã mượn sách và hết số lượng.
- [~] `Controller/library_controller.h/.cpp`: `borrow_book()` nhận và chuyển tiếp `out_message`.

### Responsive Login
- [~] `GUI/login.cpp`: `login_screen::draw_input_box()` dùng `GetWorldToScreen2D()` để đổi tọa độ vùng cắt theo Camera2D trước khi gọi `BeginScissorMode()`.

### Quyết định phạm vi
- [x] Không triển khai Hash Password trong phạm vi hiện tại theo quyết định được ghi trong báo cáo bàn giao.
- [x] Giữ nguyên giới hạn nhập ASCII tại Login; không mở rộng hỗ trợ gõ tiếng Việt có dấu trong phiên này.

### Việc chưa xác minh
- [ ] Build GUI từ source ZIP.
- [ ] Kiểm thử Undo thực tế cho từng loại thao tác và trường hợp không thể Undo.
- [ ] Kiểm thử ngày sai, ngày nhuận và các luồng khởi tạo `date` khác.
- [ ] Kiểm thử responsive Login khi thay đổi kích thước cửa sổ.
- [ ] Kiểm tra tính nhất quán của file dữ liệu khi Undo và khi ứng dụng bị đóng đột ngột.

## 7. Quy tắc cập nhật tiến độ và kiểm thử

- Chỉ đánh dấu `[x]` khi có mã nguồn và đã được kiểm tra theo luồng phù hợp.
- Ghi rõ kết quả do người dùng báo cáo hay do kiểm thử trực tiếp trong môi trường làm việc.
- Nếu mới thấy hàm/màn hình trong source nhưng chưa chạy thử, ghi “đã có mã nguồn, chưa xác minh”.
- Mỗi lỗi cần ghi: bước tái hiện, kết quả mong đợi, kết quả thực tế, file/hàm liên quan.
- Mọi thay đổi code phải kèm test case cho trường hợp bình thường, biên, lỗi và hồi quy; nêu kết quả mong đợi để người dùng kiểm tra.
- Không tự ý reset, checkout hoặc ghi đè các thay đổi chưa commit.
- Trước khi sửa, phải đọc đúng phiên bản source mới nhất trong ZIP/repository hiện tại.
