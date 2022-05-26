#include "mqtt_controller_room_manager_master.h"

MQTTClient mqttClient(MQTT_BUFFER_SIZE);   // handles the MQTT communication protocol
WiFiClient networkClient;   // handles the network connection to the MQTT broker

void MQTTSetup(){
   mqttClient.begin(MQTT_BROKERIP, 1883, networkClient);   // setup communication with MQTT broker
   mqttClient.onMessage(mqttMessageReceived);              // callback on message received from MQTT broker
}

void loopMqttClient() {
  mqttClient.loop();
}

void connectToMQTTBroker() {
  if (!mqttClient.connected()) {   // not connected
    Serial.print(F("\nConnecting to MQTT broker..."));
    while (!mqttClient.connect(MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.print(F("."));
      delay(1000);
    }
    Serial.println(F("\nConnected!"));

    // connected to broker, subscribe topics
    mqttClient.subscribe(MQTT_CONFIG_TOPIC);
    Serial.println(F("\nSubscribed to config topic!"));
  }
}

boolean isMQTTBrokerConnected() {
  return mqttClient.connected();
}

int isValidMacAddress(const char* mac) {
    int i = 0;
    int s = 0;

    while (*mac) {
       if (isxdigit(*mac)) {
          i++;
       }
       else if (*mac == ':' || *mac == '-') {

          if (i == 0 || i / 2 - 1 != s)
            break;

          ++s;
       }
       else {
           s = -1;
       }
       ++mac;
    }
    return (i == 12 && (s == 5 || s == 0));
}

void mqttSendConfig(String mac, int roomId, int sensorsId[3]) {
  DynamicJsonDocument doc(2048);

  doc["mac"] = mac;
  doc["room"] = roomId;
  for (int i = 0; i < 3; i++) {
    doc["sensors"][i] = sensorsId[i];
  }
    
  char buffer[128] = {0};
  size_t n = serializeJson(doc, buffer);
  Serial.print(F("JSON message: "));
  Serial.println(buffer);
  mqttClient.publish(MQTT_CONFIG_TOPIC, buffer, n);
}

void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  Serial.println("Incoming MQTT message: " + topic + " - " + payload);
  int roomId = -1;
  
  if (topic == MQTT_CONFIG_TOPIC) {
    Serial.println("MQTT Topic : config");
    // If a new device sent me his ip
    if (isValidMacAddress(&payload[0])) {
      
      int sensorsId[3] = {-1, -1, -1};

      // if device hasn't already a config, create a new config
      if (!getRoomConfig(payload, &roomId, sensorsId)) {
        createRoomConfig(payload);
        createSensorsConfig(payload);
        getRoomConfig(payload, &roomId, sensorsId);
      }

      //subscribe to room's logging queue
      char roomLoggingTopic[128] = {0};
      sprintf(roomLoggingTopic, "%s/%d/logging", MQTT_ROOM_TOPIC, roomId);
      mqttClient.subscribe(roomLoggingTopic);
      Serial.println("\nSubscribed to " + String(roomLoggingTopic) + " topic!"); 

      // update timestamp of tha last heartbeat
      updateLastHBTimestamp(roomId);
      
      // send room config to the slave
      mqttSendConfig(payload, roomId, sensorsId);
    }
  } else {
    if (sscanf(&topic[0], &((MQTT_ROOM_TOPIC+String("/%d/logging"))[0]), &roomId)) {
      Serial.println(roomId);
      Serial.println(F("MQTT NEW ROOM"));
    } else {
      Serial.println(F("MQTT Topic not recognized, message skipped"));
  }
  }
}
