#ifndef EPAPER_H
#define EPAPER_H

#include <stdint.h>

#define EPD_WIDTH  128
#define EPD_HEIGHT 296

#define EPD_COLOR_WHITE 0xFF
#define EPD_COLOR_BLACK 0x00
#define EPD_COLOR_RED   0x01

void EPD_Init(void);
void EPD_Clear(uint8_t color);
void EPD_DrawPoint(uint16_t x, uint16_t y, uint8_t color);
void EPD_DrawText(uint16_t x, uint16_t y, const char* text, uint8_t size, uint8_t color);
void EPD_DisplayBW(void);
void EPD_DisplayRed(void);
void EPD_DisplayUpdate(void);
void EPD_Sleep(void);

#endif /* EPAPER_H */
