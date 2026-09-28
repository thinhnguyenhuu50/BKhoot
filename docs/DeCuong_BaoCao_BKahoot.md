# ĐỀ CƯƠNG BÁO CÁO DỰ ÁN BKAHOOT

### Hệ thống trò chơi hỏi–đáp nhiều người chơi, không dây, chạy trên KIT STM32F407 + ESP8266

---

## Cách sử dụng đề cương

Đề cương này **không phải báo cáo**. Nó chỉ ra cho người viết: mỗi mục cần trình bày ý gì, cần so sánh những phương án nào và cần vẽ hình gì. Mỗi mục có thể gồm các phần sau:

- **Nội dung cần viết**: các ý chính, số liệu cần có và câu hỏi mà đoạn văn phải trả lời.
- **So sánh – biện luận**: các phương án thay thế kèm tiêu chí so sánh. Báo cáo phải kết luận **vì sao chọn phương án hiện tại** và nêu rõ nhược điểm nhóm chấp nhận.
- **Hình vẽ**: loại sơ đồ, công cụ, các thành phần cần có và cách nối chúng.
- **Tham chiếu mã nguồn**: các file cần đọc khi viết mục đó.

**Quy ước chung**

| Hạng mục | Quy ước |
|---|---|
| Tên dự án | **BKahoot** (tên trong mã nguồn và trên giao diện: *BKhoot*) |
| Công cụ vẽ | **draw.io** cho sơ đồ khối, sơ đồ bộ nhớ và định dạng khung. **PlantUML** (hoặc Mermaid) cho state machine, sequence, activity, component và Gantt. **WaveDrom** cho giản đồ thời gian. Lưu file nguồn `.drawio`/`.puml` trong `docs/diagrams/` để sửa lại được. |
| Màu thống nhất | STM32 màu **xanh dương**, ESP8266 màu **cam**, đường vô tuyến ESP-NOW **nét đứt**, ISR là khối **đỏ** có biểu tượng tia sét, queue vẽ hình ống nằm ngang, mutex/semaphore vẽ biểu tượng ổ khóa, file sinh tự động tô **xám**. |
| Bảng so sánh | Các cột tối thiểu: *Phương án · Ưu điểm · Nhược điểm · Mức phù hợp với BKahoot*. Ngay sau mỗi bảng có một đoạn **"Kết luận lựa chọn"**. |
| Số liệu | Số liệu **đo thực tế** ghi rõ điều kiện đo. Số liệu **trích datasheet** ghi nguồn. Không đưa số liệu ước lượng mà không ghi chú. |
| Đánh số | Hình *x.y*, Bảng *x.y* theo chương. Mã nguồn trích dẫn không quá 20 dòng mỗi đoạn. |

---

## Thông tin kỹ thuật tóm tắt (dùng xuyên suốt báo cáo)

| Hạng mục | Giá trị trong dự án | Nguồn |
|---|---|---|
| MCU chính | STM32F407ZGT6 (Cortex-M4F, LQFP144), 168 MHz từ HSI 16 MHz qua PLL (M=8, N=168, P=2) | [main.c](../Core/Src/main.c) `SystemClock_Config` |
| Bộ nhớ | Flash 1 MB; SRAM 128 KB và CCM 64 KB; SRAM ngoài 512 KB tại `0x68000000` (FSMC NE3) | [STM32F407XX_FLASH.ld](../STM32F407XX_FLASH.ld) |
| Hiển thị | LCD ILI9341 240×320, RGB565, FSMC 16-bit NE1 | [Drivers/LCD](../Drivers/LCD) |
| Cảm ứng | Điện trở, chip XPT2046 đọc qua SPI mềm (GPIO); hệ số hiệu chuẩn lưu trong EEPROM AT24Cxx (I2C1) | [Drivers/Touch](../Drivers/Touch), [Drivers/EEPROM](../Drivers/EEPROM) |
| RTOS | FreeRTOS qua CMSIS-RTOS v2, tick 1 kHz, heap_5 | [FreeRTOSConfig.h](../Core/Inc/FreeRTOSConfig.h) |
| GUI | LVGL 9.6, chế độ render PARTIAL, 2 draw buffer × 38 400 B | [lv_conf.h](../Middlewares/Third_Party/lv_conf.h), [gui_app.c](../App/gui_app.c) |
| Kết nối | ESP8266 chạy firmware tự viết trên ESP8266_RTOS_SDK, 160 MHz, ESP-NOW kênh 1, không mã hóa | [esp8266/main](../esp8266/main) |
| Liên kết STM32–ESP | USART2 ↔ UART0 của ESP, 115200 8N1; khung `[0xAA][CMD][LEN][PAYLOAD][XOR]` | [protocol.c](../App/Logic/protocol.c) |
| Debug | USART1 115200 qua cổng RS232, DMA TX/RX, console lệnh `h` và `1` | [rs232.c](../Drivers/Rs232/rs232.c) |
| Build | CMake + Ninja + arm-none-eabi-gcc; CubeMX sinh code khởi tạo; script Python sinh ngân hàng câu hỏi | [CMakeLists.txt](../CMakeLists.txt) |
| Quy mô | Tối đa 20 người chơi mỗi phòng, 4 đáp án mỗi câu, payload tối đa 200 B | [game_logic.h](../App/Logic/game_logic.h), [protocol.h](../App/Logic/protocol.h) |

---

## PHẦN MỞ ĐẦU

**Nội dung cần viết**
- Trang bìa, lời cảm ơn, mục lục, danh mục hình và danh mục bảng.
- **Tóm tắt** (khoảng 200 từ): bài toán (Kahoot ngoại tuyến trong lớp học), giải pháp (nhiều KIT STM32 kết nối bằng ESP-NOW), công nghệ chính và 2–3 kết quả đo nổi bật nhất lấy từ Chương 10.
- **Danh mục từ viết tắt**: FSMC, DMA, ISR, RTOS, FSM, ESP-NOW, MAC, ACK, LVGL, GRAM, RGB565, SOF, IPC, TCB, CCM, NVS, BSP, HAL…

---

## CHƯƠNG 1. TỔNG QUAN DỰ ÁN

### 1.1 Bối cảnh và lý do chọn đề tài

**Nội dung cần viết**
- **Bối cảnh**: Kahoot rất phổ biến trong lớp học, nhưng cần Internet và mỗi người phải có điện thoại hoặc máy tính. Ý tưởng của nhóm là làm một hệ thống quiz **ngoại tuyến**, chạy trên các KIT nhúng giống hệt nhau, không cần router hay Internet.
- **Lý do học thuật**: viết theo dạng *"công nghệ → kỹ năng thu được"*, mỗi ý 3–5 câu:
  1. **Tổ chức dự án STM32 bằng CMake** thay cho IDE đóng: CubeMX sinh code khởi tạo, `CMakePresets.json` cho Debug/Release, tích hợp thư viện ngoài (`add_subdirectory(lvgl)`), thêm bước sinh mã tự động (`add_custom_command`) và tùy biến linker (`-Wl,--wrap`).
  2. **FreeRTOS (CMSIS-RTOS v2)**: đa nhiệm có ưu tiên; queue, mutex và semaphore; chuyển dữ liệu từ ISR sang task; giám sát stack và heap lúc chạy.
  3. **Thư viện đồ họa LVGL**: xây GUI cảm ứng trên MCU ít tài nguyên; viết lớp porting cho display và input; dùng DMA để đẩy pixel.
  4. **Giao thức ESP-NOW**: truyền không dây không cần kết nối, phân biệt broadcast và unicast, ACK ở tầng MAC; tự thiết kế giao thức tầng ứng dụng.
  5. **Hệ thống nhiều vi điều khiển**: STM32 chạy ứng dụng, ESP8266 làm bộ đồng xử lý vô tuyến; hai chip nói chuyện qua giao thức UART có đóng khung.
  6. **Mở rộng bộ nhớ bằng SRAM ngoài qua FSMC**: sửa linker script, dùng heap nhiều vùng (heap_5), tự kiểm tra phần cứng khi khởi động.
  7. **Debug firmware qua RS232**: ghi log không chặn bằng DMA, console lệnh lúc chạy, chuyển tiếp log của ESP8266 qua STM32.
  8. **Tự động hóa**: sinh mã C từ dữ liệu JSON ngay trong quá trình build.
- **Tính thực tiễn**: dùng được trong lớp học; tất cả KIT nạp **cùng một firmware** và người dùng chọn vai trò Host hoặc Player lúc chạy.

**Hình vẽ**
- *Hình 1.1 – Sơ đồ tư duy "Công nghệ ↔ Kỹ năng"* (draw.io hoặc XMind). Nút trung tâm là "BKahoot". 8 nhánh ứng với 8 ý ở trên. Mỗi nhánh có 2–3 lá là kỹ năng cụ thể, ví dụ nhánh FreeRTOS có "queue ISR→task", "mutex bảo vệ LVGL" và "đo stack high-water mark".

### 1.2 Mục tiêu đề tài

**Nội dung cần viết**: trình bày thành bảng *Mã · Mục tiêu · Tiêu chí đánh giá · Nơi kiểm chứng (mục ở Chương 10)*.
- **Mục tiêu chức năng**: 1 Host và tối đa 20 Player; tìm và vào phòng trong vài giây; câu hỏi 4 lựa chọn có đếm giờ; chấm điểm và báo đúng/sai ngay lập tức; bảng xếp hạng sau mỗi câu và khi kết thúc; phát hiện người chơi mất kết nối.
- **Mục tiêu kỹ thuật**: một firmware dùng cho mọi KIT; giao diện mượt (đặt chỉ tiêu FPS và thời gian chuyển màn hình); không cấp phát động trong ISR; kiểm soát được bộ nhớ (heap tự do nhỏ nhất, stack watermark).
- **Mục tiêu học tập**: ánh xạ sang các ý ở mục 1.1.

### 1.3 Yêu cầu hệ thống

**Nội dung cần viết**
- **Tác nhân**: Host (người điều khiển phòng), Player (người chơi), Nhà phát triển (cập nhật câu hỏi, debug).
- **Yêu cầu chức năng (FR)**, trình bày thành bảng:
  FR-01 chọn vai trò · FR-02 Host mở phòng và thấy số người chơi · FR-03 Player quét và liệt kê các phòng · FR-04 Player vào phòng và nhận mã `P<n>` · FR-05 Host bắt đầu và phát câu hỏi · FR-06 mỗi Player chỉ trả lời một lần cho mỗi câu · FR-07 chấm điểm và gửi phản hồi · FR-08 kết thúc câu khi hết giờ hoặc khi mọi người đã trả lời · FR-09 bảng xếp hạng từng câu và kết quả cuối · FR-10 xử lý người chơi rớt mạng hoặc vào lại · FR-11 nút về màn hình chính · FR-12 console debug · FR-13 cập nhật câu hỏi bằng file JSON.
- **Yêu cầu phi chức năng (NFR)**: độ trễ từ lúc phát câu hỏi đến lúc hiển thị (đặt chỉ tiêu, ví dụ < 200 ms, rồi kiểm chứng ở Chương 10); độ tin cậy (phát hiện rớt kết nối sau 3 lần gửi thất bại); khả năng mở rộng (20 người); thời gian phản hồi cảm ứng; ngân sách bộ nhớ; khả năng bảo trì (tách module screens/logic/protocol); khả năng debug.
- **Ràng buộc**: phần cứng KIT cố định; UART0 của ESP8266 đã dành cho liên kết với STM32; font có sẵn; payload tối đa 200 B.

**Hình vẽ**
- *Hình 1.2 – Use case diagram* (PlantUML). Hệ thống "BKahoot" là một hình chữ nhật, các use case là hình elip.
  - Actor Host: Mở phòng, Bắt đầu quiz, Chuyển câu, Xem xếp hạng.
  - Actor Player: Quét phòng, Tham gia, Trả lời, Xem phản hồi.
  - Actor Nhà phát triển: Cập nhật ngân hàng câu hỏi, Xem log và thống kê RTOS.
  - Quan hệ: "Trả lời" `<<include>>` "Nhận phản hồi"; "Phát hiện người chơi rớt" `<<extend>>` "Bắt đầu quiz".

### 1.4 Giới hạn đề tài

**Nội dung cần viết**: liệt kê từng giới hạn và lý do (vì sao chấp nhận trong phạm vi đồ án):
- Mỗi phòng có 1 Host, tối đa 20 Player (`MAX_PLAYERS`, bằng với `MAX_TRACKED_PEERS` phía ESP8266).
- Ngân hàng câu hỏi cố định lúc build; muốn đổi câu hỏi phải build và nạp lại.
- Chỉ hiển thị ký tự ASCII: font Montserrat 14 có sẵn không có dấu tiếng Việt. Câu hỏi cộng 4 đáp án (kể cả ký tự `\0`) không được vượt quá khoảng 196 B khi truyền.
- Tính điểm đơn giản: +1 cho mỗi câu đúng, không cộng điểm theo tốc độ như Kahoot.
- Không mã hóa ESP-NOW, kênh cố định là kênh 1, không xác thực thiết bị và không chống gian lận nâng cao.
- Tầm phủ sóng chỉ trong phạm vi một phòng học.
- Kết quả không được lưu lại sau khi tắt nguồn. Host không tham gia trả lời.

### 1.5 Bố cục báo cáo
Một đoạn văn ngắn tóm tắt nội dung từng chương.

---

## CHƯƠNG 2. PHẦN CỨNG ĐƯỢC CẤU HÌNH VÀ SỬ DỤNG TRÊN KIT

### 2.1 Sơ đồ khối toàn hệ thống

**Nội dung cần viết**
- **Mô hình triển khai**: N KIT giống hệt nhau. Một KIT chọn vai trò Host, các KIT còn lại là Player. Các KIT liên lạc với nhau qua ESP-NOW.
- **Bên trong một KIT**: STM32F407 ở trung tâm, cùng các khối hiển thị và cảm ứng, SRAM ngoài, module ESP8266, cổng debug RS232 nối PC, LED.
- Mô tả **đường đi của dữ liệu** qua các khối, ví dụ: chạm màn hình → STM32 → USART2 → ESP8266 → sóng vô tuyến → ESP8266 của Host → STM32 của Host → LCD của Host.

**Hình vẽ**
- *Hình 2.1 – Sơ đồ triển khai nhiều KIT* (draw.io). KIT Host đặt ở giữa, 3–4 KIT Player xung quanh (ghi chú "… tối đa 20"). Mũi tên nét đứt hai chiều ghi "ESP-NOW 2.4 GHz, kênh 1". Phân biệt **broadcast** (mũi tên tỏa ra từ một Player khi quét phòng) với **unicast** (mũi tên nối một–một). Vẽ thêm một PC nối RS232 vào một KIT để debug.
- *Hình 2.2 – Sơ đồ khối phần cứng của một KIT* (draw.io). Khối STM32F407ZGT6 lớn ở giữa, bên trong ghi các ngoại vi được dùng (FSMC, DMA2, USART1, USART2, I2C1, GPIO, TIM2). Các khối bên ngoài nối vào bằng mũi tên có ghi tên giao tiếp:
  - LCD ILI9341 ← FSMC NE1 (16-bit, chân RS nối A19); đèn nền `FSMC_BLK` (PA8) và reset `FSMC_RES` (PC13) nối GPIO.
  - SRAM ngoài ↔ FSMC NE3 (16-bit, địa chỉ `0x68000000`).
  - XPT2046 ↔ SPI mềm: T_CS PG7, T_CLK PG8, T_MOSI PC9, T_MISO PC12, T_PEN PC8.
  - EEPROM AT24Cxx ↔ I2C1 (lưu hệ số hiệu chuẩn cảm ứng).
  - ESP8266 ↔ USART2 (PA2/PA3), cùng ESP_POWER (PF10) và ESP_BUSY (PF9).
  - Bộ chuyển mức RS232 ↔ USART1 ↔ PC.
  - LED_DEBUG (PE4).
  - Vẽ **bus FSMC** thành một thanh dọc chung cho LCD và SRAM để người đọc thấy hai thiết bị này **dùng chung bus** (chi tiết này sẽ được nhắc lại ở mục 9.3).

**Tham chiếu mã nguồn**: [BKhoot.ioc](../BKhoot.ioc), [main.h](../Core/Inc/main.h), [fsmc.c](../Core/Src/fsmc.c), [gpio.c](../Core/Src/gpio.c), [usart.c](../Core/Src/usart.c).

