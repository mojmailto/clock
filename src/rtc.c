#include "rtc.h"
#include "pinout.h"

#define DS1307_ADDRESS (0x68 << 1)

static I2C_HandleTypeDef hi2c2;

static uint8_t BCD_To_DEC(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t DEC_To_BCD(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef* hi2c) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if (hi2c->Instance == I2C2) {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        __HAL_RCC_I2C2_CLK_ENABLE();
        GPIO_InitStruct.Pin = RTC_SCL_PIN | RTC_SDA_PIN;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        HAL_GPIO_Init(RTC_SCL_PORT, &GPIO_InitStruct);
    }
}

void RTC_Init(void) {
    hi2c2.Instance = I2C2;
    hi2c2.Init.ClockSpeed = 100000;
    hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c2.Init.OwnAddress1 = 0;
    hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c2.Init.OwnAddress2 = 0;
    hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c2);
}

void RTC_GetTime(RTC_TimeTypeDef *time) {
    uint8_t buf[7];
    uint8_t reg = 0x00;
    HAL_I2C_Master_Transmit(&hi2c2, DS1307_ADDRESS, &reg, 1, HAL_MAX_DELAY);
    HAL_I2C_Master_Receive(&hi2c2, DS1307_ADDRESS, buf, 7, HAL_MAX_DELAY);

    time->seconds = BCD_To_DEC(buf[0] & 0x7F);
    time->minutes = BCD_To_DEC(buf[1]);
    time->hours = BCD_To_DEC(buf[2] & 0x3F);
    time->day_of_week = BCD_To_DEC(buf[3]);
    time->day_of_month = BCD_To_DEC(buf[4]);
    time->month = BCD_To_DEC(buf[5]);
    time->year = BCD_To_DEC(buf[6]);
}

void RTC_SetTime(RTC_TimeTypeDef *time) {
    uint8_t buf[8];
    buf[0] = 0x00; // Start register
    buf[1] = DEC_To_BCD(time->seconds);
    buf[2] = DEC_To_BCD(time->minutes);
    buf[3] = DEC_To_BCD(time->hours);
    buf[4] = DEC_To_BCD(time->day_of_week);
    buf[5] = DEC_To_BCD(time->day_of_month);
    buf[6] = DEC_To_BCD(time->month);
    buf[7] = DEC_To_BCD(time->year);
    HAL_I2C_Master_Transmit(&hi2c2, DS1307_ADDRESS, buf, 8, HAL_MAX_DELAY);
}
