#ifndef DATABASE_CONTROLLER_ROOM_MANAGER

#define DATABASE_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"
#include "secrets.h"

// MySQL libraries
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#endif

boolean setupConfig(int* roomId, int sensorsId[3]);

boolean logSensorMeasure(int sensor, char* value);

boolean logAlarm(char* message, int code, int roomId);

void disconnectMySql();

boolean isMySqlConnected();
