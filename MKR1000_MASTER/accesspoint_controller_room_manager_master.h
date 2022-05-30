#ifndef ACCESSPOINT_CONTROLLER_ROOM_MANAGER

#define ACCESSPOINT_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include "macros_room_manager_master.h"
#include "wifi_controller_room_manager_master.h"
#include "mqtt_controller_room_manager_master.h"
#include <SPI.h>
#include <WiFi101.h>

String connectToWifiAP();
void printWiFiStatus();
void getCredentials();
void getWiFi();
void printAPStatus();
void setupAP(boolean configured);
String getWifiList();
void listNetworks();
void listenForClients();


#endif