### 2.2 Phân tích vi điều khiển

#### 2.2.1 STM32F407ZGT6: vi điều khiển trung tâm

**Nội dung cần viết**
- **Thông số chính** (trích datasheet): Cortex-M4F có FPU, 168 MHz, Flash 1 MB, SRAM 192 KB (128 KB SRAM chính và 64 KB CCM, CCM **chỉ CPU truy cập được**), FSMC, 2 bộ DMA × 8 stream, gói LQFP144 đủ chân cho FSMC.
- **Cấu hình clock thực tế**: HSI 16 MHz → PLL (M=8, N=168, P=2) → SYSCLK 168 MHz; AHB 168 MHz; APB1 = APB2 = 42 MHz (chia 4); Flash latency 5 wait state.
  - Nhận xét: dự án dùng **HSI** (sai số khoảng 1%) thay vì thạch anh HSE. Mức sai số này vẫn đủ cho UART 115200, nhưng HSE ổn định hơn. Đưa vào mục hướng cải tiến.
- FreeRTOS dùng SysTick làm tick, nên timebase của HAL được chuyển sang **TIM2**.
- **F407 không có DMA2D (Chrom-ART) và LTDC** (khác với F429). Vì vậy LVGL phải vẽ hoàn toàn bằng CPU, còn DMA chỉ dùng để *chuyển* pixel ra LCD. Đây là tiền đề để biện luận ở Chương 7.
- Ghi chú: TIM1 đã được cấu hình (prescaler 83, tức 1 MHz) nhưng hiện chưa dùng trong ứng dụng. Có thể nêu là để dự phòng cho hàm delay tính bằng µs.

**So sánh – biện luận** (Bảng 2.1 – Lựa chọn MCU chính)

| Phương án | Ưu điểm | Nhược điểm | Mức phù hợp |
|---|---|---|---|
| STM32F103 | Rẻ, phổ biến | 72 MHz, RAM 64 KB, không có FPU; không đủ RAM cho LVGL và draw buffer | Thấp |
| **STM32F407 (chọn)** | Có sẵn trên KIT; có FSMC cho LCD và SRAM; 192 KB RAM; FPU | Không có DMA2D/LTDC | **Cao** |
| STM32F429 | Có LTDC, DMA2D, SDRAM nên GUI mạnh hơn | Không có trên KIT; dư thừa cho màn 240×320 | Trung bình |
| Chỉ dùng ESP32 | Có sẵn Wi-Fi, BLE, ESP-NOW; 520 KB RAM | Không đạt mục tiêu học STM32/FSMC; KIT không nối màn hình vào ESP | Thấp |
| Raspberry Pi | Mạnh, có Linux | Không phải bài toán nhúng thời gian thực | Thấp |

**Hình vẽ**
- *Hình 2.3 – Cây clock rút gọn* (draw.io, hoặc chụp tab Clock Configuration của CubeMX rồi khoanh vùng). Các khối nối từ trái sang phải: HSI 16 MHz → /M=8 → ×N=168 → /P=2 → SYSCLK 168 MHz → AHB /1 → HCLK 168 MHz (Core, DMA, FSMC). Từ HCLK rẽ hai nhánh: APB1 /4 = 42 MHz (USART2, I2C1; TIM2 chạy ở 84 MHz) và APB2 /4 = 42 MHz (USART1, TIM1).

#### 2.2.2 ESP8266: bộ đồng xử lý kết nối không dây

**Nội dung cần viết**
- **Thông số**: Tensilica L106 32-bit, được cấu hình chạy 160 MHz; Wi-Fi 802.11 b/g/n 2.4 GHz; Flash 2 MB (theo `sdkconfig`); hỗ trợ ESP-NOW; **không có BLE**.
- **Firmware tự viết** trên ESP8266_RTOS_SDK (nền FreeRTOS, API kiểu ESP-IDF) thay cho firmware AT mặc định. Với firmware này, ESP8266 trở thành **cầu nối gần như trong suốt** giữa UART và ESP-NOW, có thêm hai việc: *chèn/bóc địa chỉ MAC* và *theo dõi chất lượng liên kết*.
- **Vì sao dùng hai MCU**: STM32 lo GUI và logic trò chơi; ESP8266 lo ngăn xếp vô tuyến.
  - Lợi ích: ngăn xếp Wi-Fi không chiếm CPU của STM32; dùng lại module có sẵn trên KIT; giao tiếp giữa hai chip đơn giản.
  - Chi phí: thêm một firmware phải bảo trì, thêm một giao thức liên kết, thêm độ trễ của UART.
- **Chân điều khiển**: `ESP_POWER` (PF10) được kéo lên mức cao ngay khi khởi động (commit *"set ESP_POWER_Pin initially HIGH"*). `ESP_BUSY` (PF9) là input nhưng **chưa dùng**, có thể dành cho điều khiển luồng sau này.
- UART0 của ESP8266 đã dùng để nối STM32, nên console log của ESP bị tắt (`CONFIG_ESP_CONSOLE_UART_NONE`). Thay vào đó, log được đóng gói thành lệnh `CMD_ESP_LOG` và gửi về STM32 (xem mục 3.3).

**So sánh – biện luận** (Bảng 2.2 – Công nghệ kết nối không dây). Tiêu chí: có sẵn trên KIT, số node hỗ trợ, độ trễ, kích thước gói, cần hạ tầng hay không, độ phức tạp phần mềm.

| Phương án | Ưu điểm | Nhược điểm |
|---|---|---|
| **ESP8266 + ESP-NOW (chọn)** | Không cần kết nối hay router; broadcast để tìm phòng; unicast có ACK tầng MAC; payload đến 250 B, đủ cho một câu hỏi (≤ 202 B trên sóng); khoảng 20 peer | Không có TCP; số peer bị giới hạn; cần tự viết giao thức |
| ESP8266 AT + SoftAP + UDP/TCP | Điện thoại cũng tham gia được | Phải kết nối (association) trước; SoftAP của ESP8266 chỉ nhận ít station (mặc định 4, **cần tra tài liệu SDK để trích số chính xác**); độ trễ kết nối lớn |
| ESP32 (ESP-NOW/BLE) | Mạnh hơn, có BLE | Không có trên KIT |
| nRF24L01+ | Rẻ, độ trễ thấp | Gói tối đa 32 B nên phải chia nhỏ câu hỏi; 6 pipe; phải tự làm lớp mạng |
| Bluetooth HC-05 | Dễ dùng | Chỉ nối điểm–điểm, không hợp mô hình 1 Host nhiều Player |
| LoRa | Tầm rất xa | Tốc độ thấp, độ trễ cao, dư thừa tầm phủ |

**Hình vẽ**
- *Hình 2.4 – Phân chia trách nhiệm giữa hai MCU* (draw.io). Hai khối lớn đặt cạnh nhau (STM32 màu xanh, ESP8266 màu cam), nối bằng mũi tên hai chiều ghi "USART2 ↔ UART0, 115200 8N1, khung 0xAA…".
  - Trong khối STM32 liệt kê: GUI LVGL, FSM trò chơi, chấm điểm, ngân hàng câu hỏi, console debug.
  - Trong khối ESP8266 liệt kê: task UART RX/TX, task ESP-NOW, bảng peer có bộ đếm lỗi, chèn/bóc MAC, chuyển tiếp log.
  - Từ khối ESP8266 vẽ thêm một ăng-ten.

### 2.3 Giao tiếp ngoại vi

**Nội dung cần viết**: với từng ngoại vi, nêu mục đích, thông số cấu hình trong CubeMX, chân sử dụng, chế độ hoạt động (polling, ngắt hay DMA) và **lý do chọn chế độ đó**.

Bảng 2.3 – Ngoại vi được dùng (bảng chân đầy đủ đưa vào Phụ lục A):

| Ngoại vi | Thiết bị | Cấu hình chính | Chế độ |
|---|---|---|---|
| FSMC Bank1 NE1 | LCD ILI9341 | 16-bit, extended mode (timing đọc và ghi khác nhau), chân RS nối A19, nên `LCD_REG` ở `0x600FFFFE` và `LCD_RAM` ở `0x60100000` | Ghi trực tiếp và qua DMA |
| FSMC Bank1 NE3 | SRAM ngoài | 16-bit; AddressSetup 2, DataSetup 3, BusTurnAround 1 (đơn vị chu kỳ HCLK) | Ánh xạ vào không gian bộ nhớ, bắt đầu từ `0x68000000` |
| DMA2 Stream0 | Bộ nhớ → bộ nhớ | Nguồn tăng địa chỉ, đích cố định (`LCD_RAM`), đơn vị half-word; ngắt TC gọi `lv_display_flush_ready` | DMA |
| USART1 + DMA2 Stream7/Stream2 | RS232 nối PC | 115200 8N1; TX dùng DMA và semaphore; RX dùng DMA vòng và phát hiện IDLE | DMA |
| USART2 | ESP8266 | 115200 8N1, PA2/PA3 | Ngắt cho từng byte |
| I2C1 | EEPROM AT24Cxx | Địa chỉ thiết bị `0xA0`, địa chỉ ô nhớ 16-bit | Polling |
| GPIO (bit-bang) | XPT2046 | SPI mềm | Polling |
| GPIO | ESP_POWER, ESP_BUSY, LED_DEBUG, đèn nền và reset LCD | – | – |
| TIM2 | Timebase của HAL | 1 kHz | Ngắt |

**So sánh – biện luận** (Bảng 2.4 – các lựa chọn giao tiếp):
- **Giao tiếp LCD: FSMC song song 16-bit (chọn) hay SPI.** Với FSMC, mỗi pixel là một lần ghi bộ nhớ và dùng DMA được. SPI đẩy từng bit nối tiếp nên chậm hơn nhiều. Tính thời gian ghi toàn màn hình (153 600 B) cho từng phương án, rồi đối chiếu với số đo thực tế ở Chương 10.
- **Đẩy pixel: CPU chép hay DMA mem→mem (chọn).** DMA giải phóng CPU để LVGL vẽ buffer thứ hai cùng lúc (xem Hình 7.2).
- **Cảm ứng: SPI phần cứng hay SPI mềm (chọn).** Tốc độ đọc cảm ứng thấp nên không cần SPI nhanh. *Cần xem schematic KIT* để khẳng định lý do (ví dụ chân nối trên KIT không trùng với SPI phần cứng).
- **Ba cách nhận UART cùng có trong dự án**: polling, ngắt từng byte (USART2), DMA vòng + IDLE (USART1). So sánh tải CPU, độ phức tạp, nguy cơ mất byte và trường hợp nên dùng từng cách.
  - Biện luận: USART2 có lưu lượng thấp và parser xử lý từng byte, nên ngắt từng byte là đơn giản nhất.
  - Hướng cải tiến: chuyển USART2 sang DMA + IDLE.
- **Liên kết STM32–ESP: UART (chọn), SPI hay I2C.** SPI nhanh hơn nhưng phải phân vai master/slave, mà chế độ slave HSPI trên ESP8266 phức tạp. I2C chậm và ESP8266 không có I2C phần cứng.
  - **Tính băng thông**: 115200 baud tương đương khoảng 11,5 KB/s, nên khung lớn nhất 204 B mất khoảng **17,7 ms**, đủ cho nhịp độ của trò chơi.

**Hình vẽ**
- *Hình 2.5 – Ma trận bus rút gọn* (draw.io).
  - Master: CPU (bus I, D, S) và DMA2. Slave: Flash, SRAM, FSMC.
  - CCM chỉ nối với bus D của CPU, **không nối tới DMA**. Chi tiết này giải thích vì sao không đặt draw buffer vào CCM được.
  - Tô đậm đường DMA2 → FSMC (đọc SRAM ngoài) → FSMC (ghi LCD) để thấy cả hai lượt truy cập đều đi qua FSMC, nghĩa là có **tranh chấp bus**.
- *Hình 2.6 (tùy chọn) – Giản đồ thời gian một lần ghi FSMC, Mode A* (WaveDrom). Các tín hiệu NE1, NWE, A19 (RS), D[15:0]. Đánh dấu hai khoảng ADDSET và DATAST.

**Tham chiếu mã nguồn**: [fsmc.c](../Core/Src/fsmc.c), [dma.c](../Core/Src/dma.c), [usart.c](../Core/Src/usart.c), [i2c.c](../Core/Src/i2c.c), [lcd.c](../Drivers/LCD/lcd.c), [touch.c](../Drivers/Touch/touch.c), [at24c.c](../Drivers/EEPROM/at24c.c).

### 2.4 Kinh nghiệm khi đưa phần cứng vào hoạt động (bổ sung)

**Nội dung cần viết**
- **Thiếu 2 chân địa chỉ FSMC** trong cấu hình CubeMX khiến LCD và SRAM ngoài hoạt động sai (commit *"fix missing 2 pins of external SRAM"*). Chính lỗi này dẫn tới việc viết hàm self-test SRAM (mục 9.3).
- **Hiệu chuẩn cảm ứng**: quy trình chạm các điểm chuẩn, tính hệ số `xfac, yfac, xoff, yoff`, lưu 14 B cùng một byte hướng quét vào EEPROM, và tự nạp lại mỗi lần khởi động.

---

## CHƯƠNG 3. MÔI TRƯỜNG PHÁT TRIỂN, TỔ CHỨC MÃ NGUỒN VÀ DEBUG (bổ sung)

### 3.1 Chuỗi công cụ và quy trình build

**Nội dung cần viết**
- **STM32**:
  - CubeMX (`BKhoot.ioc`) sinh các thư mục `Core/`, `Drivers/` và `cmake/stm32cubemx`.
  - `CMakeLists.txt` ở thư mục gốc do nhóm tự viết; toolchain file `gcc-arm-none-eabi.cmake`; preset Debug/Release; Ninja.
  - `compile_commands.json` để clangd trong VS Code hiểu được mã nguồn.
- **Các tùy biến trong CMake** (trích từng đoạn và giải thích):
  1. `add_subdirectory(lvgl)` với `LV_BUILD_CONF_DIR`.
  2. `add_custom_command` sinh `questions_data.c/.h`.
  3. `target_link_options(-Wl,--wrap,malloc …)`.
- **ESP8266**: dùng ESP8266_RTOS_SDK (CMake). Component `main` biên dịch **chung file** `../../App/Logic/protocol.c` với STM32.
- **Nạp và debug**: STM32 nạp qua ST-Link (SWD); ESP8266 nạp qua bootloader UART. Ghi rõ cách đưa ESP vào chế độ nạp trên KIT này.
- **Lưu ý khi sinh lại code bằng CubeMX**: chỉ viết code trong các cặp `USER CODE BEGIN/END`.

**So sánh – biện luận** (Bảng 3.1): STM32CubeIDE · Keil MDK · Makefile thuần · PlatformIO · **CMake + CubeMX (chọn)**. Tiêu chí: chi phí bản quyền, không phụ thuộc IDE, dễ tích hợp thư viện ngoài, thêm được bước sinh mã tùy biến, chạy được trên CI, độ khó khi học.

**Hình vẽ**
- *Hình 3.1 – Pipeline build* (draw.io, luồng từ trái sang phải):
  - `BKhoot.ioc` –CubeMX→ `Core/`, `Drivers/`, `cmake/stm32cubemx`.
  - `App/questions.json` –`generate_questions.py`→ `questions_data.c/.h` (tô xám vì là file sinh tự động).
  - LVGL cùng `lv_conf.h`.
  - Tất cả đi vào CMake/Ninja → arm-none-eabi-gcc → ld (dùng `STM32F407XX_FLASH.ld` và `--wrap`) → `BKhoot.elf` → ST-Link → KIT.
  - Nhánh song song bên dưới: `esp8266/` cùng `protocol.c` → ESP8266_RTOS_SDK → file `.bin` → nạp qua UART → ESP8266.

### 3.2 Tổ chức mã nguồn

**Nội dung cần viết**
- Cây thư mục rút gọn (dùng lệnh `tree`) và vai trò của từng thư mục:
  - `App/Logic`: `game_logic`, `protocol`, `questions_data`.
  - `App/screens/<tên màn hình>`: mỗi màn hình là một module `.c/.h`.
  - `App/screens/common`: style chung, nút Home, danh sách xếp hạng.
  - `Drivers/{LCD, Touch, EEPROM, Rs232}`: BSP do nhóm tự viết.
  - `Middlewares`: FreeRTOS, LVGL.
  - `esp8266/main`: firmware ESP8266.
  - `scripts/`: script sinh mã.
