#include "mqtt_controller_room_manager.h"
#include "rgb_lcd_controller_room_manager.h"
#include "wifi_controller_room_manager.h"

#include "secrets.h"


MQTTClient mqttClient(MQTT_BUFFER_SIZE);   // handles the MQTT communication protocol
WiFiClient networkClient;   // handles the network connection to the MQTT broker

int MQTT_roomId = -1;

//Positions of sensor ids ordered alphabetically for types: 1° - Light, 2° - Temperature, 3° - Wifi
int MQTT_sensorsId[3] = { -1, -1, -1};

char roomTopic[128] = {0};

void MQTTSetup(){
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
      if (!isValidMacAddress(&payload[0])) {
        Serial.println("HO RICEVUTO UN MESSAGGIO DI CONFIGURAZIONE");
        DynamicJsonDocument doc(2048);
        deserializeJson(doc, payload);
    
    
        const char *mac = doc["mac"];
        MQTT_roomId = (int)doc["room"];
        for (int i = 0 ; i < 3 ; i++)
          MQTT_sensorsId[i] = doc["sensors"][i];
          
        Serial.println("STAMPE DI PROVA");
    
        Serial.println(MQTT_roomId);
        for (int i = 0 ; i < 3 ; i++)
          Serial.println(MQTT_sensorsId[i]);
      
        sprintf(roomTopic, "%s/%d", MQTT_ROOM_TOPIC ,MQTT_roomId);
          
      }
    } else {
      Serial.println(F("MQTT Topic not recognized, message skipped"));
    }
}

boolean isRoomConfigured(){
  return MQTT_roomId!= -1;
}

void mqttSendData(int lastTemp, int lastLight, int lastWifiRssi) {
  DynamicJsonDocument doc(1024);
  
  doc[String(MQTT_sensorsId[0])] = lastLight;
  doc[String(MQTT_sensorsId[1])] = lastTemp;
  doc[String(MQTT_sensorsId[2])] = lastWifiRssi;
  
  char buffer[128];
  size_t n = serializeJson(doc, buffer);
  Serial.print(F("JSON message: "));
  Serial.println(buffer);
  mqttClient.publish(roomTopic, buffer, n);
}

void mqttSendMac() {
  mqttClient.publish(MQTT_STATUS_TOPIC, getMac());
  Serial.print("INVIATO MAC:");
  Serial.print(getMac());
}
