// ============================================================================
// TFT_eSPI User_Setup.h Configuration for ESP32 MacroPad
// ============================================================================
//
// This is an example configuration file for the TFT_eSPI library.
// Copy this content to the TFT_eSPI library's User_Setup.h file.
//
// Location of User_Setup.h:
// Arduino IDE: Documents/Arduino/libraries/TFT_eSPI/User_Setup.h
// PlatformIO: .pio/libdeps/esp32dev/TFT_eSPI/User_Setup.h
//
// After editing, recompile and upload the firmware.
// ============================================================================

// ############################################################################
// #                                                                          #
// # ESP32 MacroPad Configuration for ILI9341 Display                        #
// #                                                                          #
// ############################################################################

// Define the display driver - ILI9341
#define ILI9341_DRIVER

// Display resolution
#define TFT_WIDTH  240
#define TFT_HEIGHT 320

// ############################################################################
// #                                                                          #
// # Pin Definitions for ESP32                                               #
// #                                                                          #
// ############################################################################

// ESP32 Hardware SPI pins (do not change these for hardware SPI)
// MOSI (Master Out Slave In) - GPIO 23
// MISO (Master In Slave Out) - GPIO 19
// SCLK (SPI Clock)           - GPIO 18

// Control pins (these can be changed to match your wiring)
#define TFT_MISO 19  // SPI MISO
#define TFT_MOSI 23  // SPI MOSI
#define TFT_SCLK 18  // SPI Clock
#define TFT_CS   15  // Chip select control pin for display
#define TFT_DC    2  // Data Command control pin
#define TFT_RST   4  // Reset pin (could use -1 and tie to ESP32 EN pin)

// Touch controller chip select
#define TOUCH_CS  5  // Chip select pin for XPT2046 touch controller

// Optional backlight control
// #define TFT_BL   21  // LED back-light control pin
// #define TFT_BACKLIGHT_ON HIGH  // Level to turn ON back-light (HIGH or LOW)

// ############################################################################
// #                                                                          #
// # Font Support                                                             #
// #                                                                          #
// ############################################################################

// Load basic fonts
#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.

// Enable smooth fonts
#define SMOOTH_FONT

// ############################################################################
// #                                                                          #
// # SPI Frequency Settings                                                   #
// #                                                                          #
// ############################################################################

// SPI frequency for display (write operations)
#define SPI_FREQUENCY  40000000  // 40MHz (stable for most displays)
// #define SPI_FREQUENCY  27000000  // 27MHz (use if 40MHz is unstable)

// SPI frequency for reading display
#define SPI_READ_FREQUENCY  20000000  // 20MHz

// SPI frequency for touch controller
#define SPI_TOUCH_FREQUENCY  2500000  // 2.5MHz (XPT2046 recommended)

// ############################################################################
// #                                                                          #
// # Color Depth                                                              #
// #                                                                          #
// ############################################################################

// Color depth - 16-bit is standard for ILI9341
// Comment this out for 18-bit color (not recommended for ILI9341)
#define TFT_RGB_ORDER TFT_BGR  // Color order Blue-Green-Red

// ############################################################################
// #                                                                          #
// # Optional Settings                                                        #
// #                                                                          #
// ############################################################################

// Use HSPI for ESP32 (VSPI is default)
// Uncomment to use HSPI instead of VSPI
// #define USE_HSPI_PORT

// DMA for faster screen updates (ESP32 only)
// Uncomment to disable DMA
// #define DISABLE_ALL_LIBRARY_WARNINGS

// ############################################################################
// #                                                                          #
// # Touch Controller Configuration (XPT2046)                                #
// #                                                                          #
// ############################################################################

// Touch controller calibration
// Run the TFT_eSPI touch calibration example to get these values
// These are default values and may need adjustment for your display

// Note: Calibration is also done in the main firmware code
// These settings may not be used depending on library version

// ############################################################################
// #                                                                          #
// # End of Configuration                                                     #
// #                                                                          #
// ############################################################################

// Don't change anything below this line

// ESP32 standard SPI VSPI port (default)
#ifndef USE_HSPI_PORT
  #define VSPI_MOSI 23
  #define VSPI_MISO 19
  #define VSPI_SCLK 18
#endif
