#ifndef MQTT_CONTROLLER_ROOM_MANAGER

#define MQTT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

#include <WiFi101.h>
#include <MQTT.h>

// MQTT data
#define MQTT_BUFFER_SIZE 128               // the maximum size for packets being published and received
#define MQTT_TOPIC_CONTROL "led/control"   // topic to control the led
#define MQTT_TOPIC_STATUS "led/control"     // topic to publish the led status



#include "secrets.h"

void connectToMQTTBroker();
void mqttMessageReceived(String &topic, String &payload);
void MQTTSetup();
MQTTClient getMqttClient();
#endif
