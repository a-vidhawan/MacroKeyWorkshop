#ifndef TFT_WIDTH
    #define TFT_WIDTH 64
#endif
#ifndef TFT_HEIGHT
    #define TFT_HEIGHT 64
#endif

#if (TFT_HEIGHT == 64) && (TFT_WIDTH == 64)
    #define CGRAM_OFFSET
#endif
// Delay between some initialisation commands
#define TFT_INIT_DELAY 0x80

// Generic commands used by TFT_eSPI.cpp
#define TFT_NOP     0xE3
#define TFT_SWRST   TFT_NOP
#define TFT_CASET   0x15 // SETCOLUMN
#define TFT_PASET   0x75 // SETROW
#define TFT_RAMWR   0x5C // WRITERAM
#define TFT_RAMRD   0x5D
#define TFT_IDXRD   TFT_NOP
#define TFT_INVOFF  0xA6 // NORMALDISPLAY
#define TFT_INVON   0xA7 // INVERTDISPLAY
#define TFT_DISPOFF 0xA4 // DISPLAYALLOFF
#define TFT_DISPON  0xA5 // DISPLAYALLON