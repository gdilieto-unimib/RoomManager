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
/*
boolean getConfig(int* roomId, int sensorsId[3]) { 
  // retrieve room and sensor's configuration
  
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

  if (*roomId != -1){
    return true;
  } else {
    return false;
  }
}
*/
