#ifndef RTC_H
#define RTC_H

#include "pinout.h"
#include <stdint.h>

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week;
    uint8_t day_of_month;
    uint8_t month;
    uint8_t year;
} RTC_TimeTypeDef;

void RTC_Init(void);
void RTC_GetTime(RTC_TimeTypeDef *time);
void RTC_SetTime(RTC_TimeTypeDef *time);

#endif /* RTC_H */
