#include "mqtt_controller_room_manager.h"

MQTTClient mqttClient(MQTT_BUFFER_SIZE);   // handles the MQTT communication protocol
WiFiClient networkClient;   // handles the network connection to the MQTT broker

void MQTTSetup(){
   mqttClient.begin(MQTT_BROKERIP, 1883, networkClient);   // setup communication with MQTT broker
   mqttClient.onMessage(mqttMessageReceived);              // callback on message received from MQTT broker

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
    mqttClient.subscribe(MQTT_TOPIC_CONTROL);
    Serial.println(F("\nSubscribed to control topic!"));
  }
}


void mqttMessageReceived(String &topic, String &payload) {
  // this function handles a message from the MQTT broker
  Serial.println("Incoming MQTT message: " + topic + " - " + payload);

  if (topic == MQTT_TOPIC_CONTROL) {
    if (payload == "on") {
Serial.print("PAYLOAD ON");
    } else if (payload == "off") {
Serial.print("PAYLOAD OFF");
    } else {
      Serial.println(F("MQTT Payload not recognized, message skipped"));
    }
  } else {
    Serial.println(F("MQTT Topic not recognized, message skipped"));
  }
}

void mqttSendData(int lastTemp, int lastLight) {

const int capacity = JSON_OBJECT_SIZE(2);
      StaticJsonDocument<capacity> doc;
      
      doc["temperature"] = lastTemp;
      doc["light"] = lastLight;
      
      char buffer[128];
      size_t n = serializeJson(doc, buffer);
      Serial.print(F("JSON message: "));
      Serial.println(buffer);
      getMqttClient().publish(MQTT_TOPIC_STATUS, buffer, n);
}
