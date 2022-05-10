#ifndef ACTION_CONTROLLER_ROOM_MANAGER

#define ACTION_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

void actionTempScreen(int pressedButton, int* displayRow, int* tempActivationThreshold, int* tempConfig, boolean* navigationMode);

void actionLightScreen(int pressedButton, int* displayRow, int* lightActivationThreshold, int* lightConfig, boolean* navigationMode);

void actionAlarmScreen(int pressedButton, boolean* navigationMode, boolean* fireAlarm);

#endif
