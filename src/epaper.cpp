#include "epaper.h"
#include "pinout.h"
#include "font.h"
#include <Arduino.h>
#include <string.h>

#define EPD_W_BYTES ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1))
#define EPD_BUFFER_SIZE (EPD_W_BYTES * EPD_HEIGHT)

static uint8_t image_buffer[EPD_BUFFER_SIZE];

static void EPD_WaitBusy(void) {
    uint32_t timeout = millis();
    while (digitalRead(EPD_BUSY_PIN) == HIGH) {
        if (millis() - timeout > 10000) {
            Serial.println("EPD Busy Timeout!");
            break;
        }
        delay(1);
    }
}

static void SPI_WriteByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        digitalWrite(EPD_SCL_PIN, LOW);
        delayMicroseconds(1);
        digitalWrite(EPD_SDA_PIN, (data & 0x80) ? HIGH : LOW);
        delayMicroseconds(1);
        digitalWrite(EPD_SCL_PIN, HIGH);
        delayMicroseconds(1);
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

    digitalWrite(EPD_CS_PIN, HIGH);
    digitalWrite(EPD_SCL_PIN, LOW);

    digitalWrite(EPD_RES_PIN, LOW);
    delay(50);
    digitalWrite(EPD_RES_PIN, HIGH);
    delay(50);

    EPD_WaitBusy();
    EPD_WriteCmd(0x12); // Soft Reset
    EPD_WaitBusy();

    EPD_WriteCmd(0x01); // Driver output control
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x11); // Data entry mode
    EPD_WriteData(0x03);

    EPD_WriteCmd(0x44); // Set RAM X start/end position
    EPD_WriteData(0x00);
    EPD_WriteData(0x0F);

    EPD_WriteCmd(0x45); // Set RAM Y start/end position
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);

    EPD_WriteCmd(0x3C); // Border Waveform
    EPD_WriteData(0x05);

    EPD_WriteCmd(0x18); // Internal Temp Sensor
    EPD_WriteData(0x80);

    EPD_WriteCmd(0x21); // Display Update Control
    EPD_WriteData(0x00);
    EPD_WriteData(0x80);

    EPD_WaitBusy();
}

void EPD_Clear(uint8_t color) {
    memset(image_buffer, (color == EPD_COLOR_WHITE) ? 0xFF : 0x00, EPD_BUFFER_SIZE);
}

void EPD_DrawPoint(uint16_t x, uint16_t y, uint8_t color) {
    if (x >= 296 || y >= 128) return;
    uint32_t addr = (x * EPD_W_BYTES) + (y / 8);
    if (color == EPD_COLOR_BLACK || color == EPD_COLOR_RED)
        image_buffer[addr] &= ~(0x80 >> (y % 8));
    else
        image_buffer[addr] |= (0x80 >> (y % 8));
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
            for (int i = 0; i < 12; i++) {
                for (int j = 0; j < 3; j++) {
                    uint8_t line = asc2_2412[c][i * 3 + j];
                    for (int k = 0; k < 8; k++) {
                        if (line & (0x80 >> k)) EPD_DrawPoint(x + i, y + j * 8 + k, color);
                    }
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
    EPD_WriteCmd(0x4E); // Set RAM X address counter
    EPD_WriteData(0x00);
    EPD_WriteCmd(0x4F); // Set RAM Y address counter
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x24);
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++) EPD_WriteData(image_buffer[i]);
}

void EPD_DisplayRed(void) {
    EPD_WriteCmd(0x4E);
    EPD_WriteData(0x00);
    EPD_WriteCmd(0x4F);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x26);
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++) EPD_WriteData(image_buffer[i]);
}

void EPD_DisplayUpdate(void) {
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xF7); // All refresh phases
    EPD_WriteCmd(0x20); // Master Activation
    EPD_WaitBusy();
}

void EPD_Sleep(void) {
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);
}
