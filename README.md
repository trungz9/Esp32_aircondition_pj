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
