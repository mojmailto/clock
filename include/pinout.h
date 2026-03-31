#ifndef PINOUT_H
#define PINOUT_H

/* RTC: DS1307Z (I2C on ESP32) */
#define RTC_SCL_PIN   26
#define RTC_SDA_PIN   25

/* Encoder: CLK (GPIO33), DT (GPIO32), SW (GPIO35)
   Note: GPIO35 is input-only and requires an external pull-up resistor. */
#define ENC_CLK_PIN   33
#define ENC_DT_PIN    32
#define ENC_SW_PIN    35

/* E-paper: SDA (GPIO21), SCL (GPIO23), CS (GPIO22), D/C (GPIO19), RES (GPIO18), BUSY (GPIO5) */
#define EPD_SCL_PIN   23
#define EPD_SDA_PIN   21
#define EPD_CS_PIN    22
#define EPD_DC_PIN    19
#define EPD_RES_PIN   18
#define EPD_BUSY_PIN  5

#endif /* PINOUT_H */
