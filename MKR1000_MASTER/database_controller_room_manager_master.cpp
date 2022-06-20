#include "database_controller_room_manager_master.h"
#include "wifi_controller_room_manager_master.h"
#include "mqtt_controller_room_manager_master.h"


// MySQL server cfg
char mysql_user[] = MYSQL_USER;       // MySQL user login username
char mysql_password[] = MYSQL_PASS;   // MySQL user login password
IPAddress server_addr(MYSQL_IP);      // IP of the MySQL *server* here
WiFiClient * clientPointer = getClient();
MySQL_Connection conn((Client *)clientPointer);

boolean connectToMySql() {
  // connect to MySql if not already connected

  if (!conn.connected()) {
    conn.close();
    if (conn.connect(server_addr, 3306, mysql_user, mysql_password)) {
    } else {
      return false;
    }
  }
  return true;
}

void disconnectMySql() {
  // disconnect from MySql

  conn.close();
}

boolean isMySqlConnected() {
  // check if MySql is connected

  return conn.connected();
}

boolean getRoomConfig(String mac, int* roomId, int sensorsId[3], int actuatorsId[2], boolean* monitoringActivated) {
  // retrieve room and sensor's configuration

  if (!connectToMySql()) {
    return false;
  }

  char query[256] = {0};
  char SELECT_ROOM[] = "SELECT r.id, r.monitoring, s.type, s.id, a.id FROM gdilieto.room r LEFT OUTER JOIN gdilieto.sensor s ON r.id = s.room LEFT OUTER JOIN gdilieto.actuator a ON a.type = s.type AND a.a_room = r.id WHERE r.mac='%s'";

  sprintf(query, SELECT_ROOM, &mac[0]);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();

  // Cicles over columns (not to get the related warning on the serial)
  for (int f = 0; f < columns->num_fields; f++) {
    if (f < columns->num_fields - 1) {
    } else {
    }
  }


  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {

      *roomId = atoi(row->values[0]);
      *monitoringActivated = atoi(row->values[1]) == 0 ? false : true;

      String sensorType = row->values[2];

      if (sensorType.equals("Light")) {
        sensorsId[LIGHT_SENSOR] = atoi(row->values[3]);
        actuatorsId[LIGHT_SENSOR] = atoi(row->values[4]);
      } else if (sensorType.equals("Temperature")) {
        sensorsId[TEMP_SENSOR] = atoi(row->values[3]);
        actuatorsId[TEMP_SENSOR] = atoi(row->values[4]);
      } else if (sensorType.equals("Wifi")) {
        sensorsId[WIFI_SENSOR] = atoi(row->values[3]);
      }

      // Cicles over columns (not to get the related warning on the serial)
      for (int f = 0; f < columns->num_fields; f++) {
      }
    }
  } while (row != NULL);

  delete cur_mem;

  if (*roomId != -1) {
    return true;
  } else {
    return false;
  }
}

boolean getConfiguration(boolean* singleMode, boolean* ecoMode, String * schedule, boolean * sleepMode) {
  // retrieve number of configured devices
  if (!connectToMySql()) {
    return false;
  }

  char query[128] = {0};
  char SELECT_ROOM[] = "SELECT c.name, c.value FROM gdilieto.configuration c";

  sprintf(query, SELECT_ROOM);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();
  
  // Cicles over columns (not to get the related warning on the serial)
  for (int f = 0; f < columns->num_fields; f++) {
    if (f < columns->num_fields - 1) {
    } else {
    }
  }


  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
      if (String(row->values[0]).equals("singleMode")) {
        *singleMode = String(row->values[1]).equals("0") ? false : true;
      } else if (String(row->values[0]).equals("ecoMode")) {
        boolean newValue = String(row->values[1]).equals("0") ? false : true;
        if (newValue != *ecoMode) {
          mqttSendEcoMode(newValue);
        }
        *ecoMode = newValue;
      } else if (String(row->values[0]).equals("sleepSchedule")) {
        *schedule = String(row->values[1]);
      } else if (String(row->values[0]).equals("sleepMode")) {
        *sleepMode = String(row->values[1]).equals("0") ? false : true;

      }

    }
  } while (row != NULL);

  delete cur_mem;

}

boolean getDevices(int* devices) {
  // retrieve number of configured devices
  if (!connectToMySql()) {
    return false;
  }

  char query[128] = {0};
  char SELECT_ROOM[] = "SELECT COUNT(*) FROM gdilieto.room";

  sprintf(query, SELECT_ROOM);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();

  // Cicles over columns (not to get the related warning on the serial)
  for (int f = 0; f < columns->num_fields; f++) {
    if (f < columns->num_fields - 1) {
    } else {
    }
  }


  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
      *devices = atoi(row->values[0]);

    }
  } while (row != NULL);

  delete cur_mem;

}


boolean getRoomId(String mac, int* roomId) {
  // retrieve room and sensor's configuration

  if (!connectToMySql()) {
    return false;
  }

  char query[128] = {0};
  char SELECT_ROOM[] = "SELECT r.id FROM gdilieto.room r WHERE r.mac='%s'";

  sprintf(query, SELECT_ROOM, &mac[0]);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();

  // Cicles over columns (not to get the related warning on the serial)
  for (int f = 0; f < columns->num_fields; f++) {
    if (f < columns->num_fields - 1) {
    } else {
    }
  }

  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {

      *roomId = atoi(row->values[0]);

    }
  } while (row != NULL);

  delete cur_mem;

  if (*roomId != -1) {
    return true;
  } else {
    return false;
  }
}

boolean createRoomConfig(String mac) {
  if (!connectToMySql()) {
    return false;
  }

  char query[128];
  char INSERT_ROOM[] = "INSERT INTO `gdilieto`.`room` (`mac`) VALUES ('%s')";

  sprintf(query, INSERT_ROOM, &mac[0]);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  delete cur_mem;

  return true;
}

boolean getRoomsAndActuatorsId(int roomsId[MAX_ROOMS_NUMBER], int actuatorsId[MAX_ROOMS_NUMBER][2]) {
  // Gets id of all rooms and relative sensors
  
  if (!connectToMySql()) {
    return false;
  }

  char query[256];
  char GET_ROOMS_ACTUATORS[] = "SELECT r.id, a.id FROM `gdilieto`.`room` r LEFT OUTER JOIN `gdilieto`.`actuator` a ON a.a_room = r.id ORDER BY r.id";

  sprintf(query, GET_ROOMS_ACTUATORS);
  Serial.println(query);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  
  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();

  // Cicles over columns (not to get the related warning on the serial) 
  for (int f = 0; f < columns->num_fields; f++) {
    if (f < columns->num_fields - 1) {
    } else {
    }
  }

  int roomIndex = 0 , actuatorIndex = 0;

  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {

      if(roomIndex==0 || atoi(row->values[0]) != roomsId[roomIndex-1]) {
        roomsId[roomIndex] = atoi(row->values[0]);
        roomIndex++;
        actuatorIndex=0;
      }
      if(atoi(row->values[1])!=0) {
        actuatorsId[roomIndex-1][actuatorIndex] = atoi(row->values[1]);
        Serial.println(roomsId[roomIndex-1]);
        Serial.println(actuatorsId[roomsId[roomIndex-1]][actuatorIndex]);
        actuatorIndex++;  
      }

    }
  } while (row != NULL);

  
  delete cur_mem;

  return true;
}

boolean createSensorsConfig(String mac) {
  if (!connectToMySql()) {
    return false;
  }

  char query[256];
  char INSERT_ROOM[] = "INSERT INTO `gdilieto`.`sensor` (`room`, `type`, `name`) VALUES ('%d', 'Light', 'Light'), ('%d', 'Temperature', 'Temperature'), ('%d', 'Wifi', 'Wifi')";

  int roomId = -1;
  getRoomId(mac, &roomId);

  sprintf(query, INSERT_ROOM, roomId, roomId, roomId);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  delete cur_mem;

  return true;
}

boolean createActuatorsConfig(String mac) {
  if (!connectToMySql()) {
    return false;
  }

  char query[256];
  char INSERT_ROOM[] = "INSERT INTO `gdilieto`.`actuator` (`a_room`, `type`, `name`) VALUES ('%d', 'Light', 'Light'), ('%d', 'Temperature', 'Temperature')";

  int roomId = -1;
  getRoomId(mac, &roomId);

  sprintf(query, INSERT_ROOM, roomId, roomId);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  delete cur_mem;

  return true;
}

boolean updateLastHBTimestamp(int roomId) {
  if (!connectToMySql()) {
    return false;
  }

  char query[256];
  char UPDATE_HB_ROOM[] = "UPDATE `gdilieto`.`room` SET `lastHB` = CURRENT_TIMESTAMP WHERE id='%d'";

  sprintf(query, UPDATE_HB_ROOM, roomId);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  delete cur_mem;

  return true;
}

boolean updateLastHBTimestamp(String mac) {
  if (!connectToMySql()) {
    return false;
  }

  char query[256];
  char UPDATE_HB_ROOM[] = "UPDATE `gdilieto`.`room` SET `lastHB` = CURRENT_TIMESTAMP WHERE mac='%s'";

  sprintf(query, UPDATE_HB_ROOM, &mac[0]);


  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  delete cur_mem;

  return true;
}

boolean updateRoomMonitoring(int roomId, boolean monitoring) {
  if (!connectToMySql()) {
    return false;
  }

  char query[256];
  char UPDATE_ROOM_MONITORING[] = "UPDATE `gdilieto`.`room` SET `monitoring` = '%d' WHERE id = '%d'" ;

  sprintf(query, UPDATE_ROOM_MONITORING, monitoring, roomId);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  delete cur_mem;

  return true;
}

boolean updateActuatorConfig(int actuatorId, String configuration) {
  if (!connectToMySql()) {
    return false;
  }

  char query[256];
  char UPDATE_SENSOR_CONFIG[] = "UPDATE `gdilieto`.`actuator` SET `config` = '%d' WHERE id = '%d'" ;

  sprintf(query, UPDATE_SENSOR_CONFIG, configuration == "AUTO"?2:configuration == "ON"?1:0, actuatorId);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);

  delete cur_mem;

  return true;
}

boolean logSensorMeasure(int sensor, char* value) {
  // log sensor's measure

  if (!connectToMySql()) {
    return false;
  }

  char query[128];
  char INSERT_MEASURE[] = "INSERT INTO `gdilieto`.`measure` (`sensor`, `value`) VALUES ('%d', '%s')";

  sprintf(query, INSERT_MEASURE, sensor, value);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;

  return true;
}

boolean logAlarm(char* message, int code, int roomId) {
  // log alarm

  if (!connectToMySql()) {
    return false;
  }

  char query[128];
  char INSERT_ALARM[] = "INSERT INTO `gdilieto`.`alarm` (`message`, `code`, `room_alert`) VALUES ('%s', '%d', '%d')";

  sprintf(query, INSERT_ALARM, message, code, roomId);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;

  return true;
}
