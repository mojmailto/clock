#include "encoder.h"
#include "pinout.h"

static volatile int32_t encoder_delta = 0;
static volatile bool button_pressed = false;
static volatile uint32_t last_irq_time = 0;

void ENC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStructure;
    EXTI_InitTypeDef EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

    GPIO_InitStructure.GPIO_Pin = ENC_CLK_PIN | ENC_DT_PIN | ENC_SW_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);

    EXTI_InitStructure.EXTI_Line = EXTI_Line12 | EXTI_Line14;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

extern volatile uint32_t ms_ticks;

void EXTI15_10_IRQHandler(void) {
    static uint32_t last_irq_time = 0;
    uint32_t current_time = ms_ticks;

    if (current_time - last_irq_time < 10) { // 10ms debounce
        EXTI_ClearITPendingBit(EXTI_Line12 | EXTI_Line14);
        return;
    }
    last_irq_time = current_time;

    if (EXTI_GetITStatus(EXTI_Line12) != RESET) {
        if (GPIO_ReadInputDataBit(ENC_DT_PORT, ENC_DT_PIN) != RESET) {
            encoder_delta++;
        } else {
            encoder_delta--;
        }
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if (EXTI_GetITStatus(EXTI_Line14) != RESET) {
        button_pressed = true;
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
}

ENC_Event ENC_GetEvent(void) {
    if (button_pressed) {
        button_pressed = false;
        return ENC_CLICK;
    }
    if (encoder_delta > 0) {
        encoder_delta--;
        return ENC_RIGHT;
    }
    if (encoder_delta < 0) {
        encoder_delta++;
        return ENC_LEFT;
    }
    return ENC_NONE;
}
