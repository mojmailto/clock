#include "rtc.h"
#include "pinout.h"
#include <Wire.h>

#define DS1307_ADDRESS 0x68

static uint8_t BCD_To_DEC(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t DEC_To_BCD(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

void RTC_Init(void) {
    Wire.begin(RTC_SDA_PIN, RTC_SCL_PIN);
}

void RTC_GetTime(DateTimeTypeDef *time) {
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.requestFrom(DS1307_ADDRESS, 7);
    time->seconds = BCD_To_DEC(Wire.read() & 0x7F);
    time->minutes = BCD_To_DEC(Wire.read());
    time->hours = BCD_To_DEC(Wire.read() & 0x3F);
    time->day_of_week = BCD_To_DEC(Wire.read());
    time->day_of_month = BCD_To_DEC(Wire.read());
    time->month = BCD_To_DEC(Wire.read());
    time->year = BCD_To_DEC(Wire.read());
}

void RTC_SetTime(DateTimeTypeDef *time) {
    Wire.beginTransmission(DS1307_ADDRESS);
    Wire.write(0x00);
    Wire.write(DEC_To_BCD(time->seconds));
    Wire.write(DEC_To_BCD(time->minutes));
    Wire.write(DEC_To_BCD(time->hours));
    Wire.write(DEC_To_BCD(time->day_of_week));
    Wire.write(DEC_To_BCD(time->day_of_month));
    Wire.write(DEC_To_BCD(time->month));
    Wire.write(DEC_To_BCD(time->year));
    Wire.endTransmission();
}
