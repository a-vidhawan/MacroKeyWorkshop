{
    writecommand(0xFD); // COMMAND LOCK
    writedata(0x12);
    writecommand(0xAE); // DISPLAY OFF
    writecommand(0xB3); // CLOCKDIV
    writedata(0xB0);
    writecommand(0xCA); // MUXRATIO
    writedata(0x3F);
    writecommand(0xA2); // DISPLAY OFFSET
    writedata(0x40);
    writecommand(0xA1); // STARTLINE
    writedata(0x00);
    writecommand(0xA0); // SET REMAP
    writedata(0x72);
    writedata(0x00);
    writecommand(0xC1); // CONTRASTABC
    writedata(0x88);
    writedata(0x32);
    writedata(0x88);
    writecommand(0xC7); // CONTRASTMASTER
    writedata(0x0F);
    writecommand(0xB1); // SET PHASE LENGTH
    writedata(0x32);
    writecommand(0xB6); // PRECHARGE2
    writedata(0x01);
    writecommand(0xB8); // GAMMA LOOKUP
    writedata(0x02);
    writedata(0x03);
    writedata(0x04);
    writedata(0x05);
    writedata(0x06);
    writedata(0x07);
    writedata(0x08);
    writedata(0x09);
    writedata(0x0A);
    writedata(0x0B);
    writedata(0x0C);
    writedata(0x0D);
    writedata(0x0E);
    writedata(0x0F);
    writedata(0x10);
    writedata(0x11);
    writedata(0x12);
    writedata(0x13);
    writedata(0x15);
    writedata(0x17);
    writedata(0x19);
    writedata(0x1B);
    writedata(0x1D);
    writedata(0x1F);
    writedata(0x21);
    writedata(0x23);
    writedata(0x25);
    writedata(0x27);
    writedata(0x2A);
    writedata(0x2D);
    writedata(0x30);
    writedata(0x33);
    writedata(0x36);
    writedata(0x39);
    writedata(0x3C);
    writedata(0x3F);
    writedata(0x42);
    writedata(0x45);
    writedata(0x48);
    writedata(0x4C);
    writedata(0x50);
    writedata(0x54);
    writedata(0x58);
    writedata(0x5C);
    writedata(0x60);
    writedata(0x64);
    writedata(0x68);
    writedata(0x6C);
    writedata(0x70);
    writedata(0x74);
    writedata(0x78);
    writedata(0x7D);
    writedata(0x82);
    writedata(0x87);
    writedata(0x8C);
    writedata(0x91);
    writedata(0x96);
    writedata(0x9B);
    writedata(0xA0);
    writedata(0xA5);
    writedata(0xAA);
    writedata(0xAF);
    writedata(0xB4);
    writecommand(0xBB); // PRECHARGE VOLTAGE
    writedata(0x17);
    writecommand(0xBE); // VCOMH
    writedata(0x05);
    writecommand(0x15); // SET COLUMN ADDRESS
    writedata(0x20);
    writedata(0x5F);
    writecommand(0x75); // SET ROW ADDRESS
    writedata(0x00);
    writedata(0x3F);
    writecommand(0xA6); // SET DISPLAY MODE
    delay(100);
    writecommand(0xAF); // DISPLAY ON
    delay(100);
}