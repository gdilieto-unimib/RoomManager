#ifndef ACCESSPOINT_CONTROLLER_ROOM_MANAGER

#define ACCESSPOINT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager_master.h"
#include <SPI.h>
#include <WiFi101.h>

String connectToWifiAP();
void getCredentials();
void getWiFi();
void setupAP();
String getWifiList();
void listNetworks();


#endif
