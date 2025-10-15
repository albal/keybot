# ESP-IDF Development Guide

This document provides detailed guidance for developing the ESP32 MacroPad using ESP-IDF (Espressif IoT Development Framework).

## Why ESP-IDF Instead of Arduino?

### Advantages of ESP-IDF

1. **Native ESP32 Support**: Direct access to all ESP32 hardware features
2. **Better Performance**: Optimized for ESP32 architecture
3. **Industry Standard**: Used in commercial ESP32 products
4. **Comprehensive Documentation**: Official Espressif documentation and examples
5. **FreeRTOS Integration**: Full RTOS capabilities for multi-tasking
6. **Advanced Features**: Better control over Bluetooth, WiFi, and peripherals
7. **Professional Development**: CMake build system, proper toolchain
8. **Lower Memory Overhead**: No Arduino abstraction layer

### Trade-offs

- **Steeper Learning Curve**: Requires understanding of RTOS and ESP32 APIs
- **More Verbose Code**: Less abstraction means more explicit code
- **Manual Component Management**: Need to implement or integrate drivers
- **Command-Line Focus**: Primarily terminal-based workflow

## ESP-IDF Architecture

### Project Structure

```
esp32_macropad/
├── CMakeLists.txt              # Root build configuration
├── sdkconfig                   # Project configuration (generated)
├── main/
│   ├── CMakeLists.txt          # Main component
│   ├── main.c                  # Application entry point
│   ├── display.c               # Display driver (to be added)
│   ├── display.h
│   ├── bluetooth.c             # BLE HID implementation (to be added)
│   ├── bluetooth.h
│   ├── touch.c                 # Touch controller (to be added)
│   ├── touch.h
│   ├── ui.c                    # UI rendering (to be added)
│   └── ui.h
└── components/                 # Optional external components
    └── ili9341/                # ILI9341 driver component (optional)
```

### Key Concepts

#### 1. app_main()
The entry point for ESP-IDF applications. Unlike Arduino's `setup()` and `loop()`:

```c
void app_main(void) {
    // Initialize hardware
    init_nvs();
    init_spi();
    
    // Create FreeRTOS tasks
    xTaskCreate(ui_task, "ui_task", 4096, NULL, 5, NULL);
    xTaskCreate(touch_task, "touch_task", 2048, NULL, 4, NULL);
    
    // app_main() can return - tasks continue running
}
```

#### 2. FreeRTOS Tasks
ESP-IDF uses FreeRTOS for multitasking:

```c
void ui_task(void *pvParameters) {
    while (1) {
        // Update UI
        draw_screen();
        
        // Yield to other tasks
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
```

#### 3. Event Groups and Queues
Inter-task communication:

```c
// Create queue
QueueHandle_t touch_queue = xQueueCreate(10, sizeof(touch_event_t));

// Send from touch task
xQueueSend(touch_queue, &touch_event, portMAX_DELAY);

// Receive in UI task
touch_event_t event;
if (xQueueReceive(touch_queue, &event, pdMS_TO_TICKS(10))) {
    handle_touch(event.x, event.y);
}
```

## Implementing Key Components

### 1. Display Driver (ILI9341)

The ILI9341 requires SPI communication and initialization sequence:

**main/display.c**:
```c
#include "driver/spi_master.h"
#include "driver/gpio.h"

spi_device_handle_t spi_display;

void display_init(void) {
    // Configure SPI device
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 40 * 1000 * 1000,  // 40MHz
        .mode = 0,
        .spics_io_num = PIN_TFT_CS,
        .queue_size = 7,
        .pre_cb = NULL,
    };
    
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_HOST, &devcfg, &spi_display));
    
    // Send ILI9341 initialization commands
    ili9341_send_cmd(0x01);  // Software reset
    vTaskDelay(pdMS_TO_TICKS(150));
    // ... more initialization commands
}

void ili9341_send_cmd(uint8_t cmd) {
    gpio_set_level(PIN_TFT_DC, 0);  // Command mode
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd,
    };
    spi_device_transmit(spi_display, &t);
}

void ili9341_send_data(const uint8_t *data, int len) {
    gpio_set_level(PIN_TFT_DC, 1);  // Data mode
    spi_transaction_t t = {
        .length = len * 8,
        .tx_buffer = data,
    };
    spi_device_transmit(spi_display, &t);
}
```

