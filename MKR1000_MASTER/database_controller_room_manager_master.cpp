#include "database_controller_room_manager_master.h"
#include "wifi_controller_room_manager_master.h"

// MySQL server cfg
char mysql_user[] = MYSQL_USER;       // MySQL user login username
char mysql_password[] = MYSQL_PASS;   // MySQL user login password
IPAddress server_addr(MYSQL_IP);      // IP of the MySQL *server* here
WiFiClient * clientPointer = getClient();
MySQL_Connection conn((Client *)clientPointer);

boolean connectToMySql(){ 
  // connect to MySql if not already connected
  
  if (!conn.connected()) { 
    conn.close(); 
    Serial.println(F("Connecting to MySQL...")); 
    if (conn.connect(server_addr, 3306, mysql_user, mysql_password)) { 
      Serial.println(F("MySQL connection established.")); 
    } else { 
      Serial.println(F("MySQL connection failed.")); 
      return false; 
    } 
  } 
  return true;
}

void disconnectMySql() {
  // disconnect from MySql
  
    conn.close();
}

boolean isMySqlConnected(){
  // check if MySql is connected
  
  return conn.connected();
}

boolean getRoomConfig(String mac, int* roomId, int sensorsId[3]) { 
  // retrieve room and sensor's configuration
  
  if (!connectToMySql()) {
    return false;
  }
  
  char query[128]={0};
  Serial.println(mac); 
  char SELECT_ROOM[] = "SELECT r.id, s.type, s.id FROM gdilieto.room r JOIN gdilieto.sensor s ON r.id = s.room WHERE r.mac='%s'"; 

  sprintf(query, SELECT_ROOM, &mac[0]); 
  Serial.println(query); 

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); 
  cur_mem->execute(query);
  
  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();

  for (int f = 0; f < columns->num_fields; f++) {
      Serial.print(columns->fields[f]->name);
      if (f < columns->num_fields - 1) {
          Serial.print(',');
      } else {
          Serial.println(' ');
      }
  }


  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
        
        *roomId = atoi(row->values[0]);
        
        String sensorType = row->values[1];
        
        if (sensorType.equals("Light")){
          sensorsId[LIGHT_SENSOR]=atoi(row->values[2]);
        }else if (sensorType.equals("Temperature")){
          sensorsId[TEMP_SENSOR]=atoi(row->values[2]);
        }else if (sensorType.equals("Wifi")){
          sensorsId[WIFI_SENSOR]=atoi(row->values[2]);
        }
        
        for (int f = 0; f < columns->num_fields; f++) {
        }
    }
  } while (row != NULL);
  
  delete cur_mem; 

  if (*roomId != -1){
    return true;
  } else {
    return false;
  }
}

boolean getDevices(int* devices) {
  // retrieve number of configured devices
  if (!connectToMySql()) {
    return false;
  }

  char query[128]={0};
  char SELECT_ROOM[] = "SELECT COUNT(*) FROM gdilieto.room"; 

  sprintf(query, SELECT_ROOM); 
  Serial.println(query); 

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); 
  cur_mem->execute(query);
  
  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();

  for (int f = 0; f < columns->num_fields; f++) {
      Serial.print(columns->fields[f]->name);
      if (f < columns->num_fields - 1) {
          Serial.print(',');
      } else {
          Serial.println(' ');
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
  
  char query[128]={0};
  Serial.println(mac); 
  char SELECT_ROOM[] = "SELECT r.id FROM gdilieto.room r WHERE r.mac='%s'"; 

  sprintf(query, SELECT_ROOM, &mac[0]); 
  Serial.println(query); 

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); 
  cur_mem->execute(query);
  
  row_values *row = NULL;
  column_names *columns = cur_mem->get_columns();

  for (int f = 0; f < columns->num_fields; f++) {
      Serial.print(columns->fields[f]->name);
      if (f < columns->num_fields - 1) {
          Serial.print(',');
      } else {
          Serial.println(' ');
      }
  }


  do {
    row = cur_mem->get_next_row();
    if (row != NULL) {
        
        *roomId = atoi(row->values[0]);
        
    }
  } while (row != NULL);
  
  delete cur_mem; 

  if (*roomId != -1){
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
  
  Serial.println(query);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  
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
  
  Serial.println(query);

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
  
  Serial.println(query);

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
  
  Serial.println(query);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  
  delete cur_mem;
  
  return true;
}

boolean updateSensorConfig(int sensorId, String configuration) {
  
  if (!connectToMySql()) {
    return false;
  }
  
  char query[256];
  char UPDATE_SENSOR_CONFIG[] = "UPDATE `gdilieto`.`sensor` SET `auto` = '%d', `active` = '%d' WHERE id = '%d'" ;

  sprintf(query, UPDATE_SENSOR_CONFIG, configuration=="AUTO", configuration=="ON", sensorId);
  
  Serial.println(query);

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
  
  Serial.println(query);

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
  
  Serial.println(query);

  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  cur_mem->execute(query);
  delete cur_mem;
  
  return true;
}
