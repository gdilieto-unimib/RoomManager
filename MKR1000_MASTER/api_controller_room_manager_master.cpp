#include "api_controller_room_manager_master.h"


WiFiServer apiServer(80);

boolean * ecoModeRef;

void setupApiServer(boolean * ecoMode) {
  ecoModeRef = ecoMode;
  apiServer.begin();
}

void printHeaders(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:application/json");
  client.println("Access-Control-Max-Age:600");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Access-Control-Allow-Credentials: true");
  client.println("Access-Control-Allow-Methods: OPTIONS, GET, PUT");
  client.println("Access-Control-Allow-Headers: Content-Type, Depth, User-Agent, X-File-Size, X-Requested-With, If-Modified-Since, X-File-Name, Cache-Control");
  client.println();
}

boolean printErrorHeaders(WiFiClient client) {
  client.println("HTTP/1.1 500");
  client.println("Content-type:application/json");
  client.println("Access-Control-Allow-Origin: *");
  client.println("Access-Control-Allow-Credentials: true");
  client.println("Access-Control-Allow-Methods: OPTIONS, GET, PUT");
  client.println("Access-Control-Allow-Headers: Content-Type, Depth, User-Agent, X-File-Size, X-Requested-With, If-Modified-Since, X-File-Name, Cache-Control");
  client.println();
}

String getPostContent(WiFiClient client) {
  boolean startConcat = false;
  String postContent = "";
  int count = 0;
  while (client.connected()) {
    String currentLine = ""; // make a String to hold incoming data from the client
    if (client.available()) { // if there's bytes to read from the client,
      char c = client.read(); // read a byte, then
      //Serial.write(c);
      if (c == '{') {
        count++;
        startConcat = true;
      }
      if (startConcat) {
        postContent = postContent + String(c);
      }
      if (c == '}') {
        count--;
      }
      if (count == 0 && startConcat) {
        startConcat = false;

        return postContent;
      }
    }
  }
}

String getErrorJson(int errorCode, String errorMessage) {
  return "{\"errorCode\":"+String(errorCode)+", \"errorMessage\":\""+errorMessage+ "\"}";
}

boolean sendError(boolean sqlE, boolean mqttE, WiFiClient client) {
  
  if (sqlE || mqttE) {
    printErrorHeaders(client);

    client.println("[");
    if (sqlE) {
      client.print(getErrorJson(0, "cannot connect to database"));
      client.println(mqttE?",":"");

    }
    if (mqttE) {
      client.println(getErrorJson(1, "cannot connect to mqtt broker"));
    }
    client.println("]");
    client.println();
    return true;
  }
  return false;
  
}

void listenForClients() {

  // initialization of possible internal errors
  boolean mqttError = false;
  boolean sqlError = false;
  
  // listen for http requests from a webapp
  WiFiClient client = apiServer.available(); // listen for incoming clients

  if (client) { // if you get a client,
    boolean optionsSent = false;
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        //Serial.write(c); // print it out the serial monitor
        if (c == '\n' && currentLine.length() == 0) { // if the byte is a newline character
          break;
        } else if (c == '\r') { // if you got a newline, then clear currentLine:
          currentLine = "";
        } else if (c != '\r') { // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }

        int roomId = -1;
        int actuatorId = -1;

        // Check to see which client request was sent

        if (currentLine.startsWith("OPTIONS") && !optionsSent) {
          optionsSent = true;
          printHeaders(client);
        }

        if (currentLine.startsWith("PUT") && currentLine.endsWith("config")) {
          sscanf( & currentLine[0], "PUT /rooms/%d/actuators/%d/config", &roomId, &actuatorId);
          String postContent = getPostContent(client);
          DynamicJsonDocument doc(HTTP_BUFFER_SIZE);
          deserializeJson(doc, postContent);
          mqttError = !isMQTTBrokerConnected();

          switch (doc["config"].as < int > ()) {
            case 0: {
                mqttSendActuatorControl(actuatorId, "OFF"); // PUT rooms/:roomId/actuator/:actuatorId/on turns off the actuator activation
                sqlError = ! updateActuatorConfig(actuatorId, "OFF");
                break;
              }
            case 1: {
                mqttSendActuatorControl(actuatorId, "ON"); // PUT rooms/:roomId/actuator/:actuatorId/on turns on the actuator activation
                sqlError = ! updateActuatorConfig(actuatorId, "ON");
                break;
              }
            case 2: {
                mqttSendActuatorControl(actuatorId, "AUTO"); // PUT rooms/:roomId/actuator/:actuatorId/auto turns to auto the actuator activation
                sqlError = ! updateActuatorConfig(actuatorId, "AUTO");
                break;
              }
          }
          if (!sendError(sqlError, mqttError, client)) {
            printHeaders(client);
            client.println("{\"config\":"+String(doc["config"].as<int>())+"}");
            client.println();
          }
          break;
        } else if (currentLine.startsWith("PUT") && currentLine.endsWith("monitoring")) {
          sscanf( & currentLine[0], "PUT /rooms/%d/monitoring", &roomId);
          String postContent = getPostContent(client);
          DynamicJsonDocument doc(HTTP_BUFFER_SIZE);
          deserializeJson(doc, postContent);
          switch (doc["config"].as < int > ()) {
            case 0: {
                mqttSendMonitoringControl(roomId, "STOP"); // GET /rooms/:id/monitoring/stop stops the room's monitoring
                sqlError = ! updateRoomMonitoring(roomId, 0);
                if (!sendError(sqlError, mqttError, client)) {
                  printHeaders(client); // Print response headers
                  client.print("{\"monitoringActivated\": false}");
                  client.println();
                }
                break;
              }
            case 1: {
                mqttSendMonitoringControl(roomId, "START"); // GET /rooms/:id/monitoring/start starts the room's monitoring
                sqlError = ! updateRoomMonitoring(roomId, 1);
                if (!sendError(sqlError, mqttError, client)) {
                  printHeaders(client); // Print response headers
                  client.print("{\"monitoringActivated\": true}");
                  client.println();
                }
                break;
              }
          }
          break;
        }

      } else {
        client = apiServer.available();
      }
    }
    // close the connection:
    client.stop();
  }
}
