#ifndef UI_H
#define UI_H

#include "rtc.h"
#include <stdbool.h>

void UI_Init(void);
void UI_DrawMainScreen(DateTimeTypeDef *time, bool alarm_set, uint8_t alarm_h, uint8_t alarm_m);
void UI_DrawMenu(int8_t selection);
void UI_DrawSetTime(DateTimeTypeDef *time, int8_t field);
void UI_DrawSetAlarm(uint8_t h, uint8_t m, int8_t field);

#endif /* UI_H */