- Nguyên tắc phụ thuộc: tầng trên chỉ gọi tầng dưới. Hai hướng gọi giữa màn hình và logic được quy định rõ: UI gọi `game_*`, logic gọi `gui_load_*`.

**Hình vẽ**
- *Hình 3.2 – Kiến trúc phân lớp* (draw.io). Các tầng xếp ngang, từ dưới lên: Phần cứng → CMSIS/HAL (CubeMX) → BSP (lcd, touch, at24c, rs232) → Middleware (FreeRTOS/CMSIS-RTOS2, LVGL) → Ứng dụng (khối Logic gồm game_logic, protocol, questions_data; khối UI gồm screens, ui_styles, gui_app).
  - Bên phải vẽ thêm một cột cho ESP8266: SDK (Wi-Fi, ESP-NOW, FreeRTOS) → uart_task, espnow_task, esp_debug.
  - Vẽ `protocol.c` thành một khối **vắt ngang cả hai cột** để thể hiện đây là mã dùng chung.

### 3.3 Chiến lược debug firmware qua RS232

**Nội dung cần viết**
- **Kênh log**: `debug_log()` định dạng chuỗi bằng `vsnprintf` rồi gửi bằng `HAL_UART_Transmit_DMA`. Semaphore nhị phân `uart_tx_sem` đảm bảo mỗi lúc chỉ có một bản tin được gửi; semaphore được nhả trong `HAL_UART_TxCpltCallback`. Nhờ vậy, task gọi log bị *block* trong khi chờ, thay vì bắt CPU chờ bận.
- **Kênh lệnh**: `HAL_UARTEx_ReceiveToIdle_DMA` nhận vào buffer vòng 128 B. Callback đẩy vị trí ghi hiện tại vào `uartRxQueue`. Task `CommandParser` tính ra đoạn dữ liệu mới (có xử lý quấn vòng) rồi thực thi lệnh:
  - `h`: in menu.
  - `1`: thống kê RTOS, gồm heap còn trống, heap trống nhỏ nhất từng đạt, và bảng task (trạng thái, ưu tiên, stack high-water mark).
- **Log trước khi RTOS chạy**: hàm self-test SRAM dùng `HAL_UART_Transmit` dạng blocking.
- **Log của ESP8266 được chuyển qua STM32** (`CMD_ESP_LOG`), nên chỉ một cổng RS232 là thấy log của cả hai chip. Mỗi lệnh giao thức đi qua đều được ghi dạng `TX/RX ESP CMD:xx`.
- **Các lỗi đã tìm ra nhờ kênh này**: HardFault do cấp phát bộ nhớ trong ISR; SRAM ngoài hỏng; kích thước stack của GUI task chưa hợp lý.

**So sánh – biện luận** (Bảng 3.2): SWD + GDB breakpoint · semihosting · ITM/SWO printf · SEGGER RTT · printf qua UART dạng blocking · **UART DMA + console lệnh (chọn)**. Tiêu chí: có cần debugger không, ảnh hưởng tới tính thời gian thực, chạy được trên nhiều KIT cùng lúc khi thử ngoài hiện trường, giao tiếp hai chiều, có xem được log ESP8266 không.
- Biện luận chính: breakpoint dừng CPU, làm STM32 mất đồng bộ với ESP và các KIT khác, nên không hợp để debug giao thức. KIT có sẵn cổng RS232. SWD vẫn được dùng khi cần phân tích HardFault.

**Hình vẽ**
- *Hình 3.3 – Luồng dữ liệu debug* (draw.io).
  - Chiều ra: task bất kỳ → `debug_log` → [semaphore] → DMA2 Stream7 → USART1 → bộ chuyển mức RS232 → terminal trên PC.
  - Chiều vào: PC → USART1 → DMA2 Stream2 (vòng) → ngắt IDLE → `uartRxQueue` → `CommandParser` → `debug_log`.
  - Nhánh ESP8266: `debug_log` của ESP → `CMD_ESP_LOG` → USART2 → Game task → `debug_log`.
- *Hình 3.4 – Ảnh chụp terminal*: kết quả của lệnh `1` (bảng task, heap) và dòng `EXTSRAM self-test: PASS (524288 bytes)`.

**Tham chiếu mã nguồn**: [rs232.c](../Drivers/Rs232/rs232.c), [esp_debug.c](../esp8266/main/esp_debug.c), [main.c](../Core/Src/main.c).

---

## CHƯƠNG 4. THIẾT KẾ PHẦN MỀM VÀ GIAO THỨC TRUYỀN THÔNG

### 4.1 Kiến trúc phần mềm tổng thể

**Nội dung cần viết**
- Các thành phần của hai firmware và luồng dữ liệu đầu–cuối giữa chúng.
- **Nguyên tắc thiết kế**:
  1. STM32 giữ toàn bộ logic trò chơi; ESP8266 gần như trong suốt.
  2. **Host là nguồn sự thật duy nhất** về thời gian, đáp án đúng và điểm số.
  3. Mã giao thức được dùng chung cho cả hai firmware.
  4. UI chỉ gọi API `game_*`; logic chỉ gọi `gui_load_*`/`gui_update_*`.

**Hình vẽ**
- *Hình 4.1 – Component diagram* (PlantUML).
  - Package "STM32 firmware" gồm: Screens (10 màn hình), ui_styles, gui_app (porting LVGL), game_logic, protocol, questions_data, rs232.
  - Package "ESP8266 firmware" gồm: uart_task (rx/tx), espnow_task, esp_debug, protocol (dùng chung).
  - Các interface cần vẽ:
    - `game_*()`: Screens → game_logic.
    - `gui_load_*()`: game_logic → Screens.
    - `comm_send_cmd()` và `HAL_UART_RxCpltCallback`: game_logic ↔ USART2.
    - `uart_rx_queue`/`uart_tx_queue`: bên trong ESP8266.
    - `esp_now_send()`, `recv_cb`, `send_cb`: ESP8266 ↔ sóng vô tuyến.

### 4.2 Giao thức liên kết UART giữa STM32 và ESP8266

**Nội dung cần viết**
- **Định dạng khung**: `| SOF 0xAA | CMD 1B | LEN 1B (≤200) | PAYLOAD LEN B | CHK 1B |`. CHK là XOR của CMD, LEN và toàn bộ PAYLOAD. Một khung dài tối đa 204 B.
- **Bộ phân tích khung** là máy 5 trạng thái xử lý từng byte, dùng chung cho cả hai phía: WAIT_START → CMD → LEN → PAYLOAD → CHECKSUM. Nếu LEN lớn hơn 200 thì quay về trạng thái đầu. Nếu checksum sai thì bỏ khung.
- **Tạo khung**: hàm `protocol_build_frame`.
- **Ưu điểm**: đơn giản, không cấp phát động, overhead chỉ 4 B mỗi khung.
- **Nhược điểm cần thừa nhận**:
  - XOR 8-bit phát hiện lỗi yếu: không phát hiện được hai byte bị đổi chỗ, hay hai lỗi trùng cùng một bit.
  - Byte `0xAA` có thể xuất hiện trong payload, nên sau khi mất byte, parser có thể đồng bộ sai (không có byte stuffing).
  - Tầng UART không có ACK hay truyền lại; thiết kế dựa vào việc dây nối giữa hai chip rất ngắn.

**So sánh – biện luận** (Bảng 4.1): văn bản kiểu AT/JSON · **nhị phân SOF+LEN+XOR (chọn)** · nhị phân với CRC-16 · SLIP/COBS (byte stuffing) · nanopb/Protobuf. Tiêu chí: overhead, chi phí CPU, khả năng tự đồng bộ lại, khả năng phát hiện lỗi, dễ đọc khi debug. Kết luận kèm hướng cải tiến: dùng CRC-16/CCITT và COBS.

**Hình vẽ**
- *Hình 4.2 – Định dạng khung* (draw.io, dạng bytefield). Một hàng ô chữ nhật có độ rộng tỷ lệ với số byte. Ghi offset 0, 1, 2, 3 … 3+LEN phía trên. Vẽ dấu ngoặc nhọn dưới đoạn CMD..PAYLOAD với chú thích "vùng tính XOR".
  - Hàng thứ hai là định dạng **trên sóng ESP-NOW**: `| CMD | LEN | PAYLOAD |`. Chú thích: không cần SOF và CHK vì tầng MAC 802.11 đã có FCS/CRC.
- *Hình 4.3 – State diagram của parser* (PlantUML). 5 trạng thái S0..S4 với các chuyển:
  - S0 → S1 khi byte = 0xAA.
  - S1 → S2 với mọi byte (lưu cmd).
  - S2 → S0 khi len > 200; S2 → S4 khi len = 0; S2 → S3 khi len > 0.
  - S3 tự lặp khi idx < len (lưu byte); S3 → S4 khi idx = len.
  - S4 → S0 khi checksum đúng (xuất message, trả về 1) và cả khi checksum sai (bỏ khung).

**Tham chiếu mã nguồn**: [protocol.h](../App/Logic/protocol.h), [protocol.c](../App/Logic/protocol.c).

### 4.3 Giao thức tầng ứng dụng (tập lệnh)

**Nội dung cần viết**
- Bảng lệnh đầy đủ, ghi rõ hướng truyền, payload trên UART, payload trên sóng và kiểu truyền.
- Các mã **dự trữ, chưa dùng**: 0x01, 0x02, 0x08, 0x0A, 0x0C.

Bảng 4.2 – Tập lệnh (đã điền sẵn từ mã nguồn, cần kiểm lại khi viết):

| Mã | Lệnh | Hướng | Payload trên UART | Trên sóng | Kiểu truyền |
|---|---|---|---|---|---|
| 0x03 | SCAN_HOSTS | Player → mọi KIT | – | – | Broadcast |
| 0x04 | HOST_FOUND | Host → Player | Host gửi tên phòng `"BKhoot Lobby\0"`; ESP phía Player **chèn MAC nguồn**, thành `[MAC6][tên]` | `[tên]` | Broadcast |
| 0x05 | JOIN_HOST | Player → Host | Player gửi `[MAC Host]`; ESP phía Host **chèn MAC nguồn**, thành `[MAC Player]` | LEN = 0 | Unicast |
| 0x06 | JOIN_ACK | Host → Player | Host gửi `[MAC Player][ID]`; ESP **bóc MAC đích** | `[ID]` | Unicast |
| 0x0E | START_QUIZ | Host → mọi KIT | – | – | Broadcast |
| 0x07 | BROADCAST_QUESTION | Host → các Player | `[thời gian ms, u32 LE][câu hỏi\0][A\0][B\0][C\0][D\0]` | giữ nguyên | **Unicast lần lượt tới từng peer** |
| 0x09 | SUBMIT_ANSWER | Player → Host | Player gửi `[idx]`; ESP phía Host chèn MAC, thành `[MAC][idx]` | `[idx]` | Unicast |
| 0x0B | SEND_FEEDBACK | Host → Player | `[MAC][đúng/sai][điểm]`; ESP bóc MAC | `[đúng][điểm]` | Unicast |
| 0x12 | TIME_UP | Host → các Player | – | – | Unicast lần lượt |
| 0x11 | PLAYER_DROPPED | ESP → STM32 (chỉ trong một KIT) | `[MAC]` | không phát ra sóng | – |
| 0x0F | ESP_READY | ESP → STM32 | – | – | Chỉ trong KIT |
| 0x10 | ESP_LOG | ESP → STM32 | chuỗi log | – | Chỉ trong KIT |

- **Nguyên tắc "cầu nối có dịch địa chỉ"**: STM32 nhận diện người chơi bằng MAC. ESP **chèn MAC nguồn** vào các lệnh mà bên nhận cần biết ai gửi (HOST_FOUND, JOIN_HOST, SUBMIT_ANSWER), và **bóc MAC đích** khỏi các lệnh gửi riêng cho một người (JOIN_ACK, SEND_FEEDBACK). Lợi ích: tiết kiệm byte trên sóng, và STM32 không cần biết gì về API ESP-NOW.
- **`answerIndex` không bao giờ được gửi tới Player**, nên Player không thể dò được đáp án trước (chống gian lận ở mức cơ bản).
- **Giới hạn độ dài**: câu hỏi cộng 4 đáp án (kể cả `\0`) không quá 196 B. Nếu vượt, `master_broadcast_current_question` sẽ cắt bớt.

**Hình vẽ**
- *Hình 4.4 – Chèn và bóc MAC* (draw.io). Vẽ hai ví dụ, mỗi ví dụ là các thanh byte xếp dọc theo đường đi, kèm mũi tên chỉ phép biến đổi ở mỗi chặng:
  - (a) SUBMIT_ANSWER: UART của Player `[AA|09|01|idx|CHK]` → sóng `[09|01|idx]` → ESP phía Host chèn MAC → UART của Host `[AA|09|07|MAC×6|idx|CHK]`.
  - (b) SEND_FEEDBACK theo chiều ngược lại, ESP bóc MAC.

### 4.4 Độ tin cậy của liên kết không dây

**Nội dung cần viết**
- **Tìm phòng dùng broadcast**: không có ACK và chấp nhận mất gói; người chơi bấm Refresh để quét lại.
- **Chuyển câu hỏi và TIME_UP từ broadcast sang unicast** (commit *"change from broadcast to unicast to use built-in ACK"*). ESP gửi lần lượt tới từng peer đã vào phòng. `esp_now_send_cb` trả về trạng thái ACK ở tầng MAC; tầng MAC tự truyền lại khi cần.
- **Theo dõi peer**: bảng `tracked_peers[20]`, mỗi phần tử gồm `{mac, miss_count, active}`. Gửi thành công thì đặt `miss_count` về 0. Gửi thất bại **3 lần liên tiếp** thì gọi `drop_peer`: xóa peer và gửi `CMD_PLAYER_DROPPED` lên STM32. Khi đó Host đánh dấu người chơi là inactive và không chờ câu trả lời của họ nữa.
- Player cũng lưu MAC của Host (`joined_host_mac`) để gửi câu trả lời dạng unicast.
- **Cấp ID người chơi**: Host cấp `P<n>` theo số thứ tự slot (chỉ số slot + 1). Người chơi vào lại với cùng MAC sẽ **giữ nguyên slot, ID và điểm**.
- **Chi phí**: N người chơi thì mỗi câu hỏi phải phát N lần. Đo thời gian phát cho N = 1, 2, 3… ở Chương 10.
- **Hạn chế cần nêu**:
  - Phần tử inactive trong `tracked_peers` không được dùng lại, nên sau tổng cộng 20 lần thêm peer (tính cả số lần vào lại) thì không theo dõi được peer mới nữa.
  - Peer broadcast cũng chiếm một chỗ trong bảng peer của ESP-NOW. Cần đối chiếu giới hạn số peer trong tài liệu Espressif.

**So sánh – biện luận**
- Bảng 4.3 – Cơ chế truyền câu hỏi: broadcast thuần · broadcast + ACK tầng ứng dụng + truyền lại · **unicast + ACK tầng MAC (chọn)** · TCP qua Wi-Fi. Tiêu chí: biết được gói đã tới hay chưa, lưu lượng, độ phức tạp, độ trễ khi N lớn, khả năng phát hiện người chơi rớt.
- Bảng 4.4 – Cách định danh người chơi: dùng thẳng MAC · để Player tự chọn ID · **Host cấp ID theo slot (chọn)**. Tiêu chí: dễ đọc trên giao diện, không trùng lặp, giữ được danh tính khi vào lại.

**Hình vẽ**
- *Hình 4.5 – State diagram của một peer trên ESP8266 phía Host* (PlantUML):
  - [Chưa theo dõi] → [Active, miss=0] khi nhận JOIN_HOST (hành động `track_peer`).
  - [Active] tự lặp khi gửi thành công (đặt miss=0) hoặc gửi thất bại (miss++).
  - [Active] → [Inactive] khi miss ≥ 3 (hành động `drop_peer`, gửi `PLAYER_DROPPED`).
  - Ghi chú: khi người chơi vào lại, ESP tạo một phần tử mới trong bảng.