**Alternative**: Use `esp_lcd` component:
```c
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"

esp_lcd_panel_handle_t panel_handle = NULL;

esp_lcd_panel_io_handle_t io_handle = NULL;
esp_lcd_panel_io_spi_config_t io_config = {
    .dc_gpio_num = PIN_TFT_DC,
    .cs_gpio_num = PIN_TFT_CS,
    .pclk_hz = 40 * 1000 * 1000,
    .lcd_cmd_bits = 8,
    .lcd_param_bits = 8,
    .spi_mode = 0,
    .trans_queue_depth = 10,
};
ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi((esp_lcd_spi_bus_handle_t)SPI_HOST, 
                                           &io_config, &io_handle));

esp_lcd_panel_dev_config_t panel_config = {
    .reset_gpio_num = PIN_TFT_RST,
    .color_space = ESP_LCD_COLOR_SPACE_RGB,
    .bits_per_pixel = 16,
};
ESP_ERROR_CHECK(esp_lcd_new_panel_ili9341(io_handle, &panel_config, &panel_handle));
ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
```

### 2. Touch Controller (XPT2046)

XPT2046 also uses SPI:

**main/touch.c**:
```c
spi_device_handle_t spi_touch;

void touch_init(void) {
    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 2 * 1000 * 1000,  // 2MHz
        .mode = 0,
        .spics_io_num = PIN_TOUCH_CS,
        .queue_size = 1,
    };
    
    ESP_ERROR_CHECK(spi_bus_add_device(SPI_HOST, &devcfg, &spi_touch));
}

bool touch_read(uint16_t *x, uint16_t *y) {
    uint8_t cmd_x = 0x90;  // Read X
    uint8_t cmd_y = 0xD0;  // Read Y
    uint8_t data[2];
    
    spi_transaction_t t = {
        .length = 8,
        .tx_buffer = &cmd_x,
        .rx_buffer = data,
        .rxlength = 16,
    };
    
    spi_device_transmit(spi_touch, &t);
    *x = ((data[0] << 8) | data[1]) >> 3;
    
    // Similar for Y coordinate
    // Apply calibration and pressure detection
    
    return true;  // If touch detected
}

void touch_task(void *pvParameters) {
    uint16_t x, y;
    while (1) {
        if (touch_read(&x, &y)) {
            // Send to UI task via queue
            touch_event_t event = {.x = x, .y = y};
            xQueueSend(touch_queue, &event, 0);
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

### 3. Bluetooth HID

ESP-IDF provides Bluedroid stack with HID profile:

**main/bluetooth.c**:
```c
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"
#include "esp_bt_defs.h"
#include "esp_gatt_common_api.h"
#include "esp_hidd_profile_api.h"

void ble_init(void) {
    // Initialize NVS for Bluetooth
    nvs_flash_init();
    
    // Initialize Bluetooth controller
    esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
    esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);
    
    // Initialize Bluedroid
    esp_bluedroid_init();
    esp_bluedroid_enable();
    
    // Register HID callbacks
    esp_hidd_profile_init();
    
    // Set device name
    esp_ble_gap_set_device_name("ESP32 MacroPad");
    
    // Configure advertising
    // ... advertising configuration
    
    // Start advertising
    esp_ble_gap_start_advertising(&adv_params);
}

void ble_send_keyboard_report(uint8_t modifier, uint8_t keycode) {
    uint8_t report[8] = {modifier, 0, keycode, 0, 0, 0, 0, 0};
    esp_hidd_send_keyboard_value(report, sizeof(report));
}

