#include "ui.h"
#include "epaper.h"
#include <stdio.h>
#include <string.h>

static const char* DAYS_PL[] = {"Pn", "Wt", "Sr", "Cz", "Pt", "So", "Nd"};

void UI_Init(void) {
    EPD_Init();
}

static void UI_DrawAlarmIcon(uint16_t x, uint16_t y) {
    // Simple alarm clock icon 16x16
    // Top bells
    for(int i=0; i<4; i++) {
        EPD_DrawPoint(x+2+i, y+2-i, EPD_COLOR_RED);
        EPD_DrawPoint(x+14-i, y+2-i, EPD_COLOR_RED);
    }
    // Main circle
    for(int i=0; i<12; i++) {
        EPD_DrawPoint(x+2+i, y+4, EPD_COLOR_RED);
        EPD_DrawPoint(x+2+i, y+15, EPD_COLOR_RED);
        EPD_DrawPoint(x+1, y+5+i, EPD_COLOR_RED);
        EPD_DrawPoint(x+14, y+5+i, EPD_COLOR_RED);
    }
    // Hands
    EPD_DrawPoint(x+7, y+9, EPD_COLOR_RED);
    EPD_DrawPoint(x+8, y+9, EPD_COLOR_RED);
    EPD_DrawPoint(x+7, y+8, EPD_COLOR_RED);
    EPD_DrawPoint(x+9, y+9, EPD_COLOR_RED);
}

void UI_DrawMainScreen(DateTimeTypeDef *time, bool alarm_set, uint8_t alarm_h, uint8_t alarm_m) {
    char buf[16];

    // BW Phase
    EPD_Clear(EPD_COLOR_WHITE);
    sprintf(buf, "%02d:%02d", time->hours, time->minutes);
    EPD_DrawText(20, 30, buf, 24, EPD_COLOR_BLACK);

    if (time->day_of_week >= 1 && time->day_of_week <= 7) {
        EPD_DrawText(150, 35, DAYS_PL[time->day_of_week - 1], 16, EPD_COLOR_BLACK);
    }
    EPD_DisplayBW();

    // Red Phase
    EPD_Clear(EPD_COLOR_WHITE);
    if (alarm_set) {
        UI_DrawAlarmIcon(20, 80);
        sprintf(buf, "%02d:%02d", alarm_h, alarm_m);
        EPD_DrawText(45, 80, buf, 16, EPD_COLOR_RED);
    }
    EPD_DisplayRed();

    EPD_DisplayUpdate();
}

void UI_DrawMenu(int8_t selection) {
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DrawText(10, 10, "MENU:", 16, EPD_COLOR_BLACK);
    EPD_DrawText(30, 40, (selection == 0) ? "> Ust. Czas" : "  Ust. Czas", 16, EPD_COLOR_BLACK);
    EPD_DrawText(30, 70, (selection == 1) ? "> Ust. Budzik" : "  Ust. Budzik", 16, EPD_COLOR_BLACK);
    EPD_DrawText(30, 100, (selection == 2) ? "> Budzik Wl/Wyl" : "  Budzik Wl/Wyl", 16, EPD_COLOR_BLACK);
    EPD_DisplayBW();

    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DisplayRed();
    EPD_DisplayUpdate();
}

void UI_DrawSetTime(DateTimeTypeDef *time, int8_t field) {
    char buf[32];
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DrawText(10, 10, "USTAW CZAS:", 16, EPD_COLOR_BLACK);
    sprintf(buf, "%02d:%02d", time->hours, time->minutes);
    EPD_DrawText(30, 50, buf, 24, EPD_COLOR_BLACK);

    if (field == 0) EPD_DrawText(30, 75, "__", 16, EPD_COLOR_BLACK);
    else if (field == 1) EPD_DrawText(66, 75, "__", 16, EPD_COLOR_BLACK);

    EPD_DisplayBW();
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DisplayRed();
    EPD_DisplayUpdate();
}

void UI_DrawSetAlarm(uint8_t h, uint8_t m, int8_t field) {
    char buf[32];
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DrawText(10, 10, "USTAW BUDZIK:", 16, EPD_COLOR_BLACK);
    sprintf(buf, "%02d:%02d", h, m);
    EPD_DrawText(30, 50, buf, 24, EPD_COLOR_BLACK);

    if (field == 0) EPD_DrawText(30, 75, "__", 16, EPD_COLOR_BLACK);
    else if (field == 1) EPD_DrawText(66, 75, "__", 16, EPD_COLOR_BLACK);

    EPD_DisplayBW();
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DisplayRed();
    EPD_DisplayUpdate();
}
