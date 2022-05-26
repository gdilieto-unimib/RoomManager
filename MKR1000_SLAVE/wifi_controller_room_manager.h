#ifndef WIFI_CONTROLLER_ROOM_MANAGER

#define WIFI_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "secrets.h"

void connectWifi(char* ssid, char* pass);

boolean isWifiConnected();

WiFiClient* getClient();

String getMac();

#endif
