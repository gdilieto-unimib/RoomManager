#include "api_controller_room_manager_master.h"

WiFiServer apiServer(80);

void setupApiServer() {
  apiServer.begin();
}

void printHeaders(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Access-Control-Allow-Credentials: true");
  client.println("Access-Control-Allow-Methods: OPTIONS, GET, POST");
  client.println("Access-Control-Allow-Headers: Content-Type, Depth, User-Agent, X-File-Size, X-Requested-With, If-Modified-Since, X-File-Name, Cache-Control");
  client.println();
}

void listenForClients() {
  // listen for http requests from a webapp
  
 WiFiClient client = apiServer.available();   // listen for incoming clients
    
 if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n' && currentLine.length() == 0) {                    // if the byte is a newline character
          break;
        }
        else if (c == '\r'){      // if you got a newline, then clear currentLine:
          currentLine = "";
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
        
        int roomId = -1;
        int sensorId = -1;
        
        // Check to see which client request was sent
        if (currentLine.endsWith("/monitoring/start")) {
          sscanf(&currentLine[0], "GET /rooms/%d/monitoring/start", &roomId);
          printHeaders(client);   // Print response headers
          mqttSendMonitoringControl(roomId, "START");    // GET /rooms/:id/monitoring/start starts the room's monitoring
          updateRoomMonitoring(roomId, 1);
          client.print("{\"monitoringActivated\": true}");
          client.println();
        }
        else if (currentLine.endsWith("/monitoring/stop")) {
          sscanf(&currentLine[0], "GET /rooms/%d/monitoring/stop", &roomId);
          printHeaders(client);   // Print response headers
          mqttSendMonitoringControl(roomId, "STOP");    // GET /rooms/:id/monitoring/stop stops the room's monitoring
          updateRoomMonitoring(roomId, 0);
          client.print("{\"monitoringActivated\": false}");
          client.println();
        }
        else if (currentLine.endsWith("/off")) {
          sscanf(&currentLine[0], "GET /sensors/%d/control/off  ", &sensorId);
          printHeaders(client);   // Print response headers
          mqttSendSensorControl(sensorId, "OFF");    // GET /sensors/:id/off turns off the sensor's actuator activation
          updateSensorConfig(sensorId, "OFF");
          client.print("{"+String(sensorId)+": \"off\"}");
          client.println();
        }
        else if (currentLine.endsWith("/on")) {
          sscanf(&currentLine[0], "GET /sensors/%d/control/on", &sensorId);
          printHeaders(client);   // Print response headers
          mqttSendSensorControl(sensorId, "ON");    // GET /sensors/:id/on turns on the sensor's actuator activation
          updateSensorConfig(sensorId, "ON");
          client.print("{"+String(sensorId)+": \"on\"}");
          client.println();
        }
        else if (currentLine.endsWith("/auto")) {
          sscanf(&currentLine[0], "GET /sensors/%d/control/auto", &sensorId);
          printHeaders(client);   // Print response headers
          mqttSendSensorControl(sensorId, "AUTO");    // GET /sensors/:id/off turns off the sensor's actuator activation
          updateSensorConfig(sensorId, "AUTO");
          client.print("{"+String(sensorId)+": \"auto\"}");
          client.println();
        }
      } else {
        client = apiServer.available();
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}