- *Hình 4.6 – Sequence diagram "Phát hiện người chơi rớt"* (PlantUML). Các lifeline: Host STM32, Host ESP, Player ESP (đã tắt nguồn).
  - BROADCAST_QUESTION → unicast bị lỗi (miss=1) → TIME_UP bị lỗi (miss=2) → câu tiếp theo bị lỗi (miss=3) → `PLAYER_DROPPED[MAC]` → STM32 đặt `player_active=false` → nếu mọi người còn lại đã trả lời thì kết thúc câu.

### 4.5 Thiết kế firmware ESP8266

**Nội dung cần viết**
- **Các task**: `uart_rx_thread`, `uart_tx_thread`, `espnow_thread` (ưu tiên 10) và `pwm_blink_task` (ưu tiên 5). Hàng đợi con trỏ `uart_rx_queue` và `uart_tx_queue` mỗi hàng 10 phần tử.
- **Callback `recv_cb`/`send_cb`** chạy trong ngữ cảnh task Wi-Fi, nên chỉ đóng gói dữ liệu rồi đẩy vào hàng đợi, không xử lý nặng.
- **Trình tự khởi tạo**: NVS → UART → Wi-Fi ở chế độ STA, kênh 1 → `esp_now_init` → thêm peer broadcast → gửi `ESP_READY`.
- **Nhận xét và cải tiến**: `uart_rx_thread` gọi `malloc` một `protocol_msg_t` cho **mỗi byte** nhận được rồi `free` nếu chưa đủ khung, nên heap bị cấp phát và giải phóng liên tục. Cải tiến: parse vào một biến tĩnh và chỉ `malloc` khi đã nhận đủ khung.

**Hình vẽ**
- *Hình 4.7 – Task và hàng đợi của ESP8266* (draw.io):
  - Chiều gửi ra sóng: driver UART0 → `uart_rx_thread` (parser) → `uart_rx_queue` → `espnow_thread` → `esp_now_send` → sóng.
  - Chiều nhận từ sóng: sóng → `espnow_recv_cb` (task Wi-Fi) → `uart_task_send` → `uart_tx_queue` → `uart_tx_thread` → UART0.
  - Nhánh theo dõi liên kết: `espnow_send_cb` → bảng peer → `PLAYER_DROPPED` → `uart_tx_queue`.

**Tham chiếu mã nguồn**: [espnow_task.c](../esp8266/main/espnow_task.c), [uart_task.c](../esp8266/main/uart_task.c), [main.c (ESP)](../esp8266/main/main.c).

---

## CHƯƠNG 5. MÁY TRẠNG THÁI TRÒ CHƠI

### 5.1 Mô hình hóa

**Nội dung cần viết**
- **Trạng thái có hai chiều**: vai trò `GameRole_t {NONE, MASTER, SLAVE}` và trạng thái `GameState_t {INIT, LOBBY, QUESTION, RESULTS, FEEDBACK}`. Ý nghĩa của mỗi trạng thái phụ thuộc vào vai trò (Bảng 5.1).
- **Nguồn sự kiện**:
  - UI: callback của LVGL (chọn vai trò, Start, Next, trả lời, Join, Home).
  - Mạng: các message lấy ra từ `commQueue`.
  - Timer: cờ `master_timer_expired`.
- **Cài đặt**: hai lớp `switch` lồng nhau (theo vai trò, rồi theo lệnh) trong `game_task_func`. Mỗi hành động có **điều kiện chặn** kiểm tra `current_state`, ví dụ chỉ nhận đáp án khi đang ở QUESTION.

Bảng 5.1 – Ý nghĩa trạng thái theo vai trò:

| State | Host (MASTER) | Player (SLAVE) |
|---|---|---|
| INIT | – | Đang quét phòng |
| LOBBY | Đang chờ người chơi | Đã gửi Join, đang chờ Host |
| QUESTION | Đã phát câu hỏi, đang nhận đáp án | Đang hiện câu hỏi, chờ chọn |
| FEEDBACK | – | Đã gửi đáp án, đang chờ phản hồi |
| RESULTS | Bảng xếp hạng sau câu hoặc kết quả cuối | Đã nhận phản hồi, hoặc đang ở màn "Time's up" |

**So sánh – biện luận** (Bảng 5.2): **switch-case phẳng (chọn)** · bảng chuyển trạng thái (mảng `[state][event] → handler`) · State pattern dùng con trỏ hàm · máy trạng thái phân cấp hoặc framework QP-nano. Tiêu chí: số trạng thái, độ dễ đọc, khả năng mở rộng, khả năng kiểm thử.
- Kết luận: với 5 trạng thái × 2 vai trò, `switch` là đủ. Nếu sau này thêm chế độ chơi mới thì nên chuyển sang bảng chuyển trạng thái.

### 5.2 FSM của Host (Master)

Bảng 5.3 – Bảng chuyển trạng thái của Host (đã điền sẵn từ [game_logic.c](../App/Logic/game_logic.c)):

| Trạng thái | Sự kiện | Điều kiện | Hành động | Trạng thái kế tiếp |
|---|---|---|---|---|
| (NONE) | Chạm "Host (Master)" | – | Xóa danh sách người chơi và điểm | LOBBY (màn Lobby) |
| LOBBY | `CMD_SCAN_HOSTS` | – | Gửi `HOST_FOUND "BKhoot Lobby"` | LOBBY |
| LOBBY | `CMD_JOIN_HOST [MAC]` | Chưa đủ 20 người, hoặc MAC đã có trong danh sách | Cấp slot mới hoặc giữ slot cũ; `active=true`; gửi `JOIN_ACK`; cập nhật "Players: n" | LOBBY |
| LOBBY | Chạm "Start Quiz" | Ngân hàng câu hỏi không rỗng | Gửi `START_QUIZ`; phát câu 0; chạy thanh thời gian | QUESTION |
| QUESTION | `CMD_SUBMIT_ANSWER` | Là đáp án đầu tiên của người đó | Chấm điểm (+1 nếu đúng); gửi `SEND_FEEDBACK` | QUESTION, hoặc RESULTS nếu mọi người đã trả lời |
| QUESTION | `CMD_JOIN_HOST` (vào giữa câu) | – | Đánh dấu người đó "đã trả lời" để không phải chờ | QUESTION |
| QUESTION | `CMD_PLAYER_DROPPED` | – | `active=false`; kiểm tra xem mọi người còn lại đã trả lời chưa | QUESTION hoặc RESULTS |
| QUESTION | Hết giờ (animation kết thúc, đặt cờ) | `state == QUESTION` | Gửi `TIME_UP`; xếp hạng; hiện màn Leaderboard | RESULTS |
| RESULTS | Chạm "Next" | Còn câu hỏi | Tăng chỉ số câu; phát câu tiếp theo | QUESTION |
| RESULTS | Chạm "Next" | Hết câu hỏi | Xếp hạng cuối; hiện màn Congratulations | RESULTS |
| RESULTS | `SUBMIT_ANSWER` đến trễ | – | Bỏ qua | RESULTS |
| Bất kỳ | Chạm Home | – | `role = NONE` | Màn chọn vai trò |

**Hình vẽ**
- *Hình 5.1 – UML state machine của Host* (PlantUML; có thể chuyển thẳng từ Bảng 5.3). Các trạng thái: RoleSelect → Lobby → Question ⇄ Leaderboard → Congrats. Nhãn trên mũi tên theo cú pháp `sự kiện [điều kiện] / hành động`.
  - Gom Lobby, Question, Leaderboard và Congrats vào một **superstate "InGame"**, rồi chỉ vẽ **một** mũi tên "Home / role=NONE" từ superstate về RoleSelect cho gọn.
  - Ghi chú: Leaderboard và Congrats đều ứng với `GAME_STATE_RESULTS` trong mã nguồn.

### 5.3 FSM của Player (Slave)

Bảng 5.4 – Bảng chuyển trạng thái của Player:

| Trạng thái | Sự kiện | Điều kiện | Hành động | Trạng thái kế tiếp |
|---|---|---|---|---|
| (NONE) | Chạm "Join (Slave)" | – | Xóa danh sách phòng; `player_id = 0` | INIT (màn Scan) |
| INIT | Chạm Refresh | – | Gửi `SCAN_HOSTS`; xóa danh sách | INIT |
| INIT | `CMD_HOST_FOUND` | Chưa đủ 10 phòng | Thêm phòng vào danh sách | INIT |
| INIT | Chọn một phòng | – | Gửi `JOIN_HOST [MAC]`; hiện màn Waiting | LOBBY |
| LOBBY | `CMD_JOIN_ACK [ID]` | `len == 1` | Lưu `player_id` | LOBBY |
| Bất kỳ | `CMD_BROADCAST_QUESTION` | – | Giải mã payload; hiện màn Answer (có `P<id>` và thanh thời gian) | QUESTION |
| QUESTION | Chạm một đáp án | – | Dừng animation; gửi `SUBMIT_ANSWER` | FEEDBACK |
| Bất kỳ | `CMD_SEND_FEEDBACK` | – | Hiện màn Correct/Incorrect kèm điểm | RESULTS |
| QUESTION / FEEDBACK | `CMD_TIME_UP` | Chưa nhận phản hồi | Hiện màn "Time's up – No answer counted" | RESULTS |
| RESULTS | `CMD_TIME_UP` | – | Bỏ qua | RESULTS |
| Bất kỳ | Chạm Home | – | Về màn chọn vai trò; **vai trò SLAVE vẫn giữ nguyên** | – |

- Cần bàn luận: khi Player bấm Home, vai trò SLAVE vẫn giữ nguyên, nên câu hỏi tiếp theo sẽ tự kéo màn hình quay lại màn trả lời. Báo cáo cần nói rõ đây là hành vi **có chủ đích** (cho phép quay lại trận) hay là **hạn chế**.

**Hình vẽ**
- *Hình 5.2 – UML state machine của Player* (PlantUML). Các trạng thái: RoleSelect → Scanning (INIT) → Waiting (LOBBY) → Answering (QUESTION) → WaitFeedback (FEEDBACK) → ShowResult (RESULTS).
  - ShowResult có **2 trạng thái con**: FeedbackScreen và TimeUpScreen.
  - Vẽ mũi tên `TIME_UP` từ Answering và từ WaitFeedback sang TimeUpScreen.
  - Vẽ mũi tên `BROADCAST_QUESTION` từ ShowResult quay về Answering.

### 5.4 Đồng bộ trạng thái giữa Host và Player, xử lý tranh chấp

**Nội dung cần viết**
- **Host là nguồn sự thật**: Player chỉ chuyển trạng thái khi nhận lệnh từ Host (câu hỏi, phản hồi, hết giờ). Thanh đếm ngược trên Player **chỉ để hiển thị**.
- **Tranh chấp "hết giờ" và "mọi người đã trả lời"**:
  - Timer chạy trong GUI task (callback `ready_cb` của animation LVGL). Callback này **chỉ đặt cờ** `volatile master_timer_expired`.
  - Chỉ Game task được kết thúc câu hỏi, thông qua `master_end_question()`. Hàm này chạy khi đang giữ khóa LVGL và kiểm tra lại `state == QUESTION`, nên mỗi câu **chỉ kết thúc đúng một lần**.
  - Khi rời màn câu hỏi, animation bị xóa trong `LV_EVENT_SCREEN_UNLOADED`, nên callback cũ không thể chạy muộn.
- **Các quy tắc chơi**: chỉ đáp án đầu tiên được tính; đáp án đến sau `TIME_UP` bị bỏ qua; người vào giữa câu thì Host không chờ họ trả lời câu đó.
- **Xếp hạng**: dùng sắp xếp chèn **ổn định** (điểm giảm dần; bằng điểm thì giữ thứ tự vào phòng). Độ phức tạp O(n²) với n ≤ 20 là chấp nhận được.

**So sánh – biện luận**
- Bảng 5.5 – Nơi đặt timer: **tại Host (chọn)** · mỗi Player tự chạy timer · đồng bộ đồng hồ bằng timestamp. Tiêu chí: công bằng, độ lệch do trễ truyền, độ phức tạp.
- Bảng 5.6 – Kết thúc câu: **kết thúc sớm khi mọi người đã trả lời (chọn)** · luôn chờ hết giờ. Tiêu chí: nhịp độ trò chơi, trải nghiệm người chơi.

**Hình vẽ**
- *Hình 5.3 – Sequence diagram xử lý tranh chấp* (PlantUML). Các lifeline: GUI_Task, Game_Logic_Task, lvgl_mutex. Dùng khối `alt` cho hai trường hợp:
  - [Hết giờ trước]: GUI đặt cờ → Game task thấy cờ → lock → `state == QUESTION` → kết thúc câu → unlock. Đáp án cuối đến sau đó gặp `state == RESULTS` nên bị bỏ qua.
  - [Đáp án cuối đến trước]: Game task kết thúc câu. Animation bị xóa khi rời màn hình (hoặc cờ đến sau gặp điều kiện chặn sai).

### 5.5 Ánh xạ trạng thái sang màn hình
Một bảng nối (vai trò, trạng thái) với hàm `gui_load_*` tương ứng, làm cầu nối sang Chương 7.

---

## CHƯƠNG 6. LUỒNG VẬN HÀNH TRÒ CHƠI (HOST, PLAYER)

### 6.1 Tổng quan kịch bản

**Nội dung cần viết**
- Bốn giai đoạn: Khởi động → Tìm và vào phòng → Các vòng hỏi đáp → Kết thúc. Mỗi giai đoạn mô tả từ góc nhìn người dùng Host và người dùng Player, kèm **ảnh chụp LCD thật**.

**Hình vẽ**
- *Hình 6.1 – Activity diagram có swimlane* (PlantUML, hai làn `|Host|` và `|Player|`):
  - Bắt đầu → chọn vai trò.
  - Làn Host: mở Lobby và chờ. Làn Player: quét → chọn phòng → chờ.
  - Host bấm Start, rồi vào vòng lặp: Host phát câu → Player trả lời → Host chấm → Player xem phản hồi → hết giờ hoặc đủ đáp án → Host xem Leaderboard → Next.
  - Kết thúc bằng màn Congrats.

### 6.2 Khởi động hệ thống

**Nội dung cần viết**
- **Trình tự trên STM32**: `HAL_Init` → `SystemClock_Config` → `MX_*_Init` (GPIO, DMA, FSMC, I2C1, TIM1, USART1, USART2) → `ExtSRAM_SelfTest` → `vPortDefineHeapRegions` → đăng ký callback DMA → bật nhận DMA cho USART1 → `osKernelInitialize` → `MX_FREERTOS_Init` (tạo mutex, semaphore, queue và 5 task) → `osKernelStart`.
- **GUI task**: `lv_init`, đặt tick, `lcd_init`, `touch_Init` (nạp hệ số hiệu chuẩn từ EEPROM), tạo display và 2 buffer, tạo input device, `gui_app_init` → màn chọn vai trò.
- **COMM task**: khởi tạo parser và bật ngắt nhận USART2.
- **ESP8266**: NVS → UART → Wi-Fi STA kênh 1 → ESP-NOW và peer broadcast → gửi `ESP_READY` → ghi log.
- Giải thích vì sao **self-test phải chạy trước** `vPortDefineHeapRegions` và trước khi scheduler chạy: toàn bộ heap và stack của các task động đều nằm trong SRAM ngoài.
- Nhận xét: nếu self-test **FAIL**, hiện code chỉ in địa chỉ lỗi rồi vẫn chạy tiếp. Cần bàn phương án dừng hệ thống (`Error_Handler`).

**Hình vẽ**
- *Hình 6.2 – Flowchart khởi động* (draw.io), hai cột song song STM32 | ESP8266.
  - Trong cột STM32 có hình thoi "Self-test PASS?"; nhánh FAIL in địa chỉ lỗi.
  - Kết thúc bằng mũi tên `ESP_READY` từ cột ESP8266 sang cột STM32.

### 6.3 Tìm phòng và tham gia

**Nội dung cần viết**
- Diễn giải từng bước theo Hình 6.3.
- Trường hợp có nhiều Host cùng lúc: danh sách tối đa 10 phòng, nhưng mọi phòng đều tên "BKhoot Lobby". Cải tiến: thêm hậu tố từ MAC vào tên phòng.

