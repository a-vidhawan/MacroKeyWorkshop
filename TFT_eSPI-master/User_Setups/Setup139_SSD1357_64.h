#define USER_SETUP_ID 139

#define SSD1357_DRIVER

#define TFT_WIDTH  64
#define TFT_HEIGHT 64

#define TFT_RGB_ORDER TFT_RGB
#define SPI_FREQUENCY  10000000

#if defined(ESP32)
#define TFT_MOSI 23
  #define TFT_SCLK 18
  #define TFT_DC   2
  #define TFT_RST  4
  #define TFT_CS   15
#elif defined (ARDUINO_ARCH_ESP8266)
//#define TFT_MOSI PIN_D5 // Can't change
//#define TFT_SCLK PIN_D7 // Can't change
  #define TFT_DC   PIN_D3
  #define TFT_RST  PIN_D4
  #define TFT_CS   PIN_D8
#endif

#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:-.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
//#define LOAD_FONT8N // Font 8. Alternative to Font 8 above, slightly narrower, so 3 digits fit a 160 pixel TFT
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts

// Comment out the #define below to stop the SPIFFS filing system and smooth font code being loaded
// this will save ~20kbytes of FLASH
#define SMOOTH_FONT