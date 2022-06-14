#include "mqtt_controller_room_manager_master.h"
#include "time_controller_room_manager_master.h"


boolean* ecoModeR;
int* externalTemperatureR;
boolean *thisHourSleepModeR;

MQTTClient mqttClient(MQTT_BUFFER_SIZE);   // handles the MQTT communication protocol
WiFiClient networkClient;   // handles the network connection to the MQTT broker

void MQTTSetup(int* externalTemperature, boolean* ecoMode ,boolean *  thisHourSleepMode){
   mqttClient.begin(MQTT_BROKERIP, 1883, networkClient);   // setup communication with MQTT broker
   mqttClient.onMessage(mqttMessageReceived);              // callback on message received from MQTT broker
   externalTemperatureR = externalTemperature;
   ecoModeR = ecoMode;
   thisHourSleepModeR = thisHourSleepMode;
}

void loopMqttClient() {
  if(isMQTTBrokerConnected()) {
    mqttClient.loop();  
  }
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
    mqttClient.subscribe(MQTT_HEARTBEAT_TOPIC);
    Serial.println(F("\nSubscribed to heartbeat topic!"));
  }
}

void disconnectMQTTBroker() {
  mqttClient.disconnect();
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

void mqttSendConfig(String mac, int roomId, int sensorsId[3], boolean monitoringActivated) {
  DynamicJsonDocument doc(MQTT_BUFFER_SIZE);
  
  doc["mac"] = mac;
  doc["monitoring"] = monitoringActivated;
  doc["room"] = roomId;
  doc["externalTemp"] = *externalTemperatureR;
  doc["ecoMode"] = *ecoModeR;
  doc["sleepDuration"] = *thisHourSleepModeR?60-getTimeMinute()*3600000:0;

  for (int i = 0; i < 3; i++) {
    doc["sensors"][i] = sensorsId[i];
  }
    
  char buffer[MQTT_BUFFER_SIZE] = {0};
  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(MQTT_CONFIG_TOPIC, buffer, n);
}

void mqttSendMonitoringControl(int roomId, String control) {
  mqttClient.publish(String(MQTT_ROOM_TOPIC)+"/"+String(roomId)+ "/monitoring/control", control);
}

void mqttSendSensorControl(int sensorId, String control) {
  mqttClient.publish(String(MQTT_SENSOR_TOPIC)+"/"+String(sensorId)+ "/control", control);
}

void mqttSendExternalTemperature(int temperature) {
  mqttClient.publish(String(MQTT_ROOM_TOPIC)+"/externalTemperature", String(temperature));
}

void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  //Serial.println("Incoming MQTT message: " + topic + " - " + payload);
  int roomId = -1;
  int sensorId = -1;
  
  if (topic == MQTT_HEARTBEAT_TOPIC) {
    // If a new device sent me his mac
    
    int sensorsId[3] = {-1, -1, -1};
    boolean monitoringActivated;

    // if device hasn't already a config, create a new config
    if (!getRoomConfig(payload, &roomId, sensorsId, &monitoringActivated)) {
      createRoomConfig(payload);
      createSensorsConfig(payload);
      getRoomConfig(payload, &roomId, sensorsId, &monitoringActivated);
    }

    //subscribe to room's logging queue
    mqttClient.subscribe(String(MQTT_ROOM_TOPIC)+"/"+String(roomId)+"/logging");

    //subscribe to room's monitoring queue
    mqttClient.subscribe(String(MQTT_ROOM_TOPIC)+"/"+String(roomId)+"/alarm");

    //subscribe to room's monitoring queue
    mqttClient.subscribe(String(MQTT_ROOM_TOPIC)+"/"+String(roomId)+"/monitoring/control");
    
    //subscribe to sensor's monitoring queue
    for (int i=0; i<3; i++) {
      mqttClient.subscribe(String(MQTT_SENSOR_TOPIC)+"/"+String(sensorsId[i])+ "/control");
    }

    // update timestamp of tha last heartbeat
    updateLastHBTimestamp(roomId);
    
    // send room config to the slave
    mqttSendConfig(payload, roomId, sensorsId, monitoringActivated);
  
  } else {
    
    if (topic.endsWith("/alarm")) {
      // if topic is room's logging
    
      sscanf(&topic[0], &((MQTT_ROOM_TOPIC+String("/%d/alarm"))[0]), &roomId);
      
      DynamicJsonDocument doc(MQTT_BUFFER_SIZE);
      deserializeJson(doc, payload);

      logAlarm(&(doc["message"].as<String>())[0], doc["code"].as<int>(), roomId);
    
    }
    if (topic.endsWith("/logging")) {
      // if topic is room's logging
    
      sscanf(&topic[0], &((MQTT_ROOM_TOPIC+String("/%d/logging"))[0]), &roomId);
      
      DynamicJsonDocument doc(MQTT_BUFFER_SIZE);
      deserializeJson(doc, payload);

      for (JsonPair keyValue : doc.as<JsonObject>()) {
        Serial.println("SENSOR");
        Serial.println(keyValue.key().c_str());
        Serial.println(keyValue.value().as<String>());
        logSensorMeasure(atoi(keyValue.key().c_str()), &(keyValue.value().as<String>())[0]);
      }
    
    } else if (topic.endsWith("/monitoring/control")) {
      // if topic is room's monitoring control
      
      sscanf(&topic[0], &((MQTT_ROOM_TOPIC+String("/%d/monitoring/control"))[0]), &roomId);
      updateRoomMonitoring(roomId, payload=="START"?true:false);
 
    } else if (topic.endsWith("/control")) { 
      // if topic is room's sensor's control

      sscanf(&topic[0], &((MQTT_SENSOR_TOPIC+String("/%d/control"))[0]), &sensorId);
      updateSensorConfig(sensorId, payload);
      
    } else {
      // no topic matched
      
      Serial.println(F("MQTT Topic not recognized, message skipped"));
    }
    
  }
}
