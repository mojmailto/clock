#include "encoder.h"
#include "pinout.h"
#include <Arduino.h>

static volatile int32_t encoder_delta = 0;
static volatile bool button_pressed = false;

static void IRAM_ATTR encoder_isr() {
    static uint32_t last_interrupt_time = 0;
    uint32_t interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 5) {
        if (digitalRead(ENC_DT_PIN) == digitalRead(ENC_CLK_PIN)) {
            encoder_delta--;
        } else {
            encoder_delta++;
        }
    }
    last_interrupt_time = interrupt_time;
}

static void IRAM_ATTR button_isr() {
    static uint32_t last_interrupt_time = 0;
    uint32_t interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 200) {
        button_pressed = true;
    }
    last_interrupt_time = interrupt_time;
}

void ENC_Init(void) {
    pinMode(ENC_CLK_PIN, INPUT_PULLUP);
    pinMode(ENC_DT_PIN, INPUT_PULLUP);
    pinMode(ENC_SW_PIN, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(ENC_CLK_PIN), encoder_isr, CHANGE);
    attachInterrupt(digitalPinToInterrupt(ENC_SW_PIN), button_isr, FALLING);
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