**Hình vẽ**
- *Hình 6.3 – Sequence diagram "Tìm phòng và tham gia"* (PlantUML). Các lifeline: Player UI (GUI_Task) · Player Game task · Player ESP · Host ESP · Host Game task · Host UI. Trình tự:
  1. Player chạm Refresh → `game_slave_scan_hosts` → UART `SCAN_HOSTS` → ESP phát broadcast.
  2. Host ESP nhận → UART → Host Game task gửi `HOST_FOUND("BKhoot Lobby")` → Host ESP phát broadcast.
  3. Player ESP nhận và chèn MAC → UART `[MAC|tên]` → Game task lưu vào `discovered_hosts`, lock, gọi `gui_slave_add_host_to_list`.
  4. Player chọn phòng → `JOIN_HOST[MAC Host]` → ESP lưu `joined_host`, theo dõi peer Host, unicast `[05|00]`.
  5. Host ESP gọi `track_peer(player)`, chèn MAC → Host Game task cấp slot, đặt active, cập nhật số người ở Lobby, gửi `JOIN_ACK[MAC|ID]`.
  6. Host ESP bóc MAC → unicast `[06|01|ID]` → Player Game task đặt `player_id = ID`.
  - Broadcast vẽ **nét đứt**, unicast vẽ **nét liền** kèm ghi chú "MAC ACK".

### 6.4 Vòng hỏi đáp

**Hình vẽ**
- *Hình 6.4 – Sequence diagram một vòng hỏi đáp* (PlantUML). Các lifeline: Host UI · Host Game · Host ESP · ESP của Player k · Game của Player k · UI của Player k. Trình tự:
  1. Host chạm Start hoặc Next → `master_broadcast_current_question` đóng gói `[duration][Q][A..D]` → UART → ESP lặp unicast tới từng peer active (có ACK) → `gui_load_master_question_screen` chạy animation thời gian.
  2. Player giải mã, lock, gọi `gui_load_slave_answer_screen` (hiện `P<id>`, 4 nút, animation).
  3. Player chạm đáp án → trạng thái FEEDBACK → `SUBMIT_ANSWER[idx]` → unicast tới Host.
  4. Host ESP chèn MAC → Host kiểm tra và chấm điểm → `SEND_FEEDBACK[MAC|đúng/sai|điểm]` → ESP bóc MAC → Player hiện màn Correct/Incorrect và điểm.
  5. Khối `alt`: [mọi người đã trả lời] hoặc [hết giờ] → `master_end_question` gửi `TIME_UP` unicast tới mọi Player → Host hiện Leaderboard; Player nào chưa nhận phản hồi thì hiện "Time's up".
  6. Host chạm Next → lặp lại.
  - Đánh dấu các mốc **t₁ (phát → hiển thị)** và **t₂ (chạm → phản hồi)** trên sơ đồ; hai mốc này được đo ở Chương 10.

### 6.5 Kết thúc trò chơi

**Nội dung cần viết**
- Khi bấm Next ở câu cuối, Host xếp hạng lần cuối và hiện màn Congratulations: danh sách cuộn được, mỗi dòng dạng "hạng. P<id> … điểm".
- **Hạn chế**: Player không nhận được thông báo kết thúc riêng. Cải tiến: thêm lệnh `GAME_OVER` và gửi hạng cá nhân tới từng Player.
- **Nút Home**: với Host, đặt vai trò về NONE (không quay lại phòng cũ được, chọn Host lần nữa sẽ mở phòng mới); với Player, xem mục 5.3.

### 6.6 Xử lý tình huống ngoại lệ

Bảng 6.1 – Các tình huống ngoại lệ:

| Tình huống | Cách phát hiện | Cách xử lý | Kết quả người dùng thấy |
|---|---|---|---|
| Player mất nguồn hoặc ra khỏi vùng phủ sóng | 3 lần unicast liên tiếp thất bại | `PLAYER_DROPPED` → đánh dấu inactive | Host không chờ người đó nữa |
| Player vào lại (cùng KIT) | MAC đã có trong danh sách | Giữ slot, ID và điểm | Vẫn hiện đúng `P<id>` cũ |
| Player vào giữa câu hỏi | Host đang ở trạng thái QUESTION | Đánh dấu người đó đã trả lời | Người đó chơi từ câu tiếp theo |
| Đáp án đến sau `TIME_UP` | `state != QUESTION` | Bỏ qua | Player thấy "Time's up" |
| Bấm đáp án nhiều lần | `player_answered` đã bật | Chỉ tính lần đầu | – |
| Phòng đã đủ 20 người | `connected_players ≥ 20` | Không gửi `JOIN_ACK` | Player chờ mãi (**hạn chế**) |
| Host rời phòng | `role = NONE` | Bỏ qua mọi message sau đó | Player đứng yên ở màn hiện tại (**hạn chế**) |
| Phòng không có ai | `active_count = 0` | Chỉ kết thúc câu khi hết giờ | – |
| Khung UART bị lỗi | Checksum sai | Bỏ khung | Có thể mất một lệnh |

**Hình vẽ**
- *Hình 6.5 (tùy chọn) – Flowchart xử lý `SUBMIT_ANSWER` ở Host* (draw.io). Các hình thoi theo thứ tự: `state == QUESTION`? → `len ≥ 7`? → người chơi có trong danh sách? → đã trả lời chưa? → đúng không? → mọi người đã trả lời chưa?

---

## CHƯƠNG 7. ỨNG DỤNG THƯ VIỆN ĐỒ HỌA LVGL CHO GIAO DIỆN NGƯỜI DÙNG

### 7.1 Lựa chọn thư viện đồ họa

**Nội dung cần viết**
- Giới thiệu LVGL: giấy phép MIT, viết bằng C, có sẵn widget, style, animation, hệ thống sự kiện, hỗ trợ nhiều RTOS. Dự án dùng phiên bản **9.6**.
- Các cấu hình chính trong `lv_conf.h`:
  - `LV_COLOR_DEPTH 16`.
  - `LV_USE_OS LV_OS_CMSIS_RTOS2`.
  - `LV_USE_STDLIB_MALLOC LV_STDLIB_CLIB` (nghĩa là LVGL gọi `malloc` chuẩn, mà `malloc` đã được bọc để trỏ vào heap của FreeRTOS).
  - `LV_DEF_REFR_PERIOD 33` ms (khoảng 30 fps).
  - Font Montserrat 14, bộ vẽ phần mềm (SW renderer).

**So sánh – biện luận** (Bảng 7.1): **LVGL (chọn)** · TouchGFX · STemWin/emWin · µGFX · tự viết hàm vẽ. Tiêu chí:
- Giấy phép sử dụng.
- RAM và Flash tối thiểu.
- Bộ widget và animation có sẵn.
- Khả năng tích hợp RTOS.
- Công cụ thiết kế giao diện.
- **Phụ thuộc phần cứng**: TouchGFX được tối ưu cho DMA2D/LTDC, mà F407 không có hai khối này, nên lợi thế của TouchGFX giảm đi.
- Cộng đồng và tài liệu.
- Khả năng chạy thử mã UI trên PC (mô phỏng).

### 7.2 Porting LVGL lên KIT

**Nội dung cần viết**
- **Display**: `lv_display_create(240, 320)`; mỗi buffer = 240 × 80 × 2 B = **38 400 B** (1/4 màn hình); **2 buffer**; chế độ `LV_DISPLAY_RENDER_MODE_PARTIAL`.
  - `flush_cb` gọi `lcd_set_address` để đặt cửa sổ ghi GRAM, rồi `HAL_DMA_Start_IT` chép từ buffer tới `LCD->LCD_RAM` (địa chỉ đích cố định).
  - Khi DMA xong, ngắt TC gọi `lv_display_flush_ready`.
- **Ràng buộc DMA**: thanh ghi NDTR chỉ đếm tối đa 65 535 phần tử. 1/4 màn hình là 19 200 pixel nên nằm trong giới hạn. Toàn màn hình là 76 800 pixel, vượt giới hạn, nên chế độ FULL phải chia nhỏ lần truyền. Đây là **một lý lẽ khi chọn chế độ render**.
- **Input**: `my_touchpad_read` chống dội (cần 2 lần đọc liên tiếp để xác nhận "nhấn", 3 lần để xác nhận "nhả") và lọc tọa độ bằng EMA (3/4 giá trị cũ + 1/4 giá trị mới). Hệ số hiệu chuẩn lấy từ EEPROM.
- **Tick**: `lv_tick_set_cb(HAL_GetTick)`. **Vòng lặp của GUI task**: lock → `lv_timer_handler` → unlock → `osDelay(5)`.
- **Bộ nhớ của LVGL**: `malloc` → `__wrap_malloc` → `pvPortMalloc` (heap_5, nằm trong SRAM ngoài).

**So sánh – biện luận**
- Bảng 7.2 – Chế độ render và kích thước buffer:
  - FULL: cần một framebuffer 153 600 B, phải đặt ở SRAM ngoài, và vượt giới hạn NDTR như đã nói.
  - DIRECT.
  - PARTIAL, 1 buffer 1/10 màn hình trong SRAM trong (phiên bản cũ, commit `13bb872`).
  - **PARTIAL, 2 buffer 1/4 màn hình trong SRAM ngoài (hiện tại, commit `ecde361`)**.
  - Tiêu chí: RAM cần dùng, số lần flush mỗi khung hình, vẽ và DMA có chạy song song được không, tốc độ truy cập bộ nhớ, **FPS đo được**.
- Bảng 7.3 – Lọc tín hiệu cảm ứng: không lọc · chỉ chống dội · **chống dội + EMA (chọn)** · lọc trung vị.

**Hình vẽ**
- *Hình 7.1 – Sơ đồ khối lớp porting* (draw.io). LVGL core đặt ở giữa.
  - Bên trái (Input): XPT2046 → `touch.c` → `my_touchpad_read` → `lv_indev`.
  - Bên phải (Display): `lv_display` → `draw_buf1/2` → `my_disp_flush` → DMA2 Stream0 → FSMC → GRAM của ILI9341. Vẽ đường quay ngược: ngắt DMA TC → `lv_display_flush_ready`.
  - Phía trên: tick (`HAL_GetTick`), OS (CMSIS-RTOS2), bộ nhớ (malloc → pvPortMalloc → SRAM ngoài).
- *Hình 7.2 – Giản đồ thời gian double buffering* (WaveDrom hoặc biểu đồ Gantt).
  - Hai hàng "CPU (vẽ)" và "DMA (flush)". CPU vẽ vùng 1 vào buf1; DMA flush buf1 **trong khi** CPU vẽ vùng 2 vào buf2; CPU chờ `flush_ready` rồi mới dùng lại buf1.
  - Vẽ thêm một hình nhỏ cho trường hợp **chỉ có 1 buffer** để thấy CPU phải ngồi chờ DMA.
  - Ghi thời gian vẽ và thời gian flush đo được ở Chương 10.

**Tham chiếu mã nguồn**: [gui_app.c](../App/gui_app.c), [lv_conf.h](../Middlewares/Third_Party/lv_conf.h), [lcd.c](../Drivers/LCD/lcd.c), [touch.c](../Drivers/Touch/touch.c).

### 7.3 Kiến trúc màn hình

**Nội dung cần viết**
- Có 10 màn hình, mỗi màn hình là một module `scr_xxx.c/.h`. Mỗi module cung cấp hàm `gui_load_xxx(...)` và các hàm cập nhật (`gui_update_master_lobby_count`, `gui_slave_add_host_to_list`, …). File `gui_app.h` include tất cả header.
- **Mẫu "màn hình bền vững"** (persistent screen):
  - Lần đầu thì tạo đối tượng (khi con trỏ còn `NULL`); các lần sau chỉ cập nhật nội dung.
  - Chuyển màn bằng `lv_scr_load_anim(..., auto_del = false)`.
  - Dừng animation trong sự kiện `LV_EVENT_SCREEN_UNLOADED` để callback cũ không chạy muộn.
- **Thành phần dùng chung**: các style trong `ui_styles.c`, `create_home_btn`, `create_ranking_list`/`update_ranking_list`.

**So sánh – biện luận** (Bảng 7.4): tạo và hủy màn hình mỗi lần chuyển (cách làm cũ) · **màn hình bền vững (chọn, commit `5aba46f`)**. Tiêu chí: thời gian chuyển màn, phân mảnh heap, RAM chiếm thường trực, độ phức tạp khi phải dọn trạng thái cũ (ví dụ animation còn chạy).

Bảng 7.5 – Danh sách màn hình:

| Màn hình | Vai trò | Widget chính | Dữ liệu vào | Sự kiện ra |
|---|---|---|---|---|
| role_selection | Chung | 2 nút | – | `game_set_role` |
| master_lobby | Host | Nhãn "Players: n", nút Start | Số người chơi | `game_master_start_quiz` |
| master_question | Host | Nhãn câu hỏi, thanh thời gian, 4 nút màu | Câu hỏi, thời lượng | Animation kết thúc → `game_master_timer_timeout` |
| master_leaderboard | Host | Danh sách xếp hạng, nút Next | Bảng xếp hạng | `game_master_next_question` |
| master_congrats | Host | "Congratulations!", "Final Ranking" | Bảng xếp hạng | Home |
| slave_scan | Player | Nút Refresh, `lv_list` các phòng | Tên phòng | `game_slave_scan_hosts`, `game_slave_join_host` |
| slave_waiting | Player | "Waiting for Host..." | – | Home |
| slave_answer | Player | Câu hỏi, `P<id>`, thanh thời gian, 4 nút có nhãn chữ chạy vòng | Câu hỏi, đáp án, thời lượng, ID | `game_slave_submit_answer` |
| slave_feedback | Player | Nền xanh hoặc đỏ, "Correct!/Incorrect!", "Score" | Đúng/sai, điểm | Home |
| slave_timeup | Player | "Time's up!", "No answer counted" | – | Home |

**Hình vẽ**
- *Hình 7.3 – Sơ đồ điều hướng màn hình* (draw.io).
  - Mỗi màn hình là một ảnh thu nhỏ (ảnh chụp thật hoặc wireframe 240×320).
  - Nhánh Host đặt phía trên, nhánh Player đặt phía dưới.
  - Mũi tên có nhãn sự kiện: **nét liền** cho thao tác chạm, **nét đứt** cho lệnh từ mạng (ví dụ `CMD_BROADCAST_QUESTION`).
  - Nút Home: chỉ vẽ **một** mũi tên chung quay về role_selection.
- *Hình 7.4 – Lưới ảnh chụp LCD thật*: 2 hàng × 5 ảnh, mỗi ảnh có chú thích.

### 7.4 Thiết kế trải nghiệm người dùng

**Nội dung cần viết**
- **Phong cách Kahoot**:
  - 4 màu đáp án: đỏ `#E21B3C`, xanh dương `#1368CE`, vàng `#D89E00`, xanh lá `#26890C`.
  - Lưới 2×2 nút, mỗi nút 90×70 px.
  - Thanh đếm ngược; phản hồi bằng màu nền toàn màn hình; nhãn đáp án dài tự chạy chữ vòng; hiện `P<id>`; nút Home 40×40.
  - Bố cục cho màn 240×320 đặt dọc.
- **Hạn chế**: chỉ có một cỡ chữ (14) và không có dấu tiếng Việt.
- **Cải tiến**:
  - Tạo font tiếng Việt bằng `lv_font_conv` (ước lượng thêm bao nhiêu Flash).
  - Thêm âm thanh bằng buzzer.
  - Hiện hạng cá nhân trên màn hình của Player.

### 7.5 LVGL trong môi trường đa nhiệm

**Nội dung cần viết**
- LVGL **không thread-safe**. Vì vậy chỉ GUI task được gọi `lv_timer_handler`, và task khác muốn gọi API LVGL phải bọc lời gọi bằng `app_lv_lock()`/`app_lv_unlock()`.
- Callback sự kiện của LVGL chạy trong lúc GUI task **đang giữ khóa**, nên:
  - không được lock lần nữa (mutex không đệ quy);
  - không nên block lâu. Lưu ý `comm_send_cmd` gửi UART dạng blocking, mất tới khoảng 18 ms.
- Chi tiết được trình bày ở mục 9.2.

---

## CHƯƠNG 8. NGÂN HÀNG CÂU HỎI

### 8.1 Yêu cầu

**Nội dung cần viết**
- Người soạn câu hỏi không phải sửa mã C.
- Không phải phân tích cú pháp dữ liệu lúc chạy; dữ liệu nằm trong Flash.
- Kiểm soát được kích thước dữ liệu.
- Tự động cập nhật mỗi khi build.

