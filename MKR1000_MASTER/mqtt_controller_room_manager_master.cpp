#include "mqtt_controller_room_manager_master.h"
#include "time_controller_room_manager_master.h"
#include "rgb_lcd_controller_room_manager_master.h"

boolean* ecoModeR;
boolean *sleepModeR;
int* externalTemperatureR;

boolean initialConfiguration = false;
int configuredRoomsId[MAX_ROOMS_NUMBER] = {};
int configuredActuatorsId[MAX_ROOMS_NUMBER * 2] = {};

MQTTClient mqttClient(MQTT_BUFFER_SIZE);   // handles the MQTT communication protocol
WiFiClient networkClient;   // handles the network connection to the MQTT broker

void MQTTSetup(int* externalTemperature, boolean* ecoMode , boolean* sleepMode) {
  mqttClient.begin(MQTT_BROKERIP, 1883, networkClient);   // setup communication with MQTT broker
  mqttClient.onMessage(mqttMessageReceived);              // callback on message received from MQTT broker
  externalTemperatureR = externalTemperature;
  ecoModeR = ecoMode;
  sleepModeR = sleepMode;
}

void loopMqttClient() {
  if (isMQTTBrokerConnected()) {
    mqttClient.loop();
  }
}

int subscribeToRoomQueues(int roomId) {

  //subscribe to room's logging queue
  mqttClient.subscribe(String(MQTT_ROOM_TOPIC) + "/" + String(roomId) + "/sensors/measures");

  //subscribe to room's monitoring queue
  mqttClient.subscribe(String(MQTT_ROOM_TOPIC) + "/" + String(roomId) + "/alarm");

  //subscribe to room's monitoring queue
  mqttClient.subscribe(String(MQTT_ROOM_TOPIC) + "/" + String(roomId) + "/monitoring/control");

}

int subscribeToActuatorQueues(int sensorId) {

  //subscribe to sensor's control queue
  mqttClient.subscribe(String(MQTT_ACTUATORS_TOPIC) + "/" + String(sensorId) + "/control");

}

int subscribeToConfiguredRoomsAndActuators() {

  Serial.println("INITIAL CONFIG");
  //subscribe to configured rooms queues
  for (int i = 0; i < MAX_ROOMS_NUMBER && configuredRoomsId[i] != 0; i++) {
    Serial.println(configuredRoomsId[i]);
    subscribeToRoomQueues(configuredRoomsId[i]);
  }

  //subscribe to configured actuators queues
  for (int i = 0; i < MAX_ROOMS_NUMBER * 2 && configuredActuatorsId[i] != 0; i++) {
    Serial.println(configuredActuatorsId[i]);
    subscribeToActuatorQueues(configuredActuatorsId[i]);
  }
}

void connectToMQTTBroker() {
  if (!mqttClient.connected()) {   // not connected
    Serial.println("Trying mqtt connection...");
    MQTTLoadingScreen(true);
    while (!mqttClient.connect(MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD)) {
      delay(1000);
    }
    Serial.println("Connected to mqtt broker");
    // connected to broker, subscribe topics
    mqttClient.subscribe(MQTT_HEARTBEAT_TOPIC);
    mqttClient.subscribe(MQTT_WELCOME_TOPIC);

    if (!initialConfiguration) {
      Serial.println("Getting subscribing config for rooms and sensors");
      dbLoadingScreen(true);
      initialConfiguration = getRoomsAndActuatorsId(configuredRoomsId, configuredActuatorsId);
      dbLoadingScreen(false);
    }
    if (initialConfiguration) {
      subscribeToConfiguredRoomsAndActuators();
    }

  } else if (!initialConfiguration) {
    Serial.println("Getting subscribing config for rooms and sensors");
    dbLoadingScreen(true);
    initialConfiguration = getRoomsAndActuatorsId(configuredRoomsId, configuredActuatorsId);
    dbLoadingScreen(false);

    if (initialConfiguration) {
      subscribeToConfiguredRoomsAndActuators();
    }
  }

  MQTTLoadingScreen(false);

}

void disconnectMQTTBroker() {
  mqttClient.disconnect();
}

boolean isMQTTBrokerConnected() {
  return mqttClient.connected();
}

