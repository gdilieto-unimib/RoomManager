#ifndef IO_CONTROLLER_ROOM_MANAGER

#define IO_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

void setupIO();

int getLight();

int getTemp();

void updateLight(int lightConfig, int* lightStatus, int lightActivationThreshold);

void setBuzzerAlarm(boolean active);

#endif