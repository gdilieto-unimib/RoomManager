#include "database_controller_room_manager.h"
#include "wifi_controller_room_manager.h"

// MySQL server cfg
char mysql_user[] = MYSQL_USER;       // MySQL user login username
char mysql_password[] = MYSQL_PASS;   // MySQL user login password
IPAddress server_addr(MYSQL_IP);      // IP of the MySQL *server* here
WiFiClient * clientPointer = getClient();
MySQL_Connection conn((Client *)clientPointer);

boolean connectToMySql(){ 
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


boolean setSensorsId(int roomId, int sensorsId[3]) {

  if (!connectToMySql()) {
    return false;
  }

  char query[256]={0};
  char SELECT_SENSORS[] = "SELECT * FROM gdilieto.sensor WHERE room = '32' ORDER BY type ASC"; 
  //sprintf(query, SELECT_SENSORS, roomId);
  Serial.println(query); 
  
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); 
  cur_mem->execute(SELECT_SENSORS);
  
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
          String sensorType = row->values[1];
          Serial.println(sensorType);
          if (sensorType.equals("Light")){
            sensorsId[LIGHT_SENSOR]=atoi(row->values[0]);
          }else if (sensorType.equals("Temperature")){
            sensorsId[TEMP_SENSOR]=atoi(row->values[0]);
          }else if (sensorType.equals("Wifi")){
            sensorsId[WIFI_SENSOR]=atoi(row->values[0]);
          }
          
          for (int f = 0; f < columns->num_fields; f++) {
              String batchnumber_str = "";
              batchnumber_str = String(row->values[f]);
              if (f < columns->num_fields - 1) {
              }
          }
      }
  } while (row != NULL);
    /*do {
        row = cur_mem->get_next_row();
        
        if (row != NULL) {
            for (int f = 0; f < columns->num_fields; f++)
            {
                String batchnumber_str = "";
                batchnumber_str = String(row->values[f]);
                Serial.print("batchnumber_str: ");
                Serial.println(batchnumber_str);
                if (f < columns->num_fields - 1)
                {
                    Serial.print(',');
                }
            }
            Serial.println();
        }
    } while (row != NULL);
    */
  delete cur_mem; 

  return true;
}

boolean setupConfig(int* roomId, int sensorsId[3]) { 
  if (!connectToMySql()) {
    return false;
  }

  int ip[] = IP;
  char ipv4[15] = {0};
  sprintf(ipv4, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  char query[128]={0};
  char SELECT_ROOM[] = "SELECT r.id, s.type, s.id FROM gdilieto.room r JOIN gdilieto.sensor s ON r.id = s.room WHERE r.ipv4='%s'"; 

  sprintf(query, SELECT_ROOM, ipv4); 
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

  return true;
}

boolean logSensorMeasure(int sensor, char* value) { 
  Serial.println(value);
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