void mqttSendConfig(String mac, int roomId, int sensorsId[3], boolean monitoringActivated) {
  DynamicJsonDocument doc(MQTT_BUFFER_SIZE);

  doc["mac"] = mac;
  doc["monitoring"] = monitoringActivated;
  doc["room"] = roomId;
  doc["externalTemp"] = *externalTemperatureR;
  doc["ecoMode"] = *ecoModeR;
  doc["sleepDuration"] = *sleepModeR ? 60 - getTimeMinute() * 3600000 : 0;

  for (int i = 0; i < 3; i++) {
    doc["sensors"][i] = sensorsId[i];
  }

  char buffer[MQTT_BUFFER_SIZE] = {0};
  size_t n = serializeJson(doc, buffer);
  mqttClient.publish(MQTT_CONFIG_TOPIC, buffer, n);
}

void mqttSendMonitoringControl(int roomId, String control) {
  mqttClient.publish(String(MQTT_ROOM_TOPIC) + "/" + String(roomId) + "/monitoring/control", control);
}

void mqttSendEcoMode(boolean control) {
  if (control) {
    mqttClient.publish(MQTT_ECO_MODE_TOPIC, "ON");
  } else {
    mqttClient.publish(MQTT_ECO_MODE_TOPIC, "OFF");
  }
}

void mqttSendSleepSchedule(int Time) {
  mqttClient.publish(MQTT_SLEEP_SCHEDULE_TOPIC, String(Time));
}

void mqttSendActuatorControl(int sensorId, String control) {
  mqttClient.publish(String(MQTT_ACTUATORS_TOPIC) + "/" + String(sensorId) + "/control", control);
}

void mqttSendExternalTemperature(int temperature) {
  mqttClient.publish(MQTT_EXTERNAL_TEMPERATURE_TOPIC, String(temperature));
}

void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  int roomId = -1;
  int actuatorId = -1;

  if (topic == MQTT_HEARTBEAT_TOPIC) {
    // update timestamp of tha last heartbeat
    updateLastHBTimestamp(payload);
  } else if (topic == MQTT_WELCOME_TOPIC) {
    // If a new device sent me his mac

    int sensorsId[3] = { -1, -1, -1};
    int actuatorsId[2] = { -1, -1};
    boolean monitoringActivated;

    // if device hasn't already a config, create a new config
    if (!getRoomConfig(payload, &roomId, sensorsId, actuatorsId, &monitoringActivated)) {
      createRoomConfig(payload);
      createSensorsConfig(payload);
      createActuatorsConfig(payload);
      getRoomConfig(payload, &roomId, sensorsId, actuatorsId, &monitoringActivated);
    }

    subscribeToRoomQueues(roomId);

    //subscribe to sensor's monitoring queue
    for (int i = 0; i < 3; i++) {
      subscribeToRoomQueues(sensorsId[i]);
    }

    // update timestamp of tha last heartbeat
    updateLastHBTimestamp(roomId);

    // send room config to the slave
    mqttSendConfig(payload, roomId, sensorsId, monitoringActivated);

  } else {

    if (topic.endsWith("/alarm")) {
      // if topic is room's logging

      sscanf(&topic[0], &((MQTT_ROOM_TOPIC + String("/%d/alarm"))[0]), &roomId);

      DynamicJsonDocument doc(MQTT_BUFFER_SIZE);
      deserializeJson(doc, payload);

      logAlarm(&(doc["message"].as<String>())[0], doc["code"].as<int>(), roomId);

    }
    if (topic.endsWith("/measures")) {
      // if topic is room's logging

      sscanf(&topic[0], &((MQTT_ROOM_TOPIC + String("/%d/measures"))[0]), &roomId);

      DynamicJsonDocument doc(MQTT_BUFFER_SIZE);
      deserializeJson(doc, payload);

      for (JsonPair keyValue : doc.as<JsonObject>()) {
        logSensorMeasure(atoi(keyValue.key().c_str()), &(keyValue.value().as<String>())[0]);
      }

    } else if (topic.endsWith("/monitoring/control")) {
      // if topic is room's monitoring control

      sscanf(&topic[0], &((MQTT_ROOM_TOPIC + String("/%d/monitoring/control"))[0]), &roomId);
      updateRoomMonitoring(roomId, payload == "START" ? true : false);

    } else if (topic.endsWith("/control")) {
      // if topic is room's sensor's control

      sscanf(&topic[0], &((MQTT_ACTUATORS_TOPIC + String("/%d/control"))[0]), &actuatorId);
      updateActuatorConfig(actuatorId, payload);

    } else {
      // no topic matched

    }

  }
}
