#include "mqtt_controller_room_manager.h"
#include "rgb_lcd_controller_room_manager.h"

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
    mqttClient.subscribe(MQTT_TOPIC_CONTROL);
    Serial.println(F("\nSubscribed to control topic!"));    
  }
  MQTTLoadingScreen(false);

}

void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  Serial.println("Incoming MQTT message: " + topic + " - " + payload);
  if(!validateIP(payload)){
    Serial.println("HO RICEVUTO UN MESSAGGIO DI CONFIGURAZIONE");
  if (topic == MQTT_TOPIC_CONTROL) {
    // deserialize the JSON object
    DynamicJsonDocument doc(2048);
    deserializeJson(doc, payload);

    int ip_temp[] = IP;
    char ipv4[15] = {0};
    sprintf(ipv4, "%d.%d.%d.%d", ip_temp[0], ip_temp[1], ip_temp[2], ip_temp[3]);

    //const char *ip = doc["ip"];
    MQTT_roomId = (int)doc["room"];
    for (int i = 0 ; i < 3 ; i++)
      MQTT_sensorsId[i] = doc["sensors"][i];
      
    Serial.println("STAMPE DI PROVA");

    Serial.println(MQTT_roomId);
    for (int i = 0 ; i < 3 ; i++)
      Serial.println(MQTT_sensorsId[i]);

        
      sprintf(roomTopic, "%s/%d", MQTT_ROOM_TOPIC ,MQTT_roomId);
      
    
  } else {
    Serial.println(F("MQTT Topic not recognized, message skipped"));
  }
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

void mqttSendIP() {
    int ip[] = IP;
    char ipv4[16] = {0};
    sprintf(ipv4, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);

      mqttClient.publish(MQTT_TOPIC_STATUS, ipv4);
      Serial.print("ESCO!!!!!!!");
}
