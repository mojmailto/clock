#ifndef RTC_H
#define RTC_H

#include "stm32f1xx_hal.h"
#include <stdint.h>

typedef struct {
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;
    uint8_t day_of_week;
    uint8_t day_of_month;
    uint8_t month;
    uint8_t year;
} DateTimeTypeDef;

void RTC_Init(void);
void RTC_GetTime(DateTimeTypeDef *time);
void RTC_SetTime(DateTimeTypeDef *time);

#endif /* RTC_H */
