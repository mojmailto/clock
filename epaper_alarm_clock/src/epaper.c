#include "epaper.h"
#include "pinout.h"
#include <string.h>

#define EPD_W_BYTES ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1))
#define EPD_BUFFER_SIZE (EPD_W_BYTES * EPD_HEIGHT)

static uint8_t image_buffer[EPD_BUFFER_SIZE];

/* Fonts: using 8x16 font as example */
extern const unsigned char asc2_1608[][16];

static void EPD_WaitBusy(void) {
    uint32_t timeout = 0xFFFF;
    while (GPIO_ReadInputDataBit(EPD_BUSY_PORT, EPD_BUSY_PIN) == Bit_SET && timeout--);
}

static void SPI_WriteByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        GPIO_WriteBit(EPD_SCL_PORT, EPD_SCL_PIN, Bit_RESET);
        if (data & 0x80)
            GPIO_WriteBit(EPD_SDA_PORT, EPD_SDA_PIN, Bit_SET);
        else
            GPIO_WriteBit(EPD_SDA_PORT, EPD_SDA_PIN, Bit_RESET);
        GPIO_WriteBit(EPD_SCL_PORT, EPD_SCL_PIN, Bit_SET);
        data <<= 1;
    }
}

static void EPD_WriteCmd(uint8_t cmd) {
    GPIO_WriteBit(EPD_DC_PORT, EPD_DC_PIN, Bit_RESET);
    GPIO_WriteBit(EPD_CS_PORT, EPD_CS_PIN, Bit_RESET);
    SPI_WriteByte(cmd);
    GPIO_WriteBit(EPD_CS_PORT, EPD_CS_PIN, Bit_SET);
}

static void EPD_WriteData(uint8_t data) {
    GPIO_WriteBit(EPD_DC_PORT, EPD_DC_PIN, Bit_SET);
    GPIO_WriteBit(EPD_CS_PORT, EPD_CS_PIN, Bit_RESET);
    SPI_WriteByte(data);
    GPIO_WriteBit(EPD_CS_PORT, EPD_CS_PIN, Bit_SET);
}

void EPD_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);

    GPIO_InitStructure.GPIO_Pin = EPD_SCL_PIN | EPD_SDA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = EPD_CS_PIN | EPD_DC_PIN | EPD_RES_PIN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = EPD_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_WriteBit(EPD_RES_PORT, EPD_RES_PIN, Bit_RESET);
    for(volatile int i=0; i<10000; i++);
    GPIO_WriteBit(EPD_RES_PORT, EPD_RES_PIN, Bit_SET);
    for(volatile int i=0; i<10000; i++);

    EPD_WaitBusy();
    EPD_WriteCmd(0x12); // Soft reset
    EPD_WaitBusy();

    EPD_WriteCmd(0x01); // Driver output control
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x11); // Data entry mode
    EPD_WriteData(0x01); // Y decrement, X increment

    EPD_WriteCmd(0x44); // X address
    EPD_WriteData(0x00);
    EPD_WriteData(0x0F);

    EPD_WriteCmd(0x45); // Y address
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x3C); // Border
    EPD_WriteData(0x05);

    EPD_WriteCmd(0x21); // Display update control
    EPD_WriteData(0x00);
    EPD_WriteData(0x80);
}

void EPD_Clear(uint8_t color) {
    memset(image_buffer, (color == EPD_COLOR_WHITE) ? 0xFF : 0x00, EPD_BUFFER_SIZE);
}

void EPD_DrawPoint(uint16_t x, uint16_t y, uint8_t color) {
    if (x >= EPD_WIDTH || y >= EPD_HEIGHT) return;
    uint32_t addr = (y * EPD_W_BYTES) + (x / 8);
    if (color == EPD_COLOR_BLACK || color == EPD_COLOR_RED)
        image_buffer[addr] &= ~(0x80 >> (x % 8));
    else
        image_buffer[addr] |= (0x80 >> (x % 8));
}

void EPD_DrawText(uint16_t x, uint16_t y, const char* text, uint8_t size, uint8_t color) {
    while (*text) {
        uint8_t c = *text - ' ';
        for (int i = 0; i < 16; i++) {
            uint8_t line = asc2_1608[c][i];
            for (int j = 0; j < 8; j++) {
                if (line & (0x80 >> j)) {
                    EPD_DrawPoint(x + j, y + i, color);
                }
            }
        }
        x += 8;
        text++;
    }
}

void EPD_DisplayBW(void) {
    EPD_WriteCmd(0x24); // RAM black/white
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++) {
        EPD_WriteData(image_buffer[i]);
    }
}

void EPD_DisplayRed(void) {
    EPD_WriteCmd(0x26); // RAM red
    for (uint32_t i = 0; i < EPD_BUFFER_SIZE; i++) {
        EPD_WriteData(~image_buffer[i]); // SSD1680 red is usually inverted or depends on config
    }
}

void EPD_DisplayUpdate(void) {
    EPD_WriteCmd(0x22);
    EPD_WriteData(0xF7); // Load LUT from OTP, display update
    EPD_WriteCmd(0x20);
    EPD_WaitBusy();
}

void EPD_Sleep(void) {
    EPD_WriteCmd(0x10);
    EPD_WriteData(0x01);
}
