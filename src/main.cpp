#include <Arduino.h>
#include "pinout.h"
#include "rtc.h"
#include "encoder.h"
#include "epaper.h"
#include "ui.h"
#include <stdbool.h>

typedef enum {
    MODE_NORMAL,
    MODE_MENU,
    MODE_SET_TIME_H,
    MODE_SET_TIME_M,
    MODE_SET_ALARM_H,
    MODE_SET_ALARM_M,
    MODE_TOGGLE_ALARM
} APP_Mode;

static APP_Mode current_mode = MODE_NORMAL;
static DateTimeTypeDef current_time;
static uint8_t alarm_h = 7, alarm_m = 0;
static bool alarm_enabled = false;
static int8_t menu_selection = 0;

void setup() {
    Serial.begin(115200);
    RTC_Init();
    ENC_Init();
    EPD_Init();
    UI_Init();
}

void loop() {
    ENC_Event event = ENC_GetEvent();

    if (current_mode == MODE_NORMAL) {
        RTC_GetTime(&current_time);
    }

    switch (current_mode) {
        case MODE_NORMAL:
            if (event == ENC_CLICK) {
                current_mode = MODE_MENU;
                UI_DrawMenu(menu_selection);
            } else {
                static uint8_t last_m = 99;
                if (current_time.minutes != last_m) {
                    last_m = current_time.minutes;
                    UI_DrawMainScreen(&current_time, alarm_enabled, alarm_h, alarm_m);
                }
            }
            break;

        case MODE_MENU:
            if (event == ENC_LEFT) {
                menu_selection = (menu_selection > 0) ? menu_selection - 1 : 2;
                UI_DrawMenu(menu_selection);
            } else if (event == ENC_RIGHT) {
                menu_selection = (menu_selection < 2) ? menu_selection + 1 : 0;
                UI_DrawMenu(menu_selection);
            } else if (event == ENC_CLICK) {
                if (menu_selection == 0) {
                    current_mode = MODE_SET_TIME_H;
                    UI_DrawSetTime(&current_time, 0);
                } else if (menu_selection == 1) {
                    current_mode = MODE_SET_ALARM_H;
                    UI_DrawSetAlarm(alarm_h, alarm_m, 0);
                } else if (menu_selection == 2) {
                    current_mode = MODE_TOGGLE_ALARM;
                }
            }
            break;

        case MODE_SET_TIME_H:
            if (event != ENC_NONE) {
                if (event == ENC_LEFT) current_time.hours = (current_time.hours > 0) ? current_time.hours - 1 : 23;
                else if (event == ENC_RIGHT) current_time.hours = (current_time.hours < 23) ? current_time.hours + 1 : 0;
                else if (event == ENC_CLICK) current_mode = MODE_SET_TIME_M;
                UI_DrawSetTime(&current_time, 0);
            }
            break;

        case MODE_SET_TIME_M:
            if (event != ENC_NONE) {
                if (event == ENC_LEFT) current_time.minutes = (current_time.minutes > 0) ? current_time.minutes - 1 : 59;
                else if (event == ENC_RIGHT) current_time.minutes = (current_time.minutes < 59) ? current_time.minutes + 1 : 0;
                else if (event == ENC_CLICK) {
                    RTC_SetTime(&current_time);
                    current_mode = MODE_NORMAL;
                    UI_DrawMainScreen(&current_time, alarm_enabled, alarm_h, alarm_m);
                    break;
                }
                UI_DrawSetTime(&current_time, 1);
            }
            break;

        case MODE_SET_ALARM_H:
            if (event != ENC_NONE) {
                if (event == ENC_LEFT) alarm_h = (alarm_h > 0) ? alarm_h - 1 : 23;
                else if (event == ENC_RIGHT) alarm_h = (alarm_h < 23) ? alarm_h + 1 : 0;
                else if (event == ENC_CLICK) current_mode = MODE_SET_ALARM_M;
                UI_DrawSetAlarm(alarm_h, alarm_m, 0);
            }
            break;

        case MODE_SET_ALARM_M:
            if (event != ENC_NONE) {
                if (event == ENC_LEFT) alarm_m = (alarm_m > 0) ? alarm_m - 1 : 59;
                else if (event == ENC_RIGHT) alarm_m = (alarm_m < 59) ? alarm_m + 1 : 0;
                else if (event == ENC_CLICK) {
                    current_mode = MODE_NORMAL;
                    UI_DrawMainScreen(&current_time, alarm_enabled, alarm_h, alarm_m);
                    break;
                }
                UI_DrawSetAlarm(alarm_h, alarm_m, 1);
            }
            break;

        case MODE_TOGGLE_ALARM:
            alarm_enabled = !alarm_enabled;
            current_mode = MODE_NORMAL;
            UI_DrawMainScreen(&current_time, alarm_enabled, alarm_h, alarm_m);
            break;
    }

    if (alarm_enabled && current_time.hours == alarm_h && current_time.minutes == alarm_m && current_time.seconds == 0) {
        // Optional buzzer logic could be added here
        Serial.println("ALARM!");
    }

    delay(10);
}
