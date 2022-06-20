#ifndef RGB_LCD_ROOM_MANAGER

#define RGB_LCD_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager_master.h"

void setupLcd();

void updateInfoScreenRows(int devices, boolean wifi, boolean isMySqlConnected, boolean singleMode, boolean sleepMode, boolean ecoMode);

void updateScreenCursor(int active, int displayRow);

void updateScreenRowsText(char screenRows[2][16]);

void wifiLoadingScreen(boolean i);

void dbLoadingScreen(boolean i);

void loggingLoadingScreen(boolean i);

void MQTTLoadingScreen(boolean i);

#endif
