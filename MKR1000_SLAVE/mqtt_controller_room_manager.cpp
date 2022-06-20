#include "mqtt_controller_room_manager.h"
#include "rgb_lcd_controller_room_manager.h"
#include "wifi_controller_room_manager.h"

#include "secrets.h"

MQTTClient mqttClient(MQTT_BUFFER_SIZE);   // handles the MQTT communication protocol
WiFiClient networkClient;   // handles the network connection to the MQTT broker

int MQTT_roomId = -1;

//Positions of sensor ids ordered alphabetically for types: 1° - Light, 2° - Temperature, 3° - Wifi
int MQTT_sensorsId[3] = { -1, -1, -1};

//Positions of actuators ids ordered alphabetically for types: 1° - Light, 2° - Temperature
int MQTT_actuatorsId[2] = { -1, -1};

boolean* monitoringActivatedRef;
boolean * ecoModeRef;
int* tempConfigRef;
int* lightConfigRef;
int* externalTemperatureRef;
long* scheduleRef;


void MQTTSetup(boolean* monitoringActivated, int* tempConfig, int* lightConfig, int* externalTemperature, boolean * ecoMode, long * schedule) {
  monitoringActivatedRef = monitoringActivated;
  tempConfigRef = tempConfig;
  lightConfigRef = lightConfig;
  externalTemperatureRef = externalTemperature;
  ecoModeRef = ecoMode;
  scheduleRef = schedule;

  mqttClient.begin(MQTT_BROKERIP, 1883, networkClient);   // setup communication with MQTT broker
  mqttClient.onMessage(mqttMessageReceived);              // callback on message received from MQTT broker

}

boolean isMQTTBrokerConnected() {
  return mqttClient.connected();
}

void loopMqttClient() {
  mqttClient.loop();
}

void connectToMQTTBroker() {
  MQTTLoadingScreen(true);

  if (!mqttClient.connected()) {   // not connected
    Serial.print(F("\nConnecting to MQTT broker..."));
    while (!mqttClient.connect(&(MQTT_CLIENTID + getMac())[0], MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.print(F("."));
      delay(1000);
    }
    Serial.println(F("\nConnected!"));

    // connected to broker, subscribe topics

    // subscribe to config topic to get the configuration (roomId, sensorsId, actuatorsId)
    mqttClient.subscribe(MQTT_CONFIG_TOPIC);
    
    // subscribe to eco mode topic to set/unset the eco mode
    mqttClient.subscribe(MQTT_ECO_MODE_TOPIC);
    
    // subscribe to sleepSchedule queue to get sleeping timeouts
    mqttClient.subscribe(MQTT_SLEEP_SCHEDULE_TOPIC);
    
    // subscribe to externalTemperature queue to get external temperature (used in eco mode)
    mqttClient.subscribe(MQTT_EXTERNAL_TEMPERATURE_TOPIC);

    Serial.println(F("\nSubscribed to general slave topics!"));
  }
  MQTTLoadingScreen(false);

}

void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  Serial.println("Incoming MQTT message: " + topic + " - " + payload);
  if (topic == MQTT_CONFIG_TOPIC) {
    // deserialize the JSON object
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    *ecoModeRef = doc["ecoMode"].as<boolean>();
    *externalTemperatureRef = doc["externalTemp"].as<int>();
    *scheduleRef = doc["sleepDuration"].as<int>();

    const char *mac = doc["mac"];
    if (String(mac) == getMac()) {

      // Get room id from the payload
      MQTT_roomId = (int)doc["room"];

      // Get sensors id from the payload
      for (int i = 0 ; i < 3 ; i++)
        MQTT_sensorsId[i] = doc["sensors"][i];
        
      // Get actuators id from the payload
      for (int i = 0 ; i < 2 ; i++)
        MQTT_actuatorsId[i] = doc["actuators"][i];

      //subscribe to room's monitoring queue
      mqttClient.subscribe(String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId) + "/monitoring/control");

      //subscribe to schedule queue
      mqttClient.subscribe(String(MQTT_ROOM_TOPIC) + "/sleepSchedule");

      //subscribe to actuators control queue
      for (int i = 0; i < 2; i++) {
        mqttClient.subscribe(String(MQTT_ROOM_TOPIC)+"/"+String(MQTT_roomId)+"/"+String(MQTT_ACTUATORS_TOPIC) + "/" + String(MQTT_actuatorsId[i]) + "/control");
      }

      //subscribe to singleMode queue for light
      mqttClient.subscribe(String(MQTT_ROOM_TOPIC) + "/singleMode/light/control");

      Serial.println(F("\nSubscribed to specific slave topics!"));

      *monitoringActivatedRef = doc["monitoring"];
    }

  } else if (topic == String(MQTT_ROOM_TOPIC) + "/singleMode/light/control") {

    // deserialize the JSON object
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);
    const int roomId = doc["room"].as<int>();
    if (roomId == MQTT_roomId) {
      const char *lightConfig = doc["config"];

      if (String(lightConfig) == "ON") {
        *lightConfigRef = CONFIG_ON;
      } else if (String(lightConfig) == "AUTO") {
        *lightConfigRef = CONFIG_AUTO;
      } else {
        *lightConfigRef = CONFIG_OFF;
      }
    } else {
      *lightConfigRef = CONFIG_OFF;
    }

  } else if (topic == String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId) + "/monitoring/control") {

    if (payload == "START") {
      *monitoringActivatedRef = true;
    } else {
      *monitoringActivatedRef = false;
    }

  } else if (topic == String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId)+"/"+String(MQTT_ACTUATORS_TOPIC) + "/" + String(MQTT_actuatorsId[LIGHT_SENSOR]) + "/control") {

    if (payload == "ON") {
      *lightConfigRef = CONFIG_ON;
    } else if (payload == "AUTO") {
      *lightConfigRef = CONFIG_AUTO;
    } else {
      *lightConfigRef = CONFIG_OFF;
    }

  } else if (topic == String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId)+"/"+String(MQTT_ACTUATORS_TOPIC) + "/" + String(MQTT_actuatorsId[TEMP_SENSOR]) + "/control") {

    if (payload == "ON") {
      *tempConfigRef = CONFIG_ON;
    } else {
      *tempConfigRef = CONFIG_OFF;
    }

  } else if (topic == String(MQTT_ROOM_TOPIC) + "/externalTemperature") {

    *externalTemperatureRef = atoi(&payload[0]);

  } else if (topic == String(MQTT_ECO_MODE_TOPIC)) {
    if (payload == "OFF") *ecoModeRef = false;
    if (payload == "ON") *ecoModeRef = true;



  } else if (topic == String(MQTT_SLEEP_SCHEDULE_TOPIC)) {
    *scheduleRef = atoi(&payload[0]);
  } else {
    Serial.println(F("MQTT Topic not recognized, message skipped"));
  }
}

boolean isRoomConfigured() {
  return MQTT_roomId != -1;
}

void mqttSendData(int lastTemp, int lastLight, int lastWifiRssi) {
  DynamicJsonDocument doc(MQTT_BUFFER_SIZE);

  doc[String(MQTT_sensorsId[0])] = String(lastLight) + " lux";
  doc[String(MQTT_sensorsId[1])] = String(lastTemp) + " C";
  doc[String(MQTT_sensorsId[2])] = String(lastWifiRssi) + " dB";

  char buffer[MQTT_BUFFER_SIZE];
  size_t n = serializeJson(doc, buffer);

  mqttClient.publish(&(String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId) + "/sensors/measures")[0], buffer, n);
}

void mqttSendLightConfig(int lightConfig) {
  mqttClient.publish(&(String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId) + "/" + String(MQTT_ACTUATORS_TOPIC) + "/" + String(MQTT_actuatorsId[LIGHT_SENSOR]) + "/control")[0], lightConfig == CONFIG_AUTO ? "AUTO" : lightConfig == CONFIG_ON ? "ON" : "OFF");
}

void mqttSendTempConfig(int tempConfig) {
  mqttClient.publish(&(String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId) + "/" + String(MQTT_ACTUATORS_TOPIC) + "/" + String(MQTT_actuatorsId[TEMP_SENSOR]) + "/control")[0], tempConfig == CONFIG_ON ? "ON" : "OFF");
}

void mqttSendMonitoringConfig(boolean monitoringConfig) {
  mqttClient.publish(&(String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId) + "/monitoring/control")[0], monitoringConfig ? "START" : "STOP");
}

void mqttSendAlarm(char* message, int code) {
  DynamicJsonDocument doc(MQTT_BUFFER_SIZE);
  doc["message"] = message;
  doc["code"] = code;

  char buffer[MQTT_BUFFER_SIZE];
  size_t n = serializeJson(doc, buffer);

  mqttClient.publish(&(String(MQTT_ROOM_TOPIC) + "/" + String(MQTT_roomId) + "/alarm")[0], buffer, n);
}

void mqttSendHeartbeat() {
  mqttClient.publish(MQTT_roomId == -1 ? MQTT_WELCOME_TOPIC : MQTT_HEARTBEAT_TOPIC, MQTT_roomId == -1 ? getMac() : String(MQTT_roomId));
  
  if (MQTT_roomId == -1) {
    Serial.println("SENT MAC TO " + String(MQTT_WELCOME_TOPIC) + " " + getMac()); 
  } else {
    Serial.println("SENT ROOM ID TO " + String(MQTT_HEARTBEAT_TOPIC) + " " + MQTT_roomId);
  }
}
