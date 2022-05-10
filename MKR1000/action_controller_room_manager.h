#ifndef ACTION_CONTROLLER_ROOM_MANAGER

#define ACTION_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

void action(int screen, int pressedButton, int* displayRow, int* tempActivationThreshold, int* lightActivationThreshold, int* tempConfig, int* lightConfig, boolean* navigationMode, boolean* fireAlarm);

#endif
