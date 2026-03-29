#include "rtc.h"
#include "pinout.h"

#define DS1307_ADDRESS 0xD0
#define I2C_TIMEOUT    10000

static uint8_t BCD_To_DEC(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

static uint8_t DEC_To_BCD(uint8_t dec) {
    return ((dec / 10) << 4) | (dec % 10);
}

void RTC_Init(void) {
    I2C_InitTypeDef I2C_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);

    GPIO_InitStructure.GPIO_Pin = RTC_SCL_PIN | RTC_SDA_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed = 100000;
    I2C_Init(RTC_I2C, &I2C_InitStructure);
    I2C_Cmd(RTC_I2C, ENABLE);
}

static int I2C_WaitEvent_Timeout(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT) {
    uint32_t timeout = I2C_TIMEOUT;
    while (!I2C_CheckEvent(I2Cx, I2C_EVENT)) {
        if (--timeout == 0) return -1;
    }
    return 0;
}

void RTC_GetTime(RTC_TimeTypeDef *time) {
    I2C_GenerateSTART(RTC_I2C, ENABLE);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT) < 0) return;

    I2C_Send7bitAddress(RTC_I2C, DS1307_ADDRESS, I2C_Direction_Transmitter);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) < 0) return;

    I2C_SendData(RTC_I2C, 0x00);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) < 0) return;

    I2C_GenerateSTART(RTC_I2C, ENABLE);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT) < 0) return;

    I2C_Send7bitAddress(RTC_I2C, DS1307_ADDRESS, I2C_Direction_Receiver);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED) < 0) return;

    I2C_AcknowledgeConfig(RTC_I2C, ENABLE);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) < 0) return;
    time->seconds = BCD_To_DEC(I2C_ReceiveData(RTC_I2C) & 0x7F);

    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) < 0) return;
    time->minutes = BCD_To_DEC(I2C_ReceiveData(RTC_I2C));

    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) < 0) return;
    time->hours = BCD_To_DEC(I2C_ReceiveData(RTC_I2C) & 0x3F);

    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) < 0) return;
    time->day_of_week = BCD_To_DEC(I2C_ReceiveData(RTC_I2C));

    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) < 0) return;
    time->day_of_month = BCD_To_DEC(I2C_ReceiveData(RTC_I2C));

    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) < 0) return;
    time->month = BCD_To_DEC(I2C_ReceiveData(RTC_I2C));

    I2C_AcknowledgeConfig(RTC_I2C, DISABLE);
    I2C_GenerateSTOP(RTC_I2C, ENABLE);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_RECEIVED) < 0) return;
    time->year = BCD_To_DEC(I2C_ReceiveData(RTC_I2C));
}

void RTC_SetTime(RTC_TimeTypeDef *time) {
    I2C_GenerateSTART(RTC_I2C, ENABLE);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_MODE_SELECT) < 0) return;

    I2C_Send7bitAddress(RTC_I2C, DS1307_ADDRESS, I2C_Direction_Transmitter);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED) < 0) return;

    I2C_SendData(RTC_I2C, 0x00);
    if (I2C_WaitEvent_Timeout(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED) < 0) return;

    I2C_SendData(RTC_I2C, DEC_To_BCD(time->seconds));
    I2C_WaitEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_SendData(RTC_I2C, DEC_To_BCD(time->minutes));
    I2C_WaitEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_SendData(RTC_I2C, DEC_To_BCD(time->hours));
    I2C_WaitEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_SendData(RTC_I2C, DEC_To_BCD(time->day_of_week));
    I2C_WaitEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_SendData(RTC_I2C, DEC_To_BCD(time->day_of_month));
    I2C_WaitEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_SendData(RTC_I2C, DEC_To_BCD(time->month));
    I2C_WaitEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_SendData(RTC_I2C, DEC_To_BCD(time->year));
    I2C_WaitEvent(RTC_I2C, I2C_EVENT_MASTER_BYTE_TRANSMITTED);

    I2C_GenerateSTOP(RTC_I2C, ENABLE);
}
