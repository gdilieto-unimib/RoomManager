#ifndef WIFI_CONTROLLER_ROOM_MANAGER

#define WIFI_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>


#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "secrets.h"



boolean connect();
void setupWiFi();

#endif