void ble_send_text(const char *text) {
    for (int i = 0; i < strlen(text); i++) {
        uint8_t keycode = ascii_to_keycode(text[i]);
        ble_send_keyboard_report(0, keycode);
        vTaskDelay(pdMS_TO_TICKS(10));
        ble_send_keyboard_report(0, 0);  // Release
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

**Reference Example**: 
Check `$IDF_PATH/examples/bluetooth/bluedroid/ble/ble_hid_device_demo/`

### 4. NVS (Non-Volatile Storage)

Already implemented in main.c. The NVS API is straightforward:

```c
// Open namespace
nvs_handle_t nvs_handle;
nvs_open("macropad", NVS_READWRITE, &nvs_handle);

// Write string
nvs_set_str(nvs_handle, "macro0", "Hello World");
nvs_commit(nvs_handle);

// Read string
size_t required_size;
nvs_get_str(nvs_handle, "macro0", NULL, &required_size);
char *value = malloc(required_size);
nvs_get_str(nvs_handle, "macro0", value, &required_size);

// Close
nvs_close(nvs_handle);
```

## Building and Flashing

### Basic Commands

```bash
# Set environment
. ~/esp/esp-idf/export.sh

# Configure project
idf.py menuconfig

# Clean build
idf.py fullclean
idf.py build

# Flash and monitor
idf.py -p /dev/ttyUSB0 flash monitor

# Monitor only
idf.py -p /dev/ttyUSB0 monitor

# Erase flash
idf.py -p /dev/ttyUSB0 erase-flash
```

### Configuration (menuconfig)

Important configuration options:

1. **Serial flasher config**:
   - Flash size: 4MB
   - Baud rate: 921600 (for faster flashing)

2. **Component config → Bluetooth**:
   - Bluedroid - Enable: Yes
   - Bluedroid - Host: Enabled
   - BLE HIDD: Enabled

3. **Component config → FreeRTOS**:
   - CPU frequency: 240 MHz
   - Tick rate: 1000 Hz

4. **Component config → ESP32-specific**:
   - CPU frequency: 240 MHz
   - Support for external SPI RAM: No (unless using PSRAM)

### Partition Table

Default partition table is usually sufficient:
```
# Name,   Type, SubType, Offset,  Size, Flags
nvs,      data, nvs,     0x9000,  0x6000,
phy_init, data, phy,     0xf000,  0x1000,
factory,  app,  factory, 0x10000, 1M,
```

For larger applications, create `partitions.csv`:
```
# Name,   Type, SubType, Offset,   Size, Flags
nvs,      data, nvs,     0x9000,   24K,
phy_init, data, phy,     0xf000,   4K,
factory,  app,  factory, 0x10000,  1536K,
storage,  data, spiffs,  ,         512K,
```

Reference in `CMakeLists.txt`:
```cmake
set(EXTRA_COMPONENT_DIRS components)
set(PROJECT_PARTITION_TABLE "partitions.csv")
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(esp32_macropad)
```

## Debugging

### Serial Logging

ESP-IDF provides comprehensive logging:

```c
#include "esp_log.h"

static const char *TAG = "MACROPAD";

ESP_LOGI(TAG, "Information message");
ESP_LOGW(TAG, "Warning message");
ESP_LOGE(TAG, "Error message: %s", esp_err_to_name(err));
ESP_LOGD(TAG, "Debug message (disabled in production)");
ESP_LOGV(TAG, "Verbose message (disabled in production)");
```

Set log level in menuconfig or at runtime:
```c
esp_log_level_set("MACROPAD", ESP_LOG_DEBUG);
esp_log_level_set("*", ESP_LOG_INFO);  // Set default for all tags
```

### JTAG Debugging

For advanced debugging, use OpenOCD with JTAG:

```bash
# In one terminal
openocd -f board/esp32-wrover-kit-3.3v.cfg

# In another terminal
xtensa-esp32-elf-gdb build/esp32_macropad.elf
(gdb) target remote :3333
(gdb) mon reset halt
(gdb) break app_main
(gdb) continue
```

### Core Dumps

Enable core dumps for crash analysis:
```bash
idf.py menuconfig
# Component config → ESP System Settings → Core dump
# Select "Flash" or "UART"
```

## Testing

### Unit Testing

ESP-IDF includes Unity test framework:

**test/test_macros.c**:
```c
#include "unity.h"
#include "nvs_flash.h"

TEST_CASE("NVS macro save and load", "[macros]")
{
    nvs_flash_init();
    
    const char *test_text = "Test Macro";
    save_macro(0, test_text);
    
    char loaded[MAX_MACRO_LEN];
    load_macro(0, loaded);
    
    TEST_ASSERT_EQUAL_STRING(test_text, loaded);
}

TEST_CASE("Macro buffer overflow protection", "[macros]")
{
    char large_text[MAX_MACRO_LEN + 100];
    memset(large_text, 'A', sizeof(large_text));
    large_text[sizeof(large_text) - 1] = '\0';
    
    save_macro(0, large_text);
    
    char loaded[MAX_MACRO_LEN];
    load_macro(0, loaded);
    
    TEST_ASSERT_EQUAL_INT(MAX_MACRO_LEN - 1, strlen(loaded));
}
```

Run tests:
```bash
idf.py -T test_macros build flash monitor
```

### Integration Testing

Use pytest for integration tests:

**test_integration.py**:
```python
import pytest
from pytest_embedded import Dut

def test_initialization(dut: Dut):
    dut.expect('ESP32 MacroPad Starting...', timeout=10)
    dut.expect('Initialization complete!', timeout=20)
    
def test_nvs_persistence(dut: Dut):
    dut.expect('Loading macros from NVS...', timeout=10)
    # Verify macros loaded correctly
```

## Performance Optimization

### Memory Management

Monitor heap usage:
```c
ESP_LOGI(TAG, "Free heap: %d bytes", esp_get_free_heap_size());
ESP_LOGI(TAG, "Minimum free heap: %d bytes", esp_get_minimum_free_heap_size());
```

### Task Stack Sizes

Monitor stack usage:
```c
UBaseType_t uxHighWaterMark = uxTaskGetStackHighWaterMark(NULL);
ESP_LOGI(TAG, "Task stack high water mark: %d", uxHighWaterMark);
```

Adjust in task creation:
```c
xTaskCreate(ui_task, "ui_task", 4096, NULL, 5, NULL);
//                               ^^^^ stack size in bytes
```

### DMA for SPI

Enable DMA for better SPI performance:
```c
spi_bus_config_t buscfg = {
    // ... other config
    .max_transfer_sz = SCREEN_WIDTH * SCREEN_HEIGHT * 2,
};

spi_bus_initialize(SPI_HOST, &buscfg, SPI_DMA_CH_AUTO);
```

## Useful Resources

### Official Documentation
- ESP-IDF Programming Guide: https://docs.espressif.com/projects/esp-idf/
- API Reference: https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/
- ESP-IDF Examples: $IDF_PATH/examples/

### Relevant Examples
- `examples/bluetooth/bluedroid/ble/ble_hid_device_demo/` - BLE HID
- `examples/peripherals/spi_master/` - SPI communication
- `examples/peripherals/lcd/` - LCD displays
- `examples/storage/nvs_rw_value/` - NVS usage
- `examples/system/freertos/` - FreeRTOS tasks

### Community
- ESP32 Forum: https://esp32.com
- GitHub Issues: https://github.com/espressif/esp-idf/issues
- Stack Overflow: Tag [esp32]

### Tools
- ESP-IDF Eclipse Plugin
- ESP-IDF VS Code Extension
- ESP-IDF Monitor (`idf.py monitor`)
- esptool.py (included with ESP-IDF)

## Migration Notes (Arduino to ESP-IDF)

### Common Arduino Functions → ESP-IDF Equivalents

| Arduino | ESP-IDF |
|---------|---------|
| `Serial.begin()` | `uart_driver_install()` + ESP_LOG |
| `Serial.println()` | `ESP_LOGI()` |
| `digitalWrite()` | `gpio_set_level()` |
| `digitalRead()` | `gpio_get_level()` |
| `pinMode()` | `gpio_config()` |
| `delay()` | `vTaskDelay(pdMS_TO_TICKS(ms))` |
| `millis()` | `xTaskGetTickCount() * portTICK_PERIOD_MS` |
| `analogRead()` | `adc_oneshot_read()` |
| `SPI.begin()` | `spi_bus_initialize()` |
| `Preferences.begin()` | `nvs_open()` |

### Conceptual Differences

1. **No automatic loop**: Use FreeRTOS tasks with `while(1)` loops
2. **Explicit error handling**: Check `ESP_ERROR_CHECK()` or handle `esp_err_t`
3. **Manual initialization**: Initialize each peripheral explicitly
4. **Memory management**: Be aware of heap vs stack allocation
5. **Concurrent execution**: Multiple tasks run simultaneously

## Conclusion

ESP-IDF provides powerful, native access to ESP32 capabilities. While more complex than Arduino, it offers:

- Better performance
- More control
- Professional tooling
- Industry-standard practices

The learning curve is worth it for production-quality firmware.

For this project, the main work is implementing:
1. **display.c** - ILI9341 driver and UI rendering
2. **touch.c** - XPT2046 touch controller
3. **bluetooth.c** - BLE HID keyboard implementation

The framework in `main/main.c` provides the structure. Reference ESP-IDF examples for implementation details.
