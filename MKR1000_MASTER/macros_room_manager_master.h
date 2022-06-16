#ifndef MACROS_ROOM_MANAGER

#define MACROS_ROOM_MANAGER
#include <Arduino.h>

#define DB_CONNECTION_TIMER_MILLIS 10000
#define MQTT_CONNECTION_TIMER_MILLIS 10000
#define SCREEN_UPDATE_TIMER_MILLIS 2000
#define DEVICES_UPDATE_TIMER_MILLIS 10000
#define CONFIGURATION_UPDATE_TIMER_MILLIS 10000
#define EXTERNAL_TEMP_UPDATE_TIMER_MILLIS 1800000
#define SEND_SLEEP_SCHEDULE_TIMER_MILLIS 10000
#define HTTP_BUFFER_SIZE 256

#define LIGHT_SENSOR 0 // id of the index of the light sensor among sensors list
#define TEMP_SENSOR 1 // id of the index of the temperature sensor among sensors list
#define WIFI_SENSOR 2 // id of the index of the wifi sensor among sensors list

#define MAX_ROOMS_NUMBER 100  // max number of rooms

#endif
