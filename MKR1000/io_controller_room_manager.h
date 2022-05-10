#ifndef IO_CONTROLLER_ROOM_MANAGER

#define IO_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"
#include "wifi_controller_room_manager.h"

void setupIO();

int getWifiRssi();

int getLight();

int getTemp();

void updateLight(int light, int lightConfig, int* lightStatus, int lightActivationThreshold);

void updateTemp(int temp, int tempConfig, int* tempStatus, int tempActivationThreshold);

void setBuzzerAlarm(boolean active);

#endif
