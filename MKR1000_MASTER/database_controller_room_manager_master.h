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