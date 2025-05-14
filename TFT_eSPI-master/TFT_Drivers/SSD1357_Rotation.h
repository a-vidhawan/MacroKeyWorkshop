
// This is the command sequence that rotates the SSD1357 driver coordinate frame

rotation = m % 4; // Limit the range of values to 0-3

uint8_t madctl = 0x62;
uint8_t startline = 0x00;
uint8_t dispoffset = 0x00;

switch (rotation) {
    case 0:
        madctl |= 0x10;
        _width  = _init_width;
        _height = _init_height;
        startline = 0x00;
        dispoffset = 64;
        #ifdef CGRAM_OFFSET
            colstart = 32;
            rowstart = 0;
        #endif
    break;
    case 1:
        madctl = 0b01110011;
        _width  = _init_height;
        _height = _init_width;
        startline = 0x00;
        dispoffset = 64;
        #ifdef CGRAM_OFFSET
            colstart = 0;
            rowstart = 32;
        #endif
        break;
    case 2:
        madctl |= 0x00;  // TODO
        _width  = _init_width;
        _height = _init_height;
        startline = 0x00;
        dispoffset = 64;
        #ifdef CGRAM_OFFSET
            colstart = 0;
            rowstart = 32;
        #endif
    break;
    case 3:
        madctl |= 0x00;  // TODO
        _width  = _init_height;
        _height = _init_width;
        startline = 0x00;
        dispoffset = 64;
        #ifdef CGRAM_OFFSET
            colstart = 0;
            rowstart = 32;
        #endif
    break;
}

writecommand(0xA0); // SETREMAP
writedata(madctl);
writecommand(0xA1); // STARTLINE
writedata(startline);
writecommand(0xA2); // DISPLAYOFFSET
writedata(dispoffset);
