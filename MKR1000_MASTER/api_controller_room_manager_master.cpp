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
  client.println("Access-Control-Allow-Methods: OPTIONS, GET, POST");
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

void listenForClients() {
  // listen for http requests from a webapp

  WiFiClient client = apiServer.available(); // listen for incoming clients

  if (client) { // if you get a client,
    boolean optionsSent = false;
    Serial.println("new client"); // print a message out the serial port
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
        int sensorId = -1;

        // Check to see which client request was sent

        if (currentLine.startsWith("OPTIONS") && !optionsSent) {
          optionsSent = true;
          /* client.println("HTTP/1.1 200 OK");
            client.println("Content-type: application/json");
                        client.println("Access-Control-Allow-Origin: *");

            client.println("Allow: OPTIONS, GET, HEAD, POST");
            client.println();  */

          printHeaders(client);
        }

        if (currentLine.startsWith("POST") && currentLine.endsWith("control")) {
          sscanf( & currentLine[0], "POST /sensors/%d/control  ", & sensorId);
          String postContent = getPostContent(client);
          DynamicJsonDocument doc(HTTP_BUFFER_SIZE);
          deserializeJson(doc, postContent);
          Serial.println("DESERIALIZATION: " + String(doc["config"].as < int > ()));

          switch (doc["config"].as < int > ()) {
            case 0: {
                mqttSendSensorControl(sensorId, "OFF"); // GET /sensors/:id/on turns on the sensor's actuator activation
                updateSensorConfig(sensorId, "OFF");
                break;
              }
            case 1: {
                mqttSendSensorControl(sensorId, "ON"); // GET /sensors/:id/on turns on the sensor's actuator activation
                updateSensorConfig(sensorId, "ON");
                break;
              }
            case 2: {
                mqttSendSensorControl(sensorId, "AUTO"); // GET /sensors/:id/on turns on the sensor's actuator activation
                updateSensorConfig(sensorId, "AUTO");
                break;
              }
          }

          printHeaders(client);
          client.println("{}");
          client.println();
          break;
        } else if (currentLine.startsWith("POST") && currentLine.endsWith("monitoring")) {
          sscanf( & currentLine[0], "POST /rooms/%d/monitoring", & roomId);
          String postContent = getPostContent(client);
          DynamicJsonDocument doc(HTTP_BUFFER_SIZE);
          deserializeJson(doc, postContent);
          Serial.println("DESERIALIZATION: " + String(doc["config"].as < int > ()));
          switch (doc["config"].as < int > ()) {
            case 0: {
                printHeaders(client); // Print response headers
                mqttSendMonitoringControl(roomId, "STOP"); // GET /rooms/:id/monitoring/stop stops the room's monitoring
                updateRoomMonitoring(roomId, 0);
                client.print("{\"monitoringActivated\": false}");
                client.println();
                break;
              }
            case 1: {
                printHeaders(client); // Print response headers
                mqttSendMonitoringControl(roomId, "START"); // GET /rooms/:id/monitoring/start starts the room's monitoring
                updateRoomMonitoring(roomId, 1);
                client.print("{\"monitoringActivated\": true}");
                client.println();
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
    Serial.println("client disonnected");
  }
}
