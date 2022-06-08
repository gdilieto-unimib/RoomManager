#include "mqtt_controller_room_manager.h"
#include "rgb_lcd_controller_room_manager.h"
#include "wifi_controller_room_manager.h"

#include "secrets.h"

MQTTClient mqttClient(MQTT_BUFFER_SIZE);   // handles the MQTT communication protocol
WiFiClient networkClient;   // handles the network connection to the MQTT broker

int MQTT_roomId = -1;

//Positions of sensor ids ordered alphabetically for types: 1° - Light, 2° - Temperature, 3° - Wifi
int MQTT_sensorsId[3] = { -1, -1, -1};

boolean* monitoringActivatedRef;
boolean * ecoModeRef;
int* tempConfigRef;
int* lightConfigRef;
int* externalTemperatureRef;

void MQTTSetup(boolean* monitoringActivated, int* tempConfig, int* lightConfig, int* externalTemperature, boolean * ecoMode){
   monitoringActivatedRef = monitoringActivated;
   tempConfigRef = tempConfig;
   lightConfigRef = lightConfig;
   externalTemperatureRef = externalTemperature;
   ecoModeRef=ecoMode;
   
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
    while (!mqttClient.connect(MQTT_CLIENTID, MQTT_USERNAME, MQTT_PASSWORD)) {
      Serial.print(F("."));
      delay(1000);
    }
    Serial.println(F("\nConnected!"));

    // connected to broker, subscribe topics
    mqttClient.subscribe(MQTT_CONFIG_TOPIC);
    Serial.println(F("\nSubscribed to config topic!"));    
  }
  MQTTLoadingScreen(false);

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

void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  Serial.println("Incoming MQTT message: " + topic + " - " + payload);
    if (topic == MQTT_CONFIG_TOPIC) {
      // deserialize the JSON object
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);

      *ecoModeRef = doc["ecoMode"].as<boolean>();
      *externalTemperatureRef = doc["externalTemp"].as<int>();

  
      const char *mac = doc["mac"];
      if(String(mac)==getMac()) {
        Serial.println("CONFIG FOR ME");
        MQTT_roomId = (int)doc["room"];
        for (int i = 0 ; i < 3 ; i++)
          MQTT_sensorsId[i] = doc["sensors"][i];
          
        //subscribe to room's monitoring queue
        mqttClient.subscribe(String(MQTT_ROOM_TOPIC)+"/"+String(MQTT_roomId)+"/monitoring/control");
        
        //subscribe to sensor's actuators control queue
        for (int i=0; i<3; i++) {
          mqttClient.subscribe(String(MQTT_SENSOR_TOPIC)+"/"+String(MQTT_sensorsId[i])+ "/control");
        }

        //subscribe to singleMode queue for light
        mqttClient.subscribe(String(MQTT_ROOM_TOPIC)+"/singleMode/light/control");

        //subscribe to externalTemperature queue to get external temperature (used in eco mode)
        mqttClient.subscribe(String(MQTT_ROOM_TOPIC)+"/externalTemperature");

        *monitoringActivatedRef = doc["monitoring"];
      }
       
    } else if (topic == String(MQTT_ROOM_TOPIC)+"/singleMode/light/control") {
      
      // deserialize the JSON object
      DynamicJsonDocument doc(2048);
      deserializeJson(doc, payload);
      const int roomId = doc["room"].as<int>();
      if(roomId==MQTT_roomId) {
        const char *lightConfig = doc["config"];
        
        if (String(lightConfig)=="ON") {
          *lightConfigRef = CONFIG_ON;
        } else if (String(lightConfig)=="AUTO"){
          *lightConfigRef = CONFIG_AUTO;
        } else {
          *lightConfigRef = CONFIG_OFF;
        }
      } else {
          *lightConfigRef = CONFIG_OFF;        
      }
      
    } else if (topic == String(MQTT_ROOM_TOPIC)+"/"+String(MQTT_roomId)+"/monitoring/control"){
      
      if (payload=="START") {
        *monitoringActivatedRef = true;
      } else {
        *monitoringActivatedRef = false;
      }
      
    } else if (topic == String(MQTT_SENSOR_TOPIC)+"/"+String(MQTT_sensorsId[LIGHT_SENSOR])+"/control") {
      
      if (payload=="ON") {
        *lightConfigRef = CONFIG_ON;
      } else if (payload=="AUTO"){
        *lightConfigRef = CONFIG_AUTO;
      } else {
        *lightConfigRef = CONFIG_OFF;
      }
      
    } else if (topic == String(MQTT_SENSOR_TOPIC)+"/"+String(MQTT_sensorsId[TEMP_SENSOR])+"/control") {
      
      if (payload=="ON") {
        *tempConfigRef = CONFIG_ON;
      } else {
        *tempConfigRef = CONFIG_OFF;
      }
      
    } else if (topic == String(MQTT_ROOM_TOPIC)+"/externalTemperature") {
      
      *externalTemperatureRef = atoi(&payload[0]);
      
    } else {
      Serial.println(F("MQTT Topic not recognized, message skipped"));
    }
}

boolean isRoomConfigured(){
  return MQTT_roomId!= -1;
}

void mqttSendData(int lastTemp, int lastLight, int lastWifiRssi) {
  DynamicJsonDocument doc(1024);
  
  doc[String(MQTT_sensorsId[0])] = String(lastLight)+" lux";
  doc[String(MQTT_sensorsId[1])] = String(lastTemp)+" C";
  doc[String(MQTT_sensorsId[2])] = String(lastWifiRssi)+" dB";
  
  char buffer[128];
  size_t n = serializeJson(doc, buffer);

  mqttClient.publish(&(String(MQTT_ROOM_TOPIC)+"/"+String(MQTT_roomId)+"/logging")[0], buffer, n);
}

void mqttSendLightConfig(int lightConfig) {
  mqttClient.publish(&(String(MQTT_SENSOR_TOPIC)+"/"+String(MQTT_sensorsId[LIGHT_SENSOR])+"/control")[0], lightConfig==CONFIG_AUTO?"AUTO":lightConfig==CONFIG_ON?"ON":"OFF");
}

void mqttSendTempConfig(int tempConfig) {
  mqttClient.publish(&(String(MQTT_SENSOR_TOPIC)+"/"+String(MQTT_sensorsId[TEMP_SENSOR])+"/control")[0], tempConfig==CONFIG_ON?"ON":"OFF");
}

void mqttSendMonitoringConfig(boolean monitoringConfig) {
  mqttClient.publish(&(String(MQTT_ROOM_TOPIC)+"/"+String(MQTT_roomId)+"/monitoring/control")[0], monitoringConfig?"START":"STOP");
}

void mqttSendAlarm(char* message, int code) {
  DynamicJsonDocument doc(1024);
  doc["message"] = message;
  doc["code"] = code;
  
  char buffer[256];
  size_t n = serializeJson(doc, buffer);

  mqttClient.publish(&(String(MQTT_ROOM_TOPIC)+"/"+String(MQTT_roomId)+"/alarm")[0], buffer, n);
}

void mqttSendMac() {
  mqttClient.publish(MQTT_HEARTBEAT_TOPIC, getMac());
  Serial.print("INVIATO MAC:");
  Serial.println(getMac());
}