### 8.2 Cấu trúc dữ liệu

**Nội dung cần viết**
- **Định dạng nguồn là JSON**: một mảng các đối tượng `{id, question, options[4], answerIndex (0..3), timeLimitSec}`. Trích một phần tử làm ví dụ.
- **Cấu trúc C `Question_t`**: `uint16 id`, `char question[128]`, `char options[4][64]`, `uint8 answerIndex`, `uint8 timeLimitSec`.
  - Mỗi câu chiếm **388 B**.
  - Mảng được khai báo `const`, nên nằm trong `.rodata` ở Flash; truy cập theo chỉ số với độ phức tạp O(1); đi kèm biến `question_bank_size`.
- **Ánh xạ màu**: chỉ số 0..3 tương ứng Đỏ, Xanh dương, Vàng, Xanh lá.
- **Biểu diễn khi truyền khác biểu diễn khi lưu**: khi truyền, các chuỗi được nối liền và kết thúc bằng `\0`, nên chỉ tốn đúng độ dài thực. `answerIndex` chỉ nằm ở Host.
- **Dung lượng**: N câu × 388 B. Ví dụ 100 câu khoảng 38 KB, bằng khoảng 3,7% Flash.

**So sánh – biện luận**
- Bảng 8.1 – Cách lưu chuỗi: **mảng ký tự kích thước cố định (chọn)** · `const char*` trỏ tới chuỗi · blob nén kèm bảng offset. Tiêu chí: lãng phí Flash, độ đơn giản khi sinh mã, an toàn độ dài, tốc độ truy cập.
- Bảng 8.2 – Nơi lưu câu hỏi và thời điểm nạp: viết cứng trong C · **JSON rồi sinh mã lúc build (chọn)** · parse JSON lúc chạy (cJSON) · thẻ SD + FatFS · EEPROM · tải từ PC qua RS232 lúc chạy. Tiêu chí: có cần thêm phần cứng không, RAM lúc chạy, đổi câu hỏi có cần nạp lại firmware không, khả năng bắt lỗi dữ liệu, độ phức tạp.

**Hình vẽ**
- *Hình 8.1 – Bố cục bộ nhớ của `Question_t`* (draw.io). Vẽ một thanh ngang có độ dài tỷ lệ với số byte:
  - offset 0: `id` (2 B); offset 2: `question` (128 B); offset 130: `options` (4×64 B); offset 386: `answerIndex`; offset 387: `timeLimitSec`; tổng 388 B.
  - Bên cạnh vẽ mảng `question_bank[N]` nằm trong Flash.
- *Hình 8.2 – Biến đổi dữ liệu qua 3 bước* (draw.io): đối tượng JSON → struct `Question_t` trong Flash → payload `BROADCAST_QUESTION` `[duration u32 | Q\0 | A\0 | B\0 | C\0 | D\0]` (≤ 200 B). Ở bước thứ ba, gạch bỏ `answerIndex` và chú thích "không gửi đi".

### 8.3 Đoạn mã sinh code C cho ngân hàng câu hỏi

**Nội dung cần viết**
- **Script `scripts/generate_questions.py`**:
  - Đọc JSON (UTF-8), sinh file header (typedef, `extern`, các macro `MAX_*`) và file source (mảng khởi tạo bằng designated initializer).
  - Thoát ký tự `"`; điền cho đủ 4 đáp án bằng chuỗi `""`; nếu thiếu `timeLimitSec` thì mặc định là 15.
  - Hàm `write_if_changed` chỉ ghi file khi nội dung thay đổi, nhờ vậy không làm file bị biên dịch lại thừa.
- **Tích hợp CMake**: `add_custom_command` với OUTPUT là `questions_data.c/.h` và DEPENDS là `questions.json` cùng chính script. Sửa JSON thì lần build sau file C tự được sinh lại. `README.md` hướng dẫn chạy script bằng tay.
- **Trích mã**: vòng lặp sinh từng phần tử (không quá 20 dòng) và đoạn C được sinh ra tương ứng.
- **Quy trình cho người soạn câu hỏi**: sửa JSON → build → nạp.

**Hình vẽ**
- *Hình 8.3 – Flowchart của script* (draw.io): Bắt đầu → kiểm tra số đối số → đọc JSON (lỗi thì `exit 1`) → tạo chuỗi header → với mỗi câu hỏi: thoát ký tự, bổ sung đáp án thiếu, gán giá trị mặc định → ghép vào source → so sánh với file cũ → ghi file nếu khác → in "Successfully generated…".
- *Hình 8.4 – Luồng build tự động* (sequence, PlantUML): Developer sửa `questions.json` → `cmake --build` → Ninja thấy file DEPENDS mới hơn file OUTPUT → chạy Python → gcc biên dịch `questions_data.c` → link → `.elf`.

**Tham chiếu mã nguồn**: [generate_questions.py](../scripts/generate_questions.py), [questions.json](../App/questions.json), [questions_data.h](../App/Logic/questions_data.h), [CMakeLists.txt](../CMakeLists.txt), [README.md](../README.md).

### 8.4 Kiểm tra dữ liệu và hạn chế

