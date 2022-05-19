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

MQTTClient getMqttClient(){
  return mqttClient;
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

// Function to validate an IP address
bool validateIP(String ip) {
    char* ipCharArr = &ip[0];
    int segs = 0;   /* Segment count. */
    int chcnt = 0;  /* Character count within segment. */
    int accum = 0;  /* Accumulator for segment. */

    /* Catch NULL pointer. */

    if (ipCharArr == NULL)
        return false;

    /* Process every character in string. */

    while (*ipCharArr != '\0') {
        /* Segment changeover. */

        if (*ipCharArr == '.') {
            /* Must have some digits in segment. */

            if (chcnt == 0)
                return false;

            /* Limit number of segments. */

            if (++segs == 4)
                return false;

            /* Reset segment values and restart loop. */

            chcnt = accum = 0;
            ipCharArr++;
            continue;
        }
        /* Check numeric. */

        if ((*ipCharArr < '0') || (*ipCharArr > '9'))
            return false;

        /* Accumulate and check segment. */

        if ((accum = accum * 10 + *ipCharArr - '0') > 255)
            return false;

        /* Advance other segment specific stuff and continue loop. */

        chcnt++;
        ipCharArr++;
    }

    /* Check enough segments and enough characters in last segment. */

    if (segs != 3)
        return 0;

    if (chcnt == 0)
        return 0;

    /* Address okay. */

    return true;
}

void mqttSendConfig(String ip, int roomId, int sensorsId[3]) {

  const int capacity = JSON_OBJECT_SIZE(3);
  StaticJsonDocument<capacity> doc;

  doc["ip"] = ip;
  //doc["room"] = roomId;
  //doc["sensors"] = sensorsId;
  
  char buffer[128];
  size_t n = serializeJson(doc, buffer);
  Serial.print(F("JSON message: "));
  Serial.println(buffer);
  getMqttClient().publish(MQTT_CONFIG_TOPIC, buffer, n);
}

void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  Serial.println("Incoming MQTT message: " + topic + " - " + payload);

  if (topic == MQTT_CONFIG_TOPIC) {
    Serial.println("MQTT Topic : config");
    // If a new device sent me his ip
    if (validateIP(payload)) {
      
      int roomId = -1;
      int sensorsId[3] = {-1, -1, -1};

      // if device hasn't already a config, create a new config
      if (!getRoomConfig(payload, &roomId, sensorsId)) {
        createRoomConfig(payload, &roomId, sensorsId);
        getRoomConfig(payload, &roomId, sensorsId);
      }
      
      mqttSendConfig(payload, roomId, sensorsId);
    }
  } else {
    Serial.println(F("MQTT Topic not recognized, message skipped"));
  }
}
