#include "rtc.h"
#include "pinout.h"

#define DS1307_ADDRESS 0xD0

static void I2C_Delay(void) {
    for (volatile int i = 0; i < 50; i++);
}

static void I2C_Start(void) {
    HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, GPIO_PIN_RESET);
    I2C_Delay();
    HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_RESET);
}

static void I2C_Stop(void) {
    HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, GPIO_PIN_SET);
    I2C_Delay();
}

static uint8_t I2C_Write(uint8_t data) {
    for (int i = 0; i < 8; i++) {
        HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, (data & 0x80) ? GPIO_PIN_SET : GPIO_PIN_RESET);
        data <<= 1;
        I2C_Delay();
        HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_SET);
        I2C_Delay();
        HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_RESET);
    }
    // Simple ACK check
    HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, GPIO_PIN_SET);
    HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    uint8_t ack = !HAL_GPIO_ReadPin(RTC_SDA_PORT, RTC_SDA_PIN);
    HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_RESET);
    return ack;
}

static uint8_t I2C_Read(uint8_t ack) {
    uint8_t data = 0;
    HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, GPIO_PIN_SET);
    for (int i = 0; i < 8; i++) {
        data <<= 1;
        HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_SET);
        I2C_Delay();
        if (HAL_GPIO_ReadPin(RTC_SDA_PORT, RTC_SDA_PIN)) data |= 0x01;
        HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_RESET);
        I2C_Delay();
    }
    HAL_GPIO_WritePin(RTC_SDA_PORT, RTC_SDA_PIN, ack ? GPIO_PIN_RESET : GPIO_PIN_SET);
    HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_SET);
    I2C_Delay();
    HAL_GPIO_WritePin(RTC_SCL_PORT, RTC_SCL_PIN, GPIO_PIN_RESET);
    return data;
}

static uint8_t BCD_To_DEC(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t DEC_To_BCD(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

void RTC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStruct.Pin = RTC_SCL_PIN | RTC_SDA_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(RTC_SCL_PORT, &GPIO_InitStruct);
}

void RTC_GetTime(DateTimeTypeDef *time) {
    I2C_Start();
    I2C_Write(DS1307_ADDRESS);
    I2C_Write(0x00);
    I2C_Start();
    I2C_Write(DS1307_ADDRESS | 1);
    time->seconds = BCD_To_DEC(I2C_Read(1) & 0x7F);
    time->minutes = BCD_To_DEC(I2C_Read(1));
    time->hours = BCD_To_DEC(I2C_Read(1) & 0x3F);
    time->day_of_week = BCD_To_DEC(I2C_Read(1));
    time->day_of_month = BCD_To_DEC(I2C_Read(1));
    time->month = BCD_To_DEC(I2C_Read(1));
    time->year = BCD_To_DEC(I2C_Read(0));
    I2C_Stop();
}

void RTC_SetTime(DateTimeTypeDef *time) {
    I2C_Start();
    I2C_Write(DS1307_ADDRESS);
    I2C_Write(0x00);
    I2C_Write(DEC_To_BCD(time->seconds));
    I2C_Write(DEC_To_BCD(time->minutes));
    I2C_Write(DEC_To_BCD(time->hours));
    I2C_Write(DEC_To_BCD(time->day_of_week));
    I2C_Write(DEC_To_BCD(time->day_of_month));
    I2C_Write(DEC_To_BCD(time->month));
    I2C_Write(DEC_To_BCD(time->year));
    I2C_Stop();
}
