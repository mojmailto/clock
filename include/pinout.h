#ifndef PINOUT_H
#define PINOUT_H

/* RTC: DS1307Z (Software I2C on ESP8266) */
#define RTC_SCL_PIN   9
#define RTC_SDA_PIN   8

/* Encoder: CLK (GPIO15), DT (GPIO13), SW (GPIO12) */
#define ENC_CLK_PIN   15
#define ENC_DT_PIN    13
#define ENC_SW_PIN    12

/* E-paper: SDA (GPIO4), SCL (GPIO5), CS (GPIO16), D/C (GPIO0), RES (GPIO2), BUSY (GPIO14) */
#define EPD_SCL_PIN   5
#define EPD_SDA_PIN   4
#define EPD_CS_PIN    16
#define EPD_DC_PIN    0
#define EPD_RES_PIN   2
#define EPD_BUSY_PIN  14

#endif /* PINOUT_H */