**Nội dung cần viết**: các trường hợp **hiện chưa được kiểm tra**, và hậu quả của từng trường hợp:
- **Câu hỏi dài đúng 128 ký tự** (hoặc đáp án dài đúng 64 ký tự): ngôn ngữ C cho phép khởi tạo mảng mà **không có `\0`** ở cuối, nên `strlen` sẽ đọc tràn sang trường kế tiếp. Chuỗi dài hơn nữa chỉ bị trình biên dịch cảnh báo rồi cắt bớt.
- Tổng payload lớn hơn 196 B: bị cắt khi gửi đi.
- `answerIndex` nằm ngoài khoảng 0..3.
- `timeLimitSec` lớn hơn 255 (kiểu `uint8`).
- `id` bị trùng.
- Ký tự không phải ASCII (font không hiển thị được).
- Ký tự `\` và ký tự xuống dòng chưa được thoát.
- **Đề xuất**: kiểm tra dữ liệu ngay trong script và làm build thất bại khi có lỗi; dùng JSON Schema; xáo trộn thứ tự câu hỏi và đáp án; hỗ trợ nhiều bộ đề.

---

## CHƯƠNG 9. TRIỂN KHAI PHẦN MỀM VÀ QUẢN LÝ TÀI NGUYÊN NHÚNG

### 9.1 Kiến trúc tác vụ FreeRTOS

**Nội dung cần viết**
- **Cấu hình FreeRTOS**:
  - Lập lịch preemptive, tick 1 kHz, 56 mức ưu tiên (theo CMSIS-RTOS2), heap_5.
  - `configUSE_NEWLIB_REENTRANT = 1` cùng file `newlib_lock_glue.c`, để `vsnprintf` và `malloc` an toàn khi nhiều task gọi đồng thời.
  - Bật `configUSE_TRACE_FACILITY` để dùng được `uxTaskGetSystemState`.

Bảng 9.1 – Các task trên STM32:

| Task | Hàm | Ưu tiên | Stack | Vị trí stack | Cách kích hoạt | Nhiệm vụ |
|---|---|---|---|---|---|---|
| GUI_Task | `StartTask_LVGL` | Realtime (48) | 8 KB (2048 word), cấp phát tĩnh | SRAM trong (`.bss`) | Chu kỳ khoảng 5 ms | Porting LVGL, `lv_timer_handler`, xử lý sự kiện UI |
| Game_Logic_Task | `StartTask_GAME` | Normal (24) | 2 KB | Heap (SRAM ngoài) | Chu kỳ 100 ms, mỗi lần lấy hết commQueue | FSM trò chơi, chấm điểm, kết thúc câu |
| Communication_T | `StartTask_COMM` | Normal | 2 KB | Heap | Khởi tạo parser, bật ngắt RX, sau đó chỉ ngủ 1 s mỗi vòng | Gần như rảnh |
| CommandParser | `StartTask_CommandParser` | Normal | 2 KB | Heap | Block chờ `uartRxQueue` | Console debug |
| ledBlinkingTask | `StartTask_LedBlinking` | Normal | 512 B | Heap | Chu kỳ 1 s | Nhịp tim hệ thống |

- **Bảng 9.2 – Các ngắt**:
  - USART2 RX: parser → pool → `commQueue`.
  - DMA2 Stream0 TC: `flush_ready`.
  - USART1 DMA TX TC: nhả semaphore.
  - USART1 RX event/IDLE: đẩy vị trí vào `uartRxQueue`.
  - TIM2: tick của HAL.
  - Mức ưu tiên NVIC là 5, không nhỏ hơn `configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY`, nên được phép gọi API FreeRTOS dạng FromISR.
- **Bảng 9.3 – Các task trên ESP8266** (lấy từ mục 4.5).
- **Biện luận về ưu tiên**:
  - GUI được đặt ưu tiên cao nhất để animation mượt và phản hồi chạm nhanh. GUI tự nhường CPU mỗi vòng (`osDelay(5)`), nên các task Normal vẫn được chạy.
  - Rủi ro: khung hình nặng có thể làm Game task chạy trễ.
  - Cần thảo luận phương án ngược lại (GUI ưu tiên thấp nhất, giao tiếp ưu tiên cao nhất, đây là khuyến nghị phổ biến) và **đo đạc** để quyết định.
- **Stack của GUI** được cấp phát tĩnh trong SRAM trong, vì bộ vẽ LVGL dùng stack nhiều và truy cập liên tục (commit *"LCD task uses internal SRAM"* và *"reduce the stack size of the gui task"*). Định cỡ stack dựa trên high-water mark (lệnh `1`).
- **Game task polling 100 ms thay vì block trên queue**, vì còn phải kiểm tra cờ timer. Phương án tốt hơn: dùng task notification hoặc event flags (`osThreadFlags`) để vừa block vừa nhận được nhiều nguồn sự kiện, và giảm độ trễ (hiện có thể trễ tới 100 ms).
- **Communication_T gần như rảnh**: đề xuất cho task này làm chủ sở hữu duy nhất của đường TX USART2 (xem mục 9.2b).

**So sánh – biện luận**
- Bảng 9.4 – Mô hình thực thi: super-loop + ngắt · **FreeRTOS (chọn)** · Zephyr/RT-Thread. Tiêu chí: độ phức tạp khi phải chạy song song GUI, giao tiếp và console; hỗ trợ trong CubeMX; tài liệu; kích thước bộ nhớ chiếm dụng; mục tiêu học tập.
- Bảng 9.5 – API: **CMSIS-RTOS v2 (chọn**: CubeMX sinh sẵn, LVGL có sẵn `LV_OS_CMSIS_RTOS2`, dễ chuyển sang RTOS khác) · API FreeRTOS gốc (đầy đủ hơn, có các hàm FromISR và stream buffer).

**Hình vẽ**
- *Hình 9.1 – Sơ đồ task và IPC* (draw.io).
  - Task: hình chữ nhật bo góc, ghi tên, ưu tiên và stack.
  - ISR: hình chữ nhật đỏ có biểu tượng tia sét.
  - Queue: hình ống (`commQueue` 10 con trỏ, `uartRxQueue` 3 × u16).
  - `lvgl_mutex`: ổ khóa, nối nét đứt tới GUI_Task và Game task.
  - `uart_tx_sem`: nối tới `debug_log` (mọi task gọi) và tới ISR USART1 TC.
  - Ngoại vi (USART2, USART1, DMA2, LCD) đặt ở rìa sơ đồ.
  - Chiều mũi tên là chiều đi của dữ liệu.
- *Hình 9.2 – Giản đồ thời gian thực thi* (khoảng 200 ms trên trục thời gian).
  - Các hàng: GUI (chạy thành từng đợt mỗi 5 ms, đợt dài hơn khi phải vẽ), Game (mỗi 100 ms), ISR USART2 (vạch ngắn cho mỗi byte khi có khung đến), DMA flush.
  - Vẽ từ số liệu đo: bật/tắt GPIO ở đầu và cuối mỗi đoạn rồi đo bằng logic analyzer, hoặc dùng SystemView/Tracealyzer nếu có.

**Tham chiếu mã nguồn**: [freertos.c](../Core/Src/freertos.c), [FreeRTOSConfig.h](../Core/Inc/FreeRTOSConfig.h), [game_logic.c](../App/Logic/game_logic.c) (`StartTask_COMM`, `StartTask_GAME`), [gui_app.c](../App/gui_app.c).

### 9.2 Cơ chế xử lý thông điệp và an toàn luồng

**Nội dung cần viết**

**(a) Đường nhận dữ liệu từ ESP8266**
- USART2 ngắt sau mỗi byte → `HAL_UART_RxCpltCallback` → `protocol_parse_byte` ghi thẳng vào `msg_pool[pool_idx]` (mảng tĩnh 10 phần tử).
- Khi đủ một khung, ISR đẩy **con trỏ** tới phần tử đó vào `commQueue` (10 × 4 B), tăng `pool_idx` theo vòng, rồi bật lại `HAL_UART_Receive_IT`.
- Game task **lấy hết** các message có trong `commQueue` mỗi chu kỳ, vì câu trả lời thường đến dồn cùng lúc.
- **Bài học**: phiên bản đầu gọi `pvPortMalloc` ngay trong ISR và gây HardFault (commit `9937657`), vì heap của FreeRTOS không an toàn khi gọi từ ISR. Cách sửa là chuyển sang pool tĩnh và chỉ truyền con trỏ (zero-copy).
- **Phân tích giới hạn**: kích thước pool **bằng** độ sâu queue (cùng là 10).
  - Khi queue đầy, khung mới vẫn được parser ghi vào phần tử cũ nhất *vẫn còn nằm trong queue*.
  - Nếu 10 khung đến trong lúc Game task đang xử lý một phần tử, phần tử đó cũng có thể bị ghi đè.
  - Điều kiện an toàn: số phần tử của pool ≥ độ sâu queue + 1 + số phần tử consumer đang giữ. Cách khác: dùng queue sao chép theo giá trị, hoặc dùng free-list.

**(b) Đường gửi dữ liệu tới ESP8266**
- `comm_send_cmd` tạo khung trên stack → ghi log bằng `debug_log` → gửi bằng `HAL_UART_Transmit` dạng **blocking** (timeout 100 ms; khung 204 B mất khoảng 17,7 ms).
- Hàm này được gọi từ **hai ngữ cảnh**:
  - GUI task: trong các callback UI như start, next, scan, join, submit.
  - Game task: khi gửi feedback, host_found, join_ack, time_up.
- Vì vậy hai task có thể dùng `huart2` cùng lúc. Khi đó HAL trả `HAL_BUSY` cho lời gọi đến sau và **khung đó bị mất mà không có thông báo**.
- Cần phân tích khả năng xảy ra (thấp nhưng có thật) và đề xuất cách sửa: thêm mutex cho TX, hoặc dùng hàng đợi TX với Communication_T là chủ sở hữu duy nhất của đường gửi, kết hợp DMA.

**(c) Khóa LVGL (`lvgl_mutex`)**
- **Quy tắc**: GUI task giữ khóa trong suốt `lv_timer_handler`; task khác gọi `gui_*` thì phải bọc bằng `app_lv_lock`/`app_lv_unlock`; callback UI (đang giữ khóa) được phép gọi `game_*`, nên `game_*` **không được** lock thêm lần nữa.
- Khóa này **kiêm luôn vai trò "khóa trạng thái trò chơi"** cho các chuyển trạng thái quan trọng (`master_end_question` kiểm tra `current_state` khi đang giữ khóa).
  - Được: chỉ có một khóa thô nên không có vấn đề thứ tự khóa, tránh được deadlock.
  - Mất: giảm khả năng chạy song song.
- **Dùng mutex (có kế thừa ưu tiên) thay vì semaphore nhị phân**, để tránh đảo ngược ưu tiên giữa GUI (Realtime) và Game (Normal).
- **Cờ `volatile master_timer_expired`**: chuyển sự kiện từ GUI task sang Game task mà không cần khóa, vì việc ghi một word trên Cortex-M4 là nguyên tử.

**(d) Log debug**: semaphore nhị phân `uart_tx_sem` kết hợp DMA TX; newlib được cấu hình reentrant.

**(e) Phía ESP8266**: hàng đợi chứa con trỏ, cấp phát bằng `malloc`/`free` trong ngữ cảnh task (không phải ISR); các callback Wi-Fi được giữ ngắn.

**So sánh – biện luận**
- Bảng 9.6 – Chuyển message từ ISR sang task:
  - Queue sao chép theo giá trị: mỗi phần tử 202 B, phải chép trong ISR.
  - Con trỏ + `malloc` trong ISR: **gây lỗi** (HardFault).
  - **Con trỏ + pool tĩnh xoay vòng (chọn)**.
  - Stream buffer/ring buffer theo byte, parse trong task: ISR ngắn nhất.
  - DMA + IDLE, parse trong task.
  - Tiêu chí: an toàn khi gọi từ ISR, chi phí sao chép, RAM, độ phức tạp, nguy cơ mất dữ liệu.
- Bảng 9.7 – Cơ chế đồng bộ: mutex · semaphore nhị phân · critical section/tắt ngắt · task notification/event flags · biến `volatile`. Với mỗi cơ chế, ghi khi nào nên dùng và **dự án đang dùng nó ở đâu**.

**Hình vẽ**
- *Hình 9.3 – Luồng dữ liệu của đường nhận* (draw.io). Chân RX của USART2 → (ISR: `parse_byte`) → `msg_pool[0..9]` vẽ thành dãy ô, có mũi tên chỉ `pool_idx` → (đẩy con trỏ) → `commQueue` → (Game task: lấy ra, `switch`) → `gui_*` (qua mutex) hoặc `comm_send_cmd` → chân TX của USART2. Tô **hai màu nền** để phân biệt vùng "ngữ cảnh ISR" và vùng "ngữ cảnh task".
- *Hình 9.4 – Trước và sau khi sửa HardFault* (hai sequence diagram đặt cạnh nhau): bên trái là cấp phát trong ISR dẫn tới HardFault, bên phải là dùng pool tĩnh. Dùng hình này trong mục bài học.
- *Hình 9.5 – Ma trận sở hữu tài nguyên* (vẽ dạng bảng).
  - Hàng: các tài nguyên dùng chung (đối tượng LVGL, trạng thái trò chơi, `huart2` TX, `huart1` TX, `msg_pool`, heap).
  - Cột: các ngữ cảnh truy cập (GUI task, Game task, CommandParser, ISR USART2, ISR USART1/DMA).
  - Mỗi ô ghi R/W và cơ chế bảo vệ (mutex, semaphore, hoặc không có). Tô đỏ ô "`huart2` TX: GUI ghi, Game ghi, **không có bảo vệ**".

### 9.3 Mở rộng bộ nhớ bằng SRAM ngoài

**Nội dung cần viết**
- **Nhu cầu bộ nhớ**: lập bảng ước tính. 2 draw buffer (76 800 B), cộng đối tượng và style LVGL của 10 màn hình bền vững, cộng stack các task và đối tượng RTOS, vượt quá phần còn lại của 128 KB SRAM trong (sau khi trừ `.data`/`.bss`, 8 KB stack GUI và stack MSP).
- **Bản đồ bộ nhớ**:

  | Vùng | Địa chỉ | Dung lượng | Nội dung |
  |---|---|---|---|
  | Flash | `0x08000000` | 1 MB | Code, `.rodata` (`question_bank`, font) |
  | SRAM | `0x20000000` | 128 KB | `.data`/`.bss`, stack GUI, `msg_pool`, stack MSP/ISR |
  | CCM | `0x10000000` | 64 KB | Hiện chưa dùng; chỉ CPU truy cập được, DMA không truy cập được |
  | SRAM ngoài | `0x68000000` | 512 KB | Toàn bộ heap của FreeRTOS |

- **Linker script**: thêm vùng `EXTSRAM (xrw)` và section `.ext_ram_data (NOLOAD)`. Vì là NOLOAD, section này không được chép từ Flash và startup code không xóa về 0. Mảng `ucExternalHeap[512 KB]` được đặt vào section này bằng `__attribute__((section(".ext_ram_data")))`.
- **heap_5**: phải gọi `vPortDefineHeapRegions(xHeapRegions)` *trước khi* tạo bất kỳ đối tượng RTOS nào. Lưu ý: với heap_5, `configTOTAL_HEAP_SIZE` (120000) **không còn tác dụng**.
- **Chuyển hướng `malloc` của thư viện C**:
  - Dùng cờ linker `-Wl,--wrap=malloc/free/realloc/calloc` để các lời gọi đi vào `__wrap_*`, và các hàm này gọi `pvPortMalloc`.
  - Mỗi khối có **header 8 B** lưu kích thước, để `realloc` biết cần chép bao nhiêu byte. Chọn 8 B (không phải 4 B) để giữ căn lề 8 byte theo ABI.
  - Kết quả: LVGL (`LV_STDLIB_CLIB`) và newlib dùng chung **một heap có thống kê** (`xPortGetFreeHeapSize`, heap trống nhỏ nhất).
- **Tự kiểm tra trước khi dùng**:
  - Hai lượt ghi rồi đọc lại toàn bộ 512 KB theo từng word 16-bit.
  - Lượt 1 dùng mẫu theo địa chỉ, để phát hiện chân địa chỉ bị hở hoặc chập. Lượt 2 dùng mẫu đảo bit, để phát hiện chân dữ liệu bị kẹt.
  - Kết quả PASS/FAIL, kèm địa chỉ lỗi nếu có, được gửi qua USART1 dạng blocking, vì lúc này RTOS và DMA chưa chạy.
  - Nguồn gốc: lỗi thiếu 2 chân địa chỉ trong lúc đưa phần cứng vào hoạt động (mục 2.4).
- **Đánh đổi hiệu năng**:
  - Truy cập SRAM ngoài tốn nhiều chu kỳ FSMC hơn truy cập SRAM trong (theo timing đã cấu hình).
  - SRAM ngoài **dùng chung bus FSMC với LCD**, nên khi DMA flush (đọc SRAM ngoài, ghi LCD) hai việc phải lần lượt chiếm bus.
  - Stack của task nằm trong SRAM ngoài làm chuyển ngữ cảnh chậm hơn.
  - Cần đo và so sánh các điểm này ở Chương 10.
- **Lịch sử quyết định vị trí draw buffer** (trình bày lý do của từng bước, kèm số đo):
  1. Cấp phát động từ heap.
  2. Buffer tĩnh 1/10 màn hình trong SRAM trong (`13bb872`).
  3. 2 buffer, mỗi buffer 1/4 màn hình, trong SRAM ngoài (`ecde361`).

Bảng 9.8 – Phân bố bộ nhớ (cột "Kích thước" ghi số đo từ file `.map` và lệnh `1`):

| Đối tượng | Kích thước | Vùng nhớ | Lý do |
|---|---|---|---|
| `draw_buf1`, `draw_buf2` | 2 × 38 400 B | SRAM ngoài (heap) | Kích thước lớn; DMA đọc được qua FSMC |
| Stack GUI_Task | 8 192 B | SRAM trong (`.bss`, tĩnh) | Truy cập dày đặc |
| Stack và TCB các task còn lại | khoảng 6,5 KB | SRAM ngoài | Không quan trọng về tốc độ |
| Đối tượng LVGL | *đo thực tế* | SRAM ngoài | Cấp phát qua `malloc` đã bọc |
| `msg_pool` | 10 × 202 B | SRAM trong (`.bss`) | ISR ghi trực tiếp |
| `question_bank` | N × 388 B | Flash | Hằng số |
| Buffer RS232 (`rx_buffer` 128 B, `tx_buf` 256 B) | 384 B | SRAM trong | Dùng cho DMA |

**So sánh – biện luận** (Bảng 9.9 – Chiến lược heap):
- heap_4 chỉ trong SRAM trong (khoảng 120 KB): không đủ.
- **heap_5 chỉ dùng SRAM ngoài (chọn)**.
- heap_5 nhiều vùng (SRAM trong và SRAM ngoài, ưu tiên vùng trong).
- Dùng CCM cho stack hoặc heap: nhanh, nhưng DMA không truy cập được.
- Cho LVGL một pool riêng (`LV_STDLIB_BUILTIN`), tách khỏi heap của RTOS.
- Tiêu chí: dung lượng, tốc độ, DMA có truy cập được không, phân mảnh, khả năng giám sát.

**Hình vẽ**
- *Hình 9.6 – Bản đồ bộ nhớ* (draw.io).
  - 4 cột dọc (Flash, SRAM, CCM, EXTSRAM), địa chỉ tăng dần từ dưới lên, chiều cao tỷ lệ gần đúng với dung lượng.
  - Mỗi cột chia thành các section: `.isr_vector`, `.text`, `.rodata` (`question_bank`, font), `.data`, `.bss` (stack GUI, `msg_pool`), `._user_heap_stack`/MSP, CCM (để trống), `.ext_ram_data`.
  - Bên trong `.ext_ram_data` vẽ `ucExternalHeap` 512 KB, chia tiếp thành `draw_buf1`, `draw_buf2`, đối tượng LVGL, stack task, đối tượng RTOS và vùng trống.
  - Ghi địa chỉ đầu và cuối mỗi section. Lấy số liệu từ file `.map` và lệnh `arm-none-eabi-size`.
- *Hình 9.7 – Chuỗi cấp phát bộ nhớ* (draw.io): `lv_malloc` → `malloc` → (linker `--wrap`) → `__wrap_malloc` → `pvPortMalloc` (heap_5) → `ucExternalHeap @0x68000000` → FSMC NE3 → chip SRAM. Vẽ một hình phóng to nhỏ cho header 8 B đứng trước con trỏ trả về.
- *Hình 9.8 – Flowchart self-test hai lượt* (draw.io).

**Tham chiếu mã nguồn**: [main.c](../Core/Src/main.c) (`ExtSRAM_SelfTest`, `xHeapRegions`), [STM32F407XX_FLASH.ld](../STM32F407XX_FLASH.ld), [freertos.c](../Core/Src/freertos.c) (`__wrap_*`), [CMakeLists.txt](../CMakeLists.txt) (`target_link_options`).

---

## CHƯƠNG 10. THỰC NGHIỆM VÀ ĐÁNH GIÁ KẾT QUẢ

### 10.1 Môi trường thực nghiệm

**Nội dung cần viết**
- Số KIT (ít nhất 3: 1 Host và 2 Player), phiên bản firmware (ghi commit hash), cách bố trí, khoảng cách, nguồn cấp.
- Công cụ: terminal RS232 (PuTTY hoặc minicom), logic analyzer (nếu có), camera quay chậm, đồng hồ bấm giờ.
- **Phương pháp đo thời gian**:
  - Đo round-trip trên **cùng một board** bằng `HAL_GetTick` ghi trong log. Ví dụ: Player chạm đáp án lúc t₀, nhận `SEND_FEEDBACK` lúc t₁.
  - Hoặc bật/tắt GPIO trên hai board và đo độ trễ một chiều bằng logic analyzer.
  - Đo chu kỳ CPU bằng `DWT->CYCCNT`.

**Hình vẽ**
- *Hình 10.1*: ảnh bố trí thí nghiệm, kèm sơ đồ mặt bằng (vị trí các KIT, khoảng cách, vật cản).

### 10.2 Kịch bản thử nghiệm vận hành hệ thống

Bảng 10.1 – Test case. Các cột: *Mã · Mục tiêu · Các bước · Kết quả mong đợi · Kết quả thực tế · Minh chứng (ảnh hoặc log)*.

| Mã | Kịch bản |
|---|---|
| TC-01 | Khởi động: self-test SRAM báo PASS, log `ESP_READY` xuất hiện |
| TC-02 | Console RS232: lệnh `h` và `1`; ghi lại heap trống, heap trống nhỏ nhất và high-water mark của từng task |
| TC-03 | Cảm ứng: độ chính xác khi chạm 4 góc và tâm màn hình; chống dội (không bị nhận nhấn hai lần) |
| TC-04 | Log của ESP8266 hiện ra trên cổng RS232 của STM32 |
| TC-05 | Quét phòng khi có 0, 1 và 2 Host |
| TC-06 | Vào phòng và nhận ID: P1..Pn đúng theo thứ tự vào |
| TC-07 | Một vòng đầy đủ, mọi người trả lời, câu hỏi kết thúc sớm |
| TC-08 | Hết giờ khi có người chưa trả lời: người đó thấy "Time's up" |
| TC-09 | Bấm đáp án nhiều lần; đáp án đến trễ |
| TC-10 | Rút nguồn một Player giữa chừng: `PLAYER_DROPPED` xuất hiện sau 3 lần gửi lỗi, Host không bị treo |
| TC-11 | Player vào lại: giữ nguyên ID và điểm |
| TC-12 | Host bấm Home giữa trận; Player bấm Home rồi nhận câu hỏi tiếp theo |
| TC-13 | Câu hỏi và đáp án dài (gần 127 và 63 ký tự): kiểm tra xuống dòng và chữ chạy |
| TC-14 | Chơi hết bộ đề nhiều lần liên tục (≥ 30 phút): heap trống nhỏ nhất không giảm dần (không rò rỉ bộ nhớ) |
| TC-15 | Cự ly và vật cản: 1, 5, 10, 20 m và qua tường |
| TC-16 | Tải: số Player tối đa có thể thử (theo số KIT hiện có) |
| TC-17 | Sửa `questions.json` → build → câu hỏi mới hiện đúng |

### 10.3 Đo đạc định lượng

Bảng 10.2 – Các chỉ số. Các cột: *Chỉ số · Cách đo · Kết quả · Chỉ tiêu (từ mục 1.3)*.
- Độ trễ phát câu hỏi (Host bấm Start/Next → Player thấy câu hỏi) với N = 1, 2, 3… Player.
- Round-trip của một câu trả lời (Player chạm → hiện màn phản hồi).
- Tỷ lệ gửi unicast thành công theo khoảng cách (đếm số lần `send_cb` trả SUCCESS và FAIL; cần thêm bộ đếm và in ra log).
- Thời gian từ lúc Player mất kết nối đến lúc Host phát hiện.
- FPS và thời gian vẽ (bật `LV_USE_SYSMON` và `LV_USE_PERF_MONITOR` trong `lv_conf.h`).
- Thời gian flush một buffer khi dùng DMA so với khi dùng CPU chép.
- FPS khi dùng buffer 1/10 trong SRAM trong so với 2 buffer 1/4 trong SRAM ngoài.
- Tốc độ `memcpy` 64 KB trên SRAM trong so với SRAM ngoài (đo bằng `DWT->CYCCNT`).
- Bộ nhớ: Flash và RAM tĩnh (`arm-none-eabi-size`), heap trống và heap trống nhỏ nhất, high-water mark của từng task.
- Tải CPU (bật `configGENERATE_RUN_TIME_STATS` nếu cần).

**Hình vẽ**
- *Hình 10.2*: box plot hoặc histogram độ trễ (ít nhất 50 mẫu mỗi cấu hình).
- *Hình 10.3*: đồ thị đường tỷ lệ gửi thành công theo khoảng cách.
- *Hình 10.4*: biểu đồ cột nhóm FPS theo từng cấu hình buffer.
- *Hình 10.5*: biểu đồ cột chồng, phân bố RAM theo từng vùng nhớ.
- *Hình 10.6*: dãy ảnh chụp các màn hình trong một lượt chơi thật.

### 10.4 Đánh giá kết quả đạt được

**Nội dung cần viết**
- Bảng đối chiếu: *Mục tiêu (MT-xx ở mục 1.2) · Kết quả · Đạt/Chưa đạt · Minh chứng*.
- **Ưu điểm**: một firmware cho mọi KIT; không cần hạ tầng mạng; phát hiện được người chơi rớt; giao diện mượt; dễ thay đổi bộ câu hỏi.
- **Hạn chế**, tổng hợp từ các chương:
  - Checksum XOR yếu.
  - `huart2` có thể bị hai task gửi cùng lúc.
  - Pool có kích thước bằng độ sâu queue.
  - Slot peer trên ESP không được dùng lại.
  - Player không nhận thông báo kết thúc trận.
  - Không có font tiếng Việt.
  - Điểm không tính theo tốc độ.
  - Phòng đầy thì không báo lại cho Player.
  - Các phòng trùng tên.
  - Game task có độ trễ polling 100 ms.
- **Bảng so sánh với Kahoot gốc**: có cần Internet không, thiết bị cần dùng, số người chơi, cách tính điểm, khả năng tùy biến câu hỏi, chi phí.

---

## CHƯƠNG 11. QUÁ TRÌNH HIỆN THỰC DỰ ÁN

### 11.1 Phân vai trong nhóm

| Thành viên | Vai trò chính | Phạm vi mã nguồn | Chương báo cáo phụ trách |
|---|---|---|---|
| **A** (nền tảng hệ thống, kiêm người tích hợp) | CubeMX/CMake, clock, FSMC (LCD và SRAM), DMA, console RS232, FreeRTOS, bộ nhớ (heap_5, bọc malloc, self-test), đo hiệu năng | `Core/`, `cmake/`, `CMakeLists.txt`, `Drivers/Rs232`, linker script | Ch.2, Ch.3, Ch.9; chủ trì Ch.11 và Ch.12 |
| **B** (truyền thông) | Firmware ESP8266, giao thức UART dùng chung, ESP-NOW (broadcast, unicast, ACK, phát hiện rớt), đường RX/TX phía STM32 (ISR, pool, queue, `comm_send_cmd`) | `esp8266/`, `App/Logic/protocol.*`, phần giao tiếp trong `game_logic.c` | Ch.4, Ch.6; chủ trì Ch.10 |
| **C** (ứng dụng và giao diện) | Porting LVGL (làm cùng A), driver cảm ứng và hiệu chuẩn, 10 màn hình và style, FSM trò chơi, ngân hàng câu hỏi và script sinh mã | `App/screens/`, `App/gui_app.*`, `Drivers/{LCD,Touch,EEPROM}`, `scripts/`, phần FSM trong `game_logic.c` | Ch.1, Ch.5, Ch.7, Ch.8 |

### 11.2 Kế hoạch 5 tuần

| Tuần | Mục tiêu của tuần | A | B | C | Mốc bàn giao |
|---|---|---|---|---|---|
| **1** | Dựng nền tảng và viết đặc tả | Tạo dự án CubeMX và chuyển sang CMake (preset, toolchain); clock 168 MHz; task LED nhịp tim; cấu hình FSMC cho LCD và SRAM ngoài; USART1 với `debug_log` qua DMA | Cài ESP8266_RTOS_SDK; nháy LED/PWM; echo UART; viết **đặc tả giao thức v1** (định dạng khung, bảng lệnh); viết `protocol.c/h` và test trên PC bằng gcc, gồm cả khung lỗi | Tìm hiểu LVGL 9; viết driver ILI9341, XPT2046 và AT24C (hiệu chuẩn); đưa LVGL vào CMake (cùng A); vẽ **wireframe 10 màn hình** và luồng chuyển màn | **M1**: LVGL hiển thị và nhận chạm trên KIT; log RS232 chạy; có đặc tả giao thức v1 |
| **2** | Hạ tầng hệ thống và liên kết hai chip | heap_5 trên SRAM ngoài (section linker, self-test); `--wrap malloc`; DMA mem→mem cho flush (cùng C); console lệnh và thống kê RTOS; định cỡ stack | ESP-NOW broadcast giữa 2 ESP8266; cầu nối UART↔ESP-NOW (task RX/TX, task ESP-NOW); `ESP_READY`, `ESP_LOG`; đường nhận USART2 phía STM32 (ISR, parser, queue) | Style chung; các màn role_selection, master_lobby, slave_scan, slave_waiting; lọc và chống dội cảm ứng; khung `game_logic` (vai trò, trạng thái, API `game_*`) | **M2**: STM32 và ESP8266 trao đổi được khung có checksum; chuyển được giữa các màn tĩnh; báo cáo bộ nhớ lần 1 |
| **3** | Chạy được trò chơi từ đầu đến cuối | Rà soát an toàn luồng (quy tắc `app_lv_lock`, mutex); hỗ trợ tìm nguyên nhân HardFault (thêm log trong fault handler); thử các kích thước và vị trí draw buffer, đo FPS | Luồng tìm và vào phòng (SCAN, HOST_FOUND, JOIN, chèn/bóc MAC); phát câu hỏi; nhận đáp án; **pool tĩnh cho ISR** | Màn master_question và slave_answer (có thanh thời gian); FSM của Host và Player; ngân hàng câu hỏi JSON, `generate_questions.py` và `add_custom_command` | **M3**: 1 Host và 1 Player chơi trọn một câu hỏi từ đầu đến cuối |
| **4** | Tăng độ tin cậy và hoàn thiện tính năng | Làm màn hình bền vững (hỗ trợ C, đo trước và sau); chuyển sang 2 draw buffer trong SRAM ngoài; đo heap và stack; cấu hình Release | Chuyển từ broadcast sang **unicast + ACK**; bảng peer và bộ đếm lỗi; `PLAYER_DROPPED`; Host cấp ID; vào lại giữ nguyên điểm; đo sơ bộ tỷ lệ gửi thành công | Chấm điểm và phản hồi; Leaderboard và Congrats (sắp xếp ổn định); màn Time's up; kết thúc câu sớm; xử lý nút Home; hoàn thiện UX; cập nhật bộ câu hỏi | **M4**: đủ tính năng, chạy được với ≥ 3 KIT; **đóng băng tính năng** |
| **5** | Thử nghiệm, sửa lỗi, viết báo cáo | Chạy TC-01, 02, 14; đo FPS và tốc độ SRAM; viết Ch.2, 3, 9; vẽ các hình 2.x, 3.x, 9.x | Chạy TC-04 đến 11, 15, 16 (giao thức, cự ly, độ trễ); viết Ch.4, 6; tổng hợp Ch.10 | Chạy TC-03, 12, 13, 17 (giao diện, câu hỏi); viết Ch.1, 5, 7, 8; quay video demo | **M5**: gắn tag firmware `v1.0`; nộp báo cáo, slide và video |

- **Cả nhóm trong tuần 5**: dành 2 ngày đầu để cùng tìm và sửa lỗi; viết Ch.11 và Ch.12; **đọc chéo** (A đọc chương của B, B đọc của C, C đọc của A); làm slide.
- **Nhịp phối hợp**: họp 15 phút vào đầu tuần để chốt việc. Cuối mỗi tuần có một buổi **tích hợp trên nhánh `main`** và demo đúng mốc tuần đó. Mỗi người làm trên nhánh tính năng riêng (ví dụ nhánh `esp8266` như trong lịch sử git), và nhánh chỉ được merge sau khi một người khác đã review.
- **Các điểm phụ thuộc cần canh**:
  - Đặc tả giao thức v1 (B, tuần 1) phải xong trước FSM (C, tuần 3).
  - FSMC và LCD (A, tuần 1) phải xong trước porting LVGL (C, tuần 1–2).
  - heap_5 (A, tuần 2) phải xong trước khi đưa draw buffer ra SRAM ngoài (tuần 4).
  - Đường nhận của STM32 (B, tuần 2) phải xong trước luồng vào phòng (tuần 3).

**Hình vẽ**
- *Hình 11.1 – Biểu đồ Gantt* (PlantUML `@startgantt`, hoặc Excel/draw.io).
  - Trục thời gian 5 tuần. Các thanh công việc gom theo từng người, mỗi người **một màu** (A xanh dương, B cam, C xanh lá).
  - Mốc M1..M5 vẽ bằng hình thoi ở cuối mỗi tuần.
  - Vẽ mũi tên cho các phụ thuộc nêu ở trên.
- *Hình 11.2 (tùy chọn)*: biểu đồ số commit theo tuần (lấy từ `git log --date=short`), để minh chứng tiến độ.

### 11.3 Khó khăn và bài học kinh nghiệm

**Nội dung cần viết**: lấy từ lịch sử git, mỗi mục viết theo ba ý *hiện tượng → nguyên nhân → cách khắc phục → bài học*:
1. **HardFault do cấp phát bộ nhớ trong ISR**: chuyển sang pool tĩnh (`9937657`).
2. **Thiếu 2 chân địa chỉ SRAM ngoài trong CubeMX**: LCD và SRAM chạy sai; viết thêm self-test.
3. **Bộ nhớ và stack của GUI task**: chuyển stack GUI sang cấp phát tĩnh trong SRAM trong; di chuyển draw buffer qua nhiều phương án.
4. **LCD giật khi chuyển màn**: dùng màn hình bền vững, DMA và 2 buffer.
5. **Cảm ứng bị nhận nhấn hai lần**: thêm chống dội và lọc EMA.
6. **Broadcast bị mất gói mà không biết**: chuyển sang unicast có ACK.
7. **UART0 của ESP8266 vừa dùng cho console vừa dùng nối STM32**: tắt console và chuyển log qua STM32.
8. **ESP8266 không khởi động**: phải đặt `ESP_POWER` ở mức cao ngay từ đầu.

---

## CHƯƠNG 12. KẾT LUẬN VÀ HƯỚNG PHÁT TRIỂN

**Nội dung cần viết**
- **Kết luận**: tóm tắt những gì đã đạt được theo từng mục tiêu ở mục 1.2, và những kỹ năng thu được theo mục 1.1.
- **Hướng phát triển** (xếp theo mức ưu tiên và ghi độ khó):
  1. Tính điểm theo thời gian trả lời; thêm lệnh `GAME_OVER` và gửi hạng tới từng Player.
  2. Font tiếng Việt.
  3. Kiểm tra dữ liệu ngân hàng câu hỏi; hỗ trợ nhiều bộ đề; nạp câu hỏi qua RS232 hoặc thẻ SD mà không cần build lại.
  4. Dùng CRC-16 và COBS cho khung UART; thêm hàng đợi TX kèm mutex cho USART2; tăng pool lên ít nhất bằng độ sâu queue + 1; dùng lại slot peer trên ESP.
  5. Game task block bằng event flags thay cho polling 100 ms.
  6. Mã hóa ESP-NOW (PMK/LMK); chọn kênh tự động; đặt tên phòng khác nhau.
  7. Dùng chân `ESP_BUSY` để điều khiển luồng; dùng thạch anh HSE.
  8. Lưu kết quả vào EEPROM; thêm hiệu ứng buzzer và LED; nâng cấp lên ESP32.

---

## TÀI LIỆU THAM KHẢO (gợi ý, cần bổ sung phiên bản và ngày truy cập)

1. STMicroelectronics, *RM0090 – Reference manual STM32F405/415, STM32F407/417, STM32F427/437, STM32F429/439*.
2. STMicroelectronics, *STM32F405xx/STM32F407xx Datasheet*.
3. STMicroelectronics, *UM1718 – STM32CubeMX for STM32 configuration and initialization C code generation*.
4. Ilitek, *ILI9341 Datasheet*; Texas Instruments/XPTek, *XPT2046 Touch Screen Controller Datasheet*.
5. Espressif, *ESP8266EX Datasheet*; *ESP8266_RTOS_SDK Programming Guide* (phần ESP-NOW, Wi-Fi, UART).
6. Espressif, *ESP-NOW User Guide*.
7. R. Barry, *Mastering the FreeRTOS Real Time Kernel*; *FreeRTOS Reference Manual*.
8. ARM, *CMSIS-RTOS2 API Documentation*.
9. LVGL, *LVGL v9 Documentation* (docs.lvgl.io): phần porting, display, input device, OS.
10. Kitware, *CMake Documentation* (`add_custom_command`, presets, toolchain file).

---

## PHỤ LỤC

- **Phụ lục A – Bảng chân đầy đủ**: xuất Pinout Report từ CubeMX, ghi rõ chức năng của từng chân.
- **Phụ lục B – Bảng lệnh giao thức đầy đủ**: Bảng 4.2, thêm ví dụ khung hex cho từng lệnh.
- **Phụ lục C – Hướng dẫn build và nạp**:
  - STM32: `cmake --preset Debug` → `cmake --build build/Debug` → nạp qua ST-Link.
  - ESP8266: build bằng ESP8266_RTOS_SDK → nạp qua UART (ghi rõ cách đưa ESP vào chế độ nạp trên KIT).
- **Phụ lục D – Định dạng `questions.json`**: mô tả từng trường, ràng buộc độ dài và ví dụ.
- **Phụ lục E – Log RS232 mẫu** cho một lượt chơi đầy đủ, có chú thích từng dòng.

---

## DANH MỤC HÌNH DỰ KIẾN (checklist phân công vẽ)

| Hình | Tên | Loại | Công cụ | Người vẽ |
|---|---|---|---|---|
| 1.1 | Công nghệ ↔ Kỹ năng | Mind map | draw.io/XMind | C |
| 1.2 | Use case | UML use case | PlantUML | C |
| 2.1 | Triển khai nhiều KIT | Deployment | draw.io | B |
| 2.2 | Khối phần cứng một KIT | Block diagram | draw.io | A |
| 2.3 | Cây clock | Block diagram / ảnh CubeMX | draw.io | A |
| 2.4 | Phân chia trách nhiệm 2 MCU | Block diagram | draw.io | B |
| 2.5 | Ma trận bus, tranh chấp FSMC | Block diagram | draw.io | A |
| 2.6 | Giản đồ thời gian ghi FSMC (tùy chọn) | Timing | WaveDrom | A |
| 3.1 | Pipeline build | Flowchart | draw.io | A |
| 3.2 | Kiến trúc phân lớp | Layer diagram | draw.io | A |
| 3.3 | Luồng debug RS232 | Data-flow | draw.io | A |
| 3.4 | Ảnh terminal | Ảnh chụp | – | A |
| 4.1 | Kiến trúc phần mềm | UML component | PlantUML | B |
| 4.2 | Định dạng khung | Bytefield | draw.io | B |
| 4.3 | Parser 5 trạng thái | UML state | PlantUML | B |
| 4.4 | Chèn và bóc MAC | Data transform | draw.io | B |
| 4.5 | Vòng đời một peer | UML state | PlantUML | B |
| 4.6 | Phát hiện người chơi rớt | UML sequence | PlantUML | B |
| 4.7 | Task và queue của ESP8266 | Data-flow | draw.io | B |
| 5.1 | FSM Host | UML state | PlantUML | C |
| 5.2 | FSM Player | UML state | PlantUML | C |
| 5.3 | Tranh chấp hết giờ / đủ đáp án | UML sequence | PlantUML | C |
| 6.1 | Kịch bản tổng thể | UML activity (swimlane) | PlantUML | B |
| 6.2 | Khởi động | Flowchart 2 cột | draw.io | A |
| 6.3 | Tìm phòng và tham gia | UML sequence | PlantUML | B |
| 6.4 | Một vòng hỏi đáp | UML sequence | PlantUML | B |
| 6.5 | Xử lý SUBMIT_ANSWER (tùy chọn) | Flowchart | draw.io | C |
| 7.1 | Lớp porting LVGL | Block diagram | draw.io | C |
| 7.2 | Double buffering | Timing / Gantt | WaveDrom | C |
| 7.3 | Điều hướng màn hình | Screen flow | draw.io | C |
| 7.4 | Ảnh chụp 10 màn hình | Ảnh chụp | – | C |
| 8.1 | Bố cục `Question_t` | Memory layout | draw.io | C |
| 8.2 | JSON → struct → payload | Data transform | draw.io | C |
| 8.3 | Flowchart script | Flowchart | draw.io | C |
| 8.4 | Build tự động | UML sequence | PlantUML | C |
| 9.1 | Task và IPC | Task diagram | draw.io | A |
| 9.2 | Timeline thực thi | Timing (số đo) | WaveDrom / logic analyzer | A |
| 9.3 | Đường nhận ISR → task | Data-flow | draw.io | B |
| 9.4 | HardFault: trước và sau | UML sequence ×2 | PlantUML | B |
| 9.5 | Ma trận sở hữu tài nguyên | Bảng | – | A |
| 9.6 | Bản đồ bộ nhớ | Memory map | draw.io | A |
| 9.7 | Chuỗi cấp phát | Block diagram | draw.io | A |
| 9.8 | Self-test SRAM | Flowchart | draw.io | A |
| 10.1 | Bố trí thí nghiệm | Ảnh + mặt bằng | draw.io | B |
| 10.2–10.5 | Biểu đồ kết quả đo | Box / line / bar | Python (matplotlib) hoặc Excel | B (10.2, 10.3), A (10.4, 10.5) |
| 10.6 | Ảnh một lượt chơi | Ảnh chụp | – | C |
| 11.1 | Gantt 5 tuần | Gantt | PlantUML | A |
