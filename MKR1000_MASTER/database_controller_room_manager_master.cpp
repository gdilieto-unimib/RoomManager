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
