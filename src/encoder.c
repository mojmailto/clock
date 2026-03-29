#include "encoder.h"
#include "pinout.h"

static volatile int32_t encoder_delta = 0;
static volatile bool button_pressed = false;

void ENC_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIO_InitStruct.Pin = ENC_CLK_PIN | ENC_DT_PIN | ENC_SW_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    static uint32_t last_irq_time = 0;
    uint32_t current_time = HAL_GetTick();

    if (current_time - last_irq_time < 10) return;
    last_irq_time = current_time;

    if (GPIO_Pin == ENC_CLK_PIN) {
        if (HAL_GPIO_ReadPin(ENC_DT_PORT, ENC_DT_PIN) == GPIO_PIN_SET) {
            encoder_delta++;
        } else {
            encoder_delta--;
        }
    } else if (GPIO_Pin == ENC_SW_PIN) {
        button_pressed = true;
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
