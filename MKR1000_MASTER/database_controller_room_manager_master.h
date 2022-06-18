#ifndef DATABASE_CONTROLLER_ROOM_MANAGER

#define DATABASE_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager_master.h"
#include "secrets_master.h"

// MySQL libraries
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

#endif

boolean connectToMySql();

void disconnectMySql();

boolean isMySqlConnected();

boolean getDevices(int* devices);

boolean getConfiguration(boolean* singleMode, boolean* ecoMode, String * schedule, boolean * sleepMode);

boolean getRoomConfig(String mac, int* roomId, int sensorsId[3], boolean* monitoringActivated);

boolean getRoomsAndActuatorsId(int roomsId[MAX_ROOMS_NUMBER], int actuatorsId[MAX_ROOMS_NUMBER*2]);

boolean createRoomConfig(String mac);

boolean createSensorsConfig(String mac);

boolean updateLastHBTimestamp(int roomId);

boolean updateLastHBTimestamp(String mac);

boolean updateRoomMonitoring(int roomId, boolean monitoring);

boolean updateSensorConfig(int sensorId, String configuration);

boolean logSensorMeasure(int sensor, char* value);

boolean logAlarm(char* message, int code, int roomId);
