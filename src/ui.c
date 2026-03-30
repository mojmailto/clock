#include "ui.h"
#include "epaper.h"
#include <stdio.h>
#include <string.h>

static const char* DAYS_PL[] = {"Pn", "Wt", "Sr", "Cz", "Pt", "So", "Nd"};

void UI_Init(void) {
    EPD_Init();
}

void UI_DrawMainScreen(DateTimeTypeDef *time, bool alarm_set, uint8_t alarm_h, uint8_t alarm_m) {
    char buf[16];

    // --- Phase 1: Render Black/White RAM ---
    EPD_Clear(EPD_COLOR_WHITE);
    // Draw Time: HH:MM
    sprintf(buf, "%02d:%02d", time->hours, time->minutes);
    EPD_DrawText(20, 40, buf, 16, EPD_COLOR_BLACK);
    // Draw Day of Week
    if (time->day_of_week >= 1 && time->day_of_week <= 7) {
        EPD_DrawText(150, 40, DAYS_PL[time->day_of_week - 1], 16, EPD_COLOR_BLACK);
    }
    EPD_DisplayBW();

    // --- Phase 2: Render Red RAM ---
    EPD_Clear(EPD_COLOR_WHITE);
    if (alarm_set) {
        // Draw Alarm Icon (simulated as text or simple pixels in red)
        EPD_DrawText(20, 100, "ALARM", 16, EPD_COLOR_RED);
        sprintf(buf, " %02d:%02d", alarm_h, alarm_m);
        EPD_DrawText(60, 100, buf, 16, EPD_COLOR_RED);
    }
    EPD_DisplayRed();

    // --- Phase 3: Trigger Display Update ---
    EPD_DisplayUpdate();
}

void UI_DrawMenu(int8_t selection) {
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DrawText(10, 10, "MENU:", 16, EPD_COLOR_BLACK);
    EPD_DrawText(30, 40, (selection == 0) ? "> Ust. Czas" : "  Ust. Czas", 16, EPD_COLOR_BLACK);
    EPD_DrawText(30, 70, (selection == 1) ? "> Ust. Budzik" : "  Ust. Budzik", 16, EPD_COLOR_BLACK);
    EPD_DrawText(30, 100, (selection == 2) ? "> Budzik Wl/Wyl" : "  Budzik Wl/Wyl", 16, EPD_COLOR_BLACK);

    EPD_DisplayBW();
    EPD_Clear(EPD_COLOR_WHITE); // No red content in menu
    EPD_DisplayRed();
    EPD_DisplayUpdate();
}

void UI_DrawSetTime(DateTimeTypeDef *time, int8_t field) {
    char buf[32];
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DrawText(10, 10, "USTAW CZAS:", 16, EPD_COLOR_BLACK);
    sprintf(buf, "%02d:%02d:%02d", time->hours, time->minutes, time->seconds);
    EPD_DrawText(30, 50, buf, 16, EPD_COLOR_BLACK);

    // Draw cursor (underline) under active field
    if (field == 0) EPD_DrawText(30, 66, "__", 16, EPD_COLOR_BLACK);
    else if (field == 1) EPD_DrawText(54, 66, "__", 16, EPD_COLOR_BLACK);

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
    EPD_DrawText(30, 50, buf, 16, EPD_COLOR_BLACK);

    // Draw cursor (underline) under active field
    if (field == 0) EPD_DrawText(30, 66, "__", 16, EPD_COLOR_BLACK);
    else if (field == 1) EPD_DrawText(54, 66, "__", 16, EPD_COLOR_BLACK);

    EPD_DisplayBW();
    EPD_Clear(EPD_COLOR_WHITE);
    EPD_DisplayRed();
    EPD_DisplayUpdate();
}
