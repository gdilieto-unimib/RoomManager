#ifndef MQTT_CONTROLLER_ROOM_MANAGER

#define MQTT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include <ArduinoJson.h>
#include "macros_room_manager.h"

#include <WiFi101.h>
#include <MQTT.h>

// MQTT data
#define MQTT_BUFFER_SIZE 128                            // the maximum size for packets being published and received
#define MQTT_CONFIG_TOPIC "LabIOT/gdltf/config"      // topic to control the led
#define MQTT_STATUS_TOPIC "LabIOT/gdltf/config"       // topic to publish the led status
#define MQTT_ROOM_TOPIC "LabIOT/gdltf/room"

#include "secrets.h"

void connectToMQTTBroker();
void mqttMessageReceived(String &topic, String &payload);
void MQTTSetup();
MQTTClient getMqttClient();
void mqttSendData(int lastTemp, int lastLight, int lastWifiRssi);
void mqttSendMac();
void MQTTLoadingScreen(boolean i);
void loopMqttClient();
boolean isMQTTBrokerConnected();
boolean isRoomConfigured();

#endif
