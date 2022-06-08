#ifndef RGB_LCD_ROOM_MANAGER

#define RGB_LCD_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

void setupLcd();

void setNavigationMode(int active);

void setTooHotAlarm(boolean active);

void setTooColdAlarm(boolean active);

void updateInfoScreenRows(int temp, int light, boolean wifi, boolean isMySqlConnected);

void updateTempScreenRows(int temp, int tempConfig, int tempActivationThreshold,int tempStatus);

void updateLightScreenRows(int light, int lightConfig, int lightActivationThreshold);

void updateAlarmScreenRows(boolean fireAlarm);

void updateScreenCursor(int active, int displayRow);

void updateScreenRowsText(char screenRows[2][16]);

void wifiLoadingScreen(boolean i);

void lowPowerModeLCD();

void notLowPowerModeLCD() ;


void dbLoadingScreen(boolean i);

void loggingLoadingScreen(boolean i);

// Function to validate an IP address
bool validateIP(String ip);

#endif
