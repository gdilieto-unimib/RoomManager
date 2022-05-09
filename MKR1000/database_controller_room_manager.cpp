#include "database_controller_room_manager.h"
#include "wifi_controller_room_manager.h"




// MySQL server cfg
char mysql_user[] = MYSQL_USER;       // MySQL user login username
char mysql_password[] = MYSQL_PASS;   // MySQL user login password
IPAddress server_addr(MYSQL_IP);      // IP of the MySQL *server* here
WiFiClient * clientPointer = getClient();
MySQL_Connection conn((Client *)clientPointer);

char query[128];

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

boolean setupConfig(int* roomId, int sensorsId[]) { 
  char SELECT_ROOM[] = "SELECT * FROM gdilieto.room WHERE ipv4='%s'"; 
   
  char ipv4[15] = {0};
  int ip[] = IP;  
  sprintf(ipv4, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

  if (!connectToMySql()) {
    return false;
  }
 
  // log data 
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn); 
  sprintf(query, SELECT_ROOM, ipv4); 
  Serial.println(query); 
  // execute the query 
  cur_mem->execute(query); 
  // Note: since there are no results, we do not need to read any data 
  // deleting the cursor also frees up memory used 
   
  delete cur_mem; 

  return true;
}
