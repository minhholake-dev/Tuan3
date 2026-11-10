# Tuần 03 — Kết nối không dây và IoT

Các bài thực hành Wi-Fi, BLE, ESP-NOW, Web Server và MQTT trên ESP32-C3.

## Danh mục bài tập

| Environment | Nội dung | Mã nguồn |
| --- | --- | --- |
| `main` / `wifi-ap` | Điểm truy cập Wi-Fi có mật khẩu | [Access Point](bai-tap/bai-01-wifi/access-point/main.cpp) |
| `wifi-station` | Kết nối Wi-Fi, tự thử lại khi mất mạng | [Station](bai-tap/bai-01-wifi/station/main.cpp) |
| `ble-server` | BLE Server với characteristic đọc/ghi | [BLE Server](bai-tap/bai-02-ble-server/main.cpp) |
| `espnow-transmitter` | Gửi bộ đếm qua ESP-NOW mỗi 2 giây | [Transmitter](bai-tap/bai-03-esp-now/transmitter/main.cpp) |
| `espnow-receiver` | Kiểm tra kích thước gói và nhận bộ đếm | [Receiver](bai-tap/bai-03-esp-now/receiver/main.cpp) |
| `web-server` | Điểm truy cập và trang web điều khiển LED | [Web Server](bai-tap/bai-04-web-server/main.cpp) |
| `mqtt` | Gửi bộ đếm và nhận lệnh LED qua MQTT | [MQTT](bai-tap/bai-05-mqtt/main.cpp) |

Sơ đồ gốc: [Bài 04 — Web Server](hardware/bai-04-web-server.fzz). Sơ đồ có thể còn đánh dấu GPIO22; với board ESP32-C3 hiện tại, nối LED rời qua điện trở hạn dòng tới **GPIO4** thay cho GPIO22. Chân LED dùng chung được khai báo ở [board_config.h](include/board_config.h).

## Cấu hình cục bộ và Git ignore

Thông tin SSID, mật khẩu Wi-Fi/AP, địa chỉ và cổng broker, tài khoản MQTT, client ID prefix, topic và MAC đích ESP-NOW nằm trong `include/secrets.h`. File này đã được thêm vào [.gitignore](.gitignore); không đưa vào repository hoặc bản ZIP chia sẻ.

Các giá trị có sẵn đã được chuyển sang file cục bộ trên máy. Khi clone hoặc giải nén một bản sạch, tạo lại cấu hình từ [secrets.example.h](include/secrets.example.h):

```powershell
Copy-Item -LiteralPath include/secrets.example.h -Destination include/secrets.h
```

Sau đó điền thông tin trong `include/secrets.h`. Chỉ chạy lệnh sao chép khi file cục bộ chưa tồn tại, để tránh ghi đè cấu hình đang dùng.

- `AP_*`: bài Wi-Fi Access Point mặc định.
- `STATION_*`: bài Wi-Fi Station.
- `WEB_AP_*`: Access Point của bài Web Server.
- `MQTT_WIFI_*`, `MQTT_*`: mạng và broker của bài MQTT. Có thể để username/password trống nếu broker không yêu cầu xác thực.
- `ESPNOW_CHANNEL`, `ESPNOW_PEER_MAC`: kênh chung và MAC đích cho ESP-NOW. MAC toàn `FF` là broadcast trong bài minh họa; thay bằng MAC receiver nếu gửi unicast.

Access Point chỉ được bật khi SSID hợp lệ và mật khẩu dài 8–63 ký tự; cấu hình thiếu sẽ không tạo mạng mở. Chương trình không in mật khẩu hoặc cấu hình broker ra Serial. Những mục Wi-Fi còn là giá trị mẫu phải được điền trước khi kết nối.

`.gitignore` ngăn file cục bộ mới được thêm vào Git; nó không xóa thông tin đã từng được commit trong lịch sử kho. Nếu thông tin thật từng được chia sẻ, cần thay mật khẩu tương ứng.

## Build và nạp từng bài

Môi trường được khai báo ở [platformio.ini](platformio.ini), sử dụng `espressif32@8.0.0`, Arduino ESP32 3.3.12, board `adafruit_qtpy_esp32c3` và thư viện PubSubClient. Chọn environment trong **PlatformIO: Project Tasks**, không cần chép đè các sketch.

Ví dụ trong terminal PlatformIO tại thư mục tuần:

```powershell
pio run -e web-server
pio run -e web-server -t upload
pio device monitor -b 115200
```

Thay `web-server` bằng environment trong bảng. `main` nạp bài Access Point qua [src/main.cpp](src/main.cpp). Mỗi environment chỉ biên dịch một sketch có `setup()` và `loop()`.

## Cách thử trên thiết bị

- **Access Point:** kết nối vào Wi-Fi đã đặt trong file cục bộ; IP của AP được in ra Serial.
- **Station:** khi mất mạng, chương trình thử kết nối lại theo chu kỳ thay vì chờ vô hạn trong `setup()`.
- **BLE:** tìm thiết bị `ESP32-C3 BLE Demo`, mở service/characteristic UUID trong source để đọc hoặc ghi. Sau khi ngắt kết nối, thiết bị tự quảng bá lại.
- **ESP-NOW:** nạp transmitter và receiver lên hai board; dùng cùng kênh, đúng MAC nếu unicast. Callback nhận chỉ sao chép gói đúng kích thước; Serial được xử lý trong vòng lặp chính. Thông báo “queued” chỉ xác nhận gói được xếp hàng, không chứng minh bên nhận đã nhận được.
- **Web Server:** nối LED GPIO4 qua điện trở, kết nối AP rồi mở IP được in ra Serial. Các nút gửi lệnh POST để bật/tắt LED.
- **MQTT:** cấu hình broker/topic, nối LED GPIO4; nhận payload một ký tự `0` hoặc `1` ở LED topic. Gõ `0`/`1` trong Serial để gửi lệnh, bộ đếm được publish mỗi 2 giây. Hai topic phải khác nhau, dài tối đa 160 byte và không chứa wildcard. Mỗi board có client ID riêng theo hậu tố địa chỉ phần cứng 48 bit. Khi mất broker, các lần thử lại cách nhau 5 giây, không có vòng chờ vô hạn; TCP connect giới hạn 1,5 giây và thời gian chờ MQTT là 3 giây.

Các bài MQTT hiện dùng TCP thường (`WiFiClient`), không tự mã hóa dữ liệu bằng TLS. ESP-NOW broadcast và BLE minh họa cũng chưa xác thực/mã hóa ứng dụng; chỉ dùng dữ liệu thử trong mạng thực hành phù hợp. Việc ẩn cấu hình khỏi Git không thay thế bảo mật đường truyền.

Các lỗi thiếu dấu ngoặc/biến, tên header, kiểm tra gói ESP-NOW, chân LED và xử lý kết nối lại đã được sửa. Kết quả thực tế vẫn cần kiểm tra trên hai board, mạng Wi-Fi và broker của bạn.

API đối chiếu theo [BLEServer.h của Arduino ESP32 3.3.12](https://github.com/espressif/arduino-esp32/blob/3.3.12/libraries/BLE/src/BLEServer.h) và [ESP-NOW của ESP-IDF 5.5.5](https://docs.espressif.com/projects/esp-idf/en/v5.5.5/esp32c3/api-reference/network/esp_now.html).
