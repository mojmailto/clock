#ifndef ENCODER_H
#define ENCODER_H

#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

typedef enum {
    ENC_NONE,
    ENC_LEFT,
    ENC_RIGHT,
    ENC_CLICK
} ENC_Event;

void ENC_Init(void);
ENC_Event ENC_GetEvent(void);

#endif /* ENCODER_H */
