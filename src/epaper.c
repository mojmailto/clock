#include "epaper.h"
#include "pinout.h"
#include "font.h"
#include <string.h>

#define EPD_W_BYTES ((EPD_WIDTH % 8 == 0) ? (EPD_WIDTH / 8) : (EPD_WIDTH / 8 + 1))
#define EPD_BUFFER_SIZE (EPD_W_BYTES * EPD_HEIGHT)

static uint8_t image_buffer[EPD_BUFFER_SIZE];

static void EPD_WaitBusy(void) {
    while (HAL_GPIO_ReadPin(EPD_BUSY_PORT, EPD_BUSY_PIN) == GPIO_PIN_SET);
}

static void SPI_WriteByte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(EPD_SCL_PORT, EPD_SCL_PIN, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(EPD_SDA_PORT, EPD_SDA_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        HAL_GPIO_WritePin(EPD_SCL_PORT, EPD_SCL_PIN, GPIO_PIN_SET);
        data <<= 1;
    }
}

static void EPD_WriteCmd(uint8_t cmd) {
    HAL_GPIO_WritePin(EPD_DC_PORT, EPD_DC_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(EPD_CS_PORT, EPD_CS_PIN, GPIO_PIN_RESET);
    SPI_WriteByte(cmd);
    HAL_GPIO_WritePin(EPD_CS_PORT, EPD_CS_PIN, GPIO_PIN_SET);
}

static void EPD_WriteData(uint8_t data) {
    HAL_GPIO_WritePin(EPD_DC_PORT, EPD_DC_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(EPD_CS_PORT, EPD_CS_PIN, GPIO_PIN_RESET);
    SPI_WriteByte(data);
    HAL_GPIO_WritePin(EPD_CS_PORT, EPD_CS_PIN, GPIO_PIN_SET);
}

void EPD_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = EPD_SCL_PIN | EPD_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EPD_CS_PIN | EPD_DC_PIN | EPD_RES_PIN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = EPD_BUSY_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    HAL_GPIO_WritePin(EPD_RES_PORT, EPD_RES_PIN, GPIO_PIN_RESET);
    HAL_Delay(10);
    HAL_GPIO_WritePin(EPD_RES_PORT, EPD_RES_PIN, GPIO_PIN_SET);
    HAL_Delay(10);

    EPD_WaitBusy();
    EPD_WriteCmd(0x12);
    EPD_WaitBusy();

    EPD_WriteCmd(0x01);
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x11);
    EPD_WriteData(0x01);

    EPD_WriteCmd(0x44);
    EPD_WriteData(0x00);
    EPD_WriteData(0x0F);

    EPD_WriteCmd(0x45);
    EPD_WriteData(0x27);
    EPD_WriteData(0x01);
    EPD_WriteData(0x00);
    EPD_WriteData(0x00);

    EPD_WriteCmd(0x3C);
    EPD_WriteData(0x05);

    EPD_WriteCmd(0x21);
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
