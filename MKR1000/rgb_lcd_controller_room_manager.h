#ifndef RGB_LCD_ROOM_MANAGER

#define RGB_LCD_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

void setupLcd();

void setNavigationMode(int active);

void setTooHotAlarm(boolean active);

void setTooColdAlarm(boolean active);

void updateInfoScreenRows(int temp, int light, boolean wifi, boolean deviceConfigured);

void updateTempScreenRows(int tempStatus, int tempConfig, int tempActivationThreshold);

void updateLightScreenRows(int lightStatus, int lightConfig, int lightActivationThreshold);

void updateAlarmScreenRows(boolean fireAlarm);

void updateScreenCursor(int active, int displayRow);

void updateScreenRowsText(char screenRows[2][16]);

void wifiLoadingScreen(boolean i);

void dbLoadingScreen(boolean i);

void loggingLoadingScreen(boolean i);

void showLoadingScreen(boolean loadingScreen);


#endif
