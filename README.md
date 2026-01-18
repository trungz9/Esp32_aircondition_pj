# ESP32 WiFi Fan & LED Control 🌐🌀

Dự án này sử dụng **ESP32** để điều khiển **quạt DC (qua MOSFET)** và **LED** thông qua **trình duyệt web** trong cùng mạng WiFi.  
Người dùng có thể điều chỉnh **công suất (PWM)** của quạt và LED bằng **thanh trượt (slider)** trên giao diện web.

---

## 📌 Chức năng chính

- Kết nối ESP32 vào mạng WiFi
- Tạo **Web Server (port 80)**
- Hiển thị trang web điều khiển
- Điều chỉnh tốc độ quạt & độ sáng LED bằng PWM
- Điều khiển **real-time** qua trình duyệt (PC / điện thoại)

---

## 🧩 Phần cứng sử dụng

| Thành phần | Mô tả |
|----------|------|
| ESP32 | Vi điều khiển chính |
| Quạt DC 12V | Điều khiển qua MOSFET |
| MOSFET N-channel | Điều khiển công suất quạt |
| LED | LED báo trạng thái |
| Nguồn 12V | Cấp cho quạt |
| Nguồn 5V / 3.3V | Cấp cho ESP32 |

---

## 🔌 Sơ đồ chân (Pin Mapping)

| Chức năng | GPIO |
|---------|------|
| Quạt (MOSFET Gate) | GPIO 4 |
| LED | GPIO 2 |

---

## ⚙️ Cấu hình PWM

- **Tần số PWM**: `25 kHz`  
  → phù hợp cho quạt, giảm tiếng ồn
- **Độ phân giải**: `8 bit`
- **Giá trị duty**: `0 – 255`

```cpp
#define PWM_FREQ 25000
#define PWM_RESOLUTION 8
```

**ESP32 sử dụng pin-based PWM (core mới):** 
- **Sao chép mã** 
```cpp 
ledcAttach(FAN_PIN, PWM_FREQ, PWM_RESOLUTION); 
ledcAttach(LED_PIN, PWM_FREQ, PWM_RESOLUTION); 
``` 
## 🌐 **Giao diện Web**  
Trang web được nhúng trực tiếp trong code (PROGMEM)

Sử dụng HTML + JavaScript

Thanh trượt điều chỉnh từ 0–100%

Gửi dữ liệu bằng fetch() đến ESP32

URL điều khiển:

```text  
http://<IP_ESP32>/  
```
## 🔁 **Nguyên lý hoạt động**
ESP32 kết nối WiFi

Khởi động Web Server

Người dùng mở trình duyệt và truy cập IP ESP32

Thay đổi slider → gửi request:

```arduino
Sao chép mã
/set?speed=VALUE
```
ESP32:Chuyển % → duty PWM  

Điều khiển quạt & LED  

Quạt và LED thay đổi công suất tương ứng

## 📄 Thư viện sử dụng 
```cpp 
Sao chép mã
#include <WiFi.h>
#include <WebServer.h>
``` 
## 🚀 **Cách sử dụng**  
Sửa thông tin WiFi:

```cpp  
Sao chép mã
const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASSWORD";
```  
Nạp code cho ESP32

Mở Serial Monitor (115200 baud)

Lấy địa chỉ IP ESP32

Truy cập IP bằng trình duyệt

Điều chỉnh quạt bằng slider

## **🧠 Ghi chú kỹ thuật**
GPIO 2 là LED tích hợp trên nhiều board ESP32

MOSFET nên có:

Gate resistor (100–220Ω)

Diode bảo vệ (nếu quạt cảm ứng)

Có thể mở rộng:

Cảm biến MQ (khói)

Điều khiển tự động theo ngưỡng

Giao diện đẹp hơn (CSS)

## **📌 Ứng dụng**
Máy hút khói hàn mini

Hệ thống lọc không khí nhỏ

Điều khiển quạt thông minh

Dự án IoT học tập ESP32

### ✅ Lưu ý quan trọng
- Đây là **Markdown chuẩn GitHub**
- Không lỗi render
- Không lẫn text thừa
- Dùng tốt cho:
  - Repo cá nhân
  - Báo cáo môn học
  - Đồ án IoT / ESP32

Nếu bạn muốn, mình có thể:
- Viết thêm **LICENSE**
- Viết **README tiếng Anh**
- Tách README thành **User Guide + Developer Guide**
- Chuẩn hóa theo **format đồ án đại học**
