#ifndef PINOUT_H
#define PINOUT_H

/* Target STM32F103C6 is Low Density device */
#ifndef STM32F10X_LD
#define STM32F10X_LD
#endif

#ifndef USE_STDPERIPH_DRIVER
#define USE_STDPERIPH_DRIVER
#endif

#include "stm32f10x.h"

/* RTC: DS1307Z (I2C2) */
#define RTC_I2C       I2C2
#define RTC_SCL_PIN   GPIO_Pin_10
#define RTC_SCL_PORT  GPIOB
#define RTC_SDA_PIN   GPIO_Pin_11
#define RTC_SDA_PORT  GPIOB

/* Encoder: CLK (B12), DT (B13), SW (B14) */
#define ENC_CLK_PIN   GPIO_Pin_12
#define ENC_CLK_PORT  GPIOB
#define ENC_DT_PIN    GPIO_Pin_13
#define ENC_DT_PORT   GPIOB
#define ENC_SW_PIN    GPIO_Pin_14
#define ENC_SW_PORT   GPIOB

/* E-paper: SDA (B7), SCL (B6), CS (A2), D/C (A3), RES (A4), BUSY (A5) */
#define EPD_SCL_PIN   GPIO_Pin_6
#define EPD_SCL_PORT  GPIOB
#define EPD_SDA_PIN   GPIO_Pin_7
#define EPD_SDA_PORT  GPIOB
#define EPD_CS_PIN    GPIO_Pin_2
#define EPD_CS_PORT   GPIOA
#define EPD_DC_PIN    GPIO_Pin_3
#define EPD_DC_PORT   GPIOA
#define EPD_RES_PIN   GPIO_Pin_4
#define EPD_RES_PORT  GPIOA
#define EPD_BUSY_PIN  GPIO_Pin_5
#define EPD_BUSY_PORT GPIOA

#endif /* PINOUT_H */
