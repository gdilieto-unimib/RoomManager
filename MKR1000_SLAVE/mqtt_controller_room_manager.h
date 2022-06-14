#ifndef MQTT_CONTROLLER_ROOM_MANAGER

#define MQTT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include <ArduinoJson.h>
#include "macros_room_manager.h"

#include <WiFi101.h>
#include <MQTT.h>

// MQTT data
#define MQTT_BUFFER_SIZE 256                              // the maximum size for packets being published and received

#define MQTT_HEARTBEAT_TOPIC "LabIOT/gdltf/heartbeat"       // topic for heartbeats
#define MQTT_CONFIG_TOPIC "LabIOT/gdltf/config"            // topic for room's configuration
#define MQTT_ROOM_TOPIC "LabIOT/gdltf/rooms"              // topic for room's control
#define MQTT_SENSOR_TOPIC "LabIOT/gdltf/sensors"         // topic for sensor's control
#define MQTT_ECO_MODE_TOPIC "LabIOT/gdltf/ecoMode"
#define MQTT_SLEEP_SCHEDULE_TOPIC "LabIOT/gdltf/scheduleTopic"

#include "secrets.h"

void connectToMQTTBroker();

void mqttMessageReceived(String &topic, String &payload);

void MQTTSetup(boolean* monitoringActivated, int* tempConfig, int* lightConfig, int* externalTemperature, boolean * ecoMode, long * schedule);

MQTTClient getMqttClient();

void mqttSendData(int lastTemp, int lastLight, int lastWifiRssi);

void mqttSendAlarm(char* message, int code);

void mqttSendLightConfig(int lightConfig);

void mqttSendTempConfig(int tempConfig);

void mqttSendMonitoringConfig(boolean monitoringConfig);

void mqttSendMac();

void MQTTLoadingScreen(boolean i);

void loopMqttClient();

boolean isMQTTBrokerConnected();

boolean isRoomConfigured();

#endif
