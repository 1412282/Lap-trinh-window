- Thông tin cá nhân:
  Họ tên: Nguyễn Hoàng Lân
  MSSV: 1412282
  Email: hoanglanars96@gmail.com
- Các chức năng làm được:
1.Tạo ra TreeView bên trái, ListView bên phải. 
2.Xét TreeView
- Tạo node root là This PC
- Lấy danh sách các ổ đĩa trong máy bằng hàm GetLogicalDrives hoặc GetLogicalDriveStrings, thêm các ổ đĩa vào node root, tạo sẵn thuộc tính cChildren = true để báo hiệu có các node con. Gán giá trị của ổ đĩa ví dụ C:\ vào PARAM (tag) để lấy lên xài lại.
- Bắt sự kiện Expanding, lấy ra đường dẫn dấu ở PARAM để biết mình phải xư lí thư mục nào, duyệt nội dung thư mục bằng FindFirstFile & FindNextFile, chỉ lấy các thư mục để thêm vào làm node con.
3.Xét ListView
- Hiển thị toàn bộ thư mục và tập tin tương ứng với một đường dẫn
- Bấm đôi vào một thư mục sẽ thấy toàn bộ thư mục con và tập tin.
* Các luồng sự kiện chính:
1. Chạy chương trình lên, hiển thị node This PC trên TreeView bên trái ở trạng thái collapse (thu gọn). Bấm vào sẽ xổ xuống các node con là danh sách ổ đĩa.
2. Bấm vào ổ đĩa C đang ở trạng thái collapse(thu gọn) trong TreeView bên trái sẽ xổ xuống danh sách các thư mục con.
3. Double click vào từng thư mục trong Listview sẽ hiện ra các thư mục con của thư mục đó.