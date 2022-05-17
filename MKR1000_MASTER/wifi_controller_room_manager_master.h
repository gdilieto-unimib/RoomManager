#ifndef WIFI_CONTROLLER_ROOM_MANAGER

#define WIFI_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager_master.h"

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "secrets_master.h"

void connectWifi();

boolean isWifiConnected();

WiFiClient* getClient();

#endif
