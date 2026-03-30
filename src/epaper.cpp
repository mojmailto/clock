#include "epaper.h"
#include "pinout.h"
#include "font.h"
#include <Arduino.h>
#include <string.h>

#define EPD_W_BYTES ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1))
#define EPD_BUFFER_SIZE (EPD_W_BYTES * EPD_HEIGHT)

static uint8_t image_buffer[EPD_BUFFER_SIZE];

static void EPD_WaitBusy(void) {
    while (digitalRead(EPD_BUSY_PIN) == HIGH) delay(1);
}

static void SPI_WriteByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(EPD_SCL_PIN, LOW);
        digitalWrite(EPD_SDA_PIN, (data & 0x80) ? HIGH : LOW);
        digitalWrite(EPD_SCL_PIN, HIGH);
        data <<= 1;
    }
}

static void EPD_WriteCmd(uint8_t cmd) {
    digitalWrite(EPD_DC_PIN, LOW);
    digitalWrite(EPD_CS_PIN, LOW);
    SPI_WriteByte(cmd);
    digitalWrite(EPD_CS_PIN, HIGH);
}

static void EPD_WriteData(uint8_t data) {
    digitalWrite(EPD_DC_PIN, HIGH);
    digitalWrite(EPD_CS_PIN, LOW);
    SPI_WriteByte(data);
    digitalWrite(EPD_CS_PIN, HIGH);
}

void EPD_Init(void) {
    pinMode(EPD_SCL_PIN, OUTPUT);
    pinMode(EPD_SDA_PIN, OUTPUT);
    pinMode(EPD_CS_PIN, OUTPUT);
    pinMode(EPD_DC_PIN, OUTPUT);
    pinMode(EPD_RES_PIN, OUTPUT);
    pinMode(EPD_BUSY_PIN, INPUT);

    digitalWrite(EPD_RES_PIN, LOW);
    delay(10);
    digitalWrite(EPD_RES_PIN, HIGH);
    delay(10);

    EPD_WaitBusy();
    EPD_WriteCmd(0x12); // Soft Reset
    EPD_WaitBusy();

    EPD_WriteCmd(0x01); // Driver output control
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x11); // Data entry mode
    EPD_WriteData(0x01);

    EPD_WriteCmd(0x44); // Set RAM X start/end position
    EPD_WriteData(0x00);
    EPD_WriteData(0x0F);

    EPD_WriteCmd(0x45); // Set RAM Y start/end position
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x3C); // BorderWavefrom
    EPD_WriteData(0x05);

    EPD_WriteCmd(0x21); // Display update control
    EPD_WriteData(0x00);
    EPD_WriteData(0x80);
}

void EPD_Clear(uint8_t color) {
    memset(image_buffer, (color == EPD_COLOR_WHITE) ? 0xFF : 0x00, EPD_BUFFER_SIZE);
}

void EPD_DrawPoint(uint16_t x, uint16_t y, uint8_t color) {
    uint16_t nx = y;
    uint16_t ny = 295 - x;
    if (x >= 296 || y >= 128) return;
    uint32_t addr = (ny * EPD_W_BYTES) + (nx / 8);
    if (color == EPD_COLOR_BLACK || color == EPD_COLOR_RED)
        image_buffer[addr] &= ~(0x80 >> (nx % 8));
    else
        image_buffer[addr] |= (0x80 >> (nx % 8));
}

void EPD_DrawText(uint16_t x, uint16_t y, const char* text, uint8_t size, uint8_t color) {
    while (*text) {
        uint8_t c = *text - ' ';
        if (size == 16) {
            for (int i = 0; i < 16; i++) {
                uint8_t line = asc2_1608[c][i];
                for (int j = 0; j < 8; j++) {
                    if (line & (0x80 >> j)) EPD_DrawPoint(x + j, y + i, color);
                }
            }
            x += 8;
        } else if (size == 24) {
            for (int i = 0; i < 24; i++) {
                uint16_t line = (asc2_2412[c][i*2] << 8) | asc2_2412[c][i*2+1];
                for (int j = 0; j < 12; j++) {
                    if (line & (0x8000 >> j)) EPD_DrawPoint(x + j, y + i, color);
                }
            }
            x += 12;
        } else {
            for (int i = 0; i < 6; i++) {
                uint8_t line = asc2_0806[c][i];
                for (int j = 0; j < 8; j++) {
                    if (line & (0x01 << j)) EPD_DrawPoint(x + i, y + j, color);
                }
            }
            x += 6;
        }
        text++;
    }
}

void EPD_DisplayBW(void) {
    EPD_WriteCmd(0x24);
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++) EPD_WriteData(image_buffer[i]);
}

void EPD_DisplayRed(void) {
    EPD_WriteCmd(0x26);
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++) EPD_WriteData(image_buffer[i]);
}

void EPD_DisplayUpdate(void) {
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xF7);
    EPD_WriteCmd(0x20);
    EPD_WaitBusy();
}

void EPD_Sleep(void) {
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);
}
