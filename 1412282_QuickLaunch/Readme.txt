1.Thông tin cá nhân:
MSSV: 1412282
Họ tên: Nguyễn Hoàng Lân
Email: hoanglanars96@gmail.com

2.Chức năng đã làm được:
- Bấm phím mở nhanh Quick Launch: Windows + Space
- Bấm Enter để mở ứng dụng đầu tiên tìm thấy
- Bấm tổ hợp phím Ctrl + [number] để mở ứng dụng theo thứ tự button hiển thị) 
- Notify Icon gồm các menu: 
  + Scan folder: scan folder chứa các ứng dụng để lưu lên Quick Launch
  + Statistic: thống kê số lần chạy ứng dụng bằng Quick Launch
  + Exit: tắt ứng dụng Quick Launch
3. Luồng sự kiện chính:
 a. Scan folder:
   - Khi chạy ứng dụng lần đầu (chưa có dữ liệu), ứng dụng sẽ tự scan lần lượt các folder sau:
	+ System32
	+ ProgramFile (App 64bits)
	+ ProgramFile(x86) (App 32bits)
   - Khi muốn scan thư mục thì chọn Scan folder ở Notify để scan folder:
   	+ Ứng dụng hiện hộp thoại Browser folder
   	+ Chọn 1 folder cần scan -> bấm Select
   	+ Ứng dụng sẽ quét toàn bộ thư mục và tập tin trong folder để tìm tất cả file *.exe
   	+ Nếu chọn folder là folder cha của các folder đã từng chọn, hoặc chọn folder đã từng chọn, 
ứng dụng sẽ tự động cập nhật các lần chạy trong lịch sử và các ứng dụng mới.
   - Khi đang scan có thể bấm Stop để Hủy Scan (scan ở thread khác).
 b. Tìm ứng dụng (double click vào notify Icon hoặc Windows + Space)
   - Bấm Enter để chạy ứng dụng đầu tiên
   - Ứng dụng sẽ hiện top 10 ứng dụng thường xuyên sử dụng nhất
   - Nhập tên ứng dụng cần tìm
   - Ứng dụng sẽ hiện top 10 ứng dụng với subtring tương ứng có lượt sử dụng cao nhất
   - Click vào Button để chạy hoặc Bấm Ctrl + [number] theo thứ tự để chạy ứng dụng
   - Cửa sổ Quick launch tự động đóng lại chờ lệnh tiếp theo
 c. Statistic:
   - Ứng dụng sẽ hiện thống kế theo lượt chạy, sắp xếp theo thứ tự từ nhiều đến ít
   - Bấm next để sang top kế tiếp
   - Bấm previous để trở lại top trước		
4. Luồng sự kiện phụ:
 a. Scan folder:
   - Chọn ổ đĩa, ứng dụng không nhận được icon
 b. Tìm ứng dụng (bấm double click vào notify Icon hoặc Windows + Space)
   - Chưa hoàn thành phần xem tất cả ứng dụng tìm được
   - Không sử dụng hook keyboard low level nên nếu có thể bị trùng với phím tắt của Windows sẽ có thêm ứng dụng Windows chạy (Windows + Space là phím thay đổi ngôn ngữ bàn phím với windows 10)
   - Nếu ứng dụng bị xóa hoặc đổi tên, cần phải Scan lại chứ ứng dụng không tự động Scan liên tục.
*Link youtube: https://youtu.be/UAOV70na13g
*Link github:
	