#ifndef MQTT_CONTROLLER_ROOM_MANAGER

#define MQTT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi101.h>
#include <MQTT.h>

// MQTT data
#define MQTT_BUFFER_SIZE 128               // the maximum size for packets being published and received
#define MQTT_CONFIG_TOPIC "LabIOT/gdltf/config"   // topic for config
#define MQTT_ROOM_TOPIC "LabIOT/gdltf/room"

#include "macros_room_manager_master.h"
#include "database_controller_room_manager_master.h"
#include "secrets_master.h"

void loopMqttClient();

void connectToMQTTBroker();

boolean isMQTTBrokerConnected();

void mqttMessageReceived(String &topic, String &payload);

void MQTTSetup();

void mqttSendData(int lastTemp, int lastLight);

#endif
