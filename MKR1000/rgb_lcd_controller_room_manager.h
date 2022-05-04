#ifndef RGB_LCD_ROOM_MANAGER

#define RGB_LCD_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

void setupLcd();

void setNavigationMode(int active);

void updateInfoScreenRows(int tempStatus, int tempConfig, int tempActivationThreshold);

void updateTempScreenRows(int tempStatus, int tempConfig, int tempActivationThreshold);

void updateLightScreenRows(int lightStatus, int lightConfig, int lightActivationThreshold);

void updateScreenCursor(int active, int displayRow);

void updateScreenRowsText(char screenRows[2][16]);

#endif
