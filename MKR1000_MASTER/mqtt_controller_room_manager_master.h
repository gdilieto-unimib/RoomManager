#ifndef MQTT_CONTROLLER_ROOM_MANAGER

#define MQTT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include <ArduinoJson.h>

#include <WiFi101.h>
#include <MQTT.h>

// MQTT data
#define MQTT_BUFFER_SIZE 256                              // the maximum size for packets being published and received

#define MQTT_WELCOME_TOPIC "LabIOT/gdltf/welcome"            // topic for slave welcome
#define MQTT_HEARTBEAT_TOPIC "LabIOT/gdltf/heartbeat"       // topic for heartbeats
#define MQTT_CONFIG_TOPIC "LabIOT/gdltf/config"            // topic for room's configuration
#define MQTT_ROOM_TOPIC "LabIOT/gdltf/rooms"              // topic for room's control
#define MQTT_SENSOR_TOPIC "LabIOT/gdltf/sensors"         // topic for sensor's control
#define MQTT_ACTUATORS_TOPIC "LabIOT/gdltf/actuators"   // topic for actuator's control
#define MQTT_ECO_MODE_TOPIC "LabIOT/gdltf/rooms/ecoMode"      // topic for eco mode's control
#define MQTT_SLEEP_SCHEDULE_TOPIC "LabIOT/gdltf/rooms/scheduleTopic"      // topic for sleep schedule's control
#define MQTT_EXTERNAL_TEMPERATURE_TOPIC "LabIOT/gdltf/rooms/externalTemperature"      // topic for sleep external temperature notification

#include "macros_room_manager_master.h"
#include "database_controller_room_manager_master.h"
#include "secrets_master.h"

void loopMqttClient();

void connectToMQTTBroker();

void disconnectMQTTBroker();

boolean isMQTTBrokerConnected();

void mqttMessageReceived(String &topic, String &payload);

void MQTTSetup(int* externalTemperature, boolean* ecoMode, boolean*  thisHourSleepMode);

void mqttSendMonitoringControl(int roomId, String action);

void mqttSendActuatorControl(int sensorId, String action);

void mqttSendExternalTemperature(int temperature);

void mqttSendEcoMode(boolean control);

void mqttSendSleepSchedule(int Time);




#endif
