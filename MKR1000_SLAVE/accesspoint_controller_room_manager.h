#ifndef ACCESSPOINT_CONTROLLER_ROOM_MANAGER

#define ACCESSPOINT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager.h"
#include <SPI.h>
#include <WiFi101.h>

String connectToWifiAP();
void printWiFiStatus();
void getCredentials();
void getWiFi ();
void printAPStatus();
void setupAP();
String getWifiList();
void listNetworks();


#endif
