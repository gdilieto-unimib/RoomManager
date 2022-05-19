/*
 * ****************************************************************************
  University of Milano-Bicocca
  DISCo - Department of Informatics, Systems and Communication
  Viale Sarca 336, 20126 Milano, Italy

  Assignment 1 for IOT Laboratory classroom by:
     Gabriele Di Lieto - g.dilieto@unimib.it
     Tiberio Falsiroli - t.falsiroli@unimib.it
 * ****************************************************************************

  Room manager: an IOT application that allows you to monitorate your rooms' temperature and light sensors and control your actuators (thermostats and lights).

  This is the master component of the architecture.
  
  Notes:
    - MKR pin mapping -> https://docs.arduino.cc/static/a5b251782d7fa7d0212e7a8b34c45a9e/ABX00004-pinout.png

*/

#include "macros_room_manager_master.h"
#include "wifi_controller_room_manager_master.h"
#include "database_controller_room_manager_master.h"
#include "rgb_lcd_controller_room_manager_master.h"
#include "mqtt_controller_room_manager_master.h"

long timeDb, timeLogging, timeScreen;

void setup()
{
  setupLcd();
  MQTTSetup();
  timeDb = timeLogging = timeScreen = millis();
}

void loop()
{
  // Connect to wifi
  tryWifiConnection();
  
  // Connect to mqtt broker
  tryMQTTBrokerConnection();

  // Check new messages if connected to the Broker
  if (isMQTTBrokerConnected()) {
    getMqttClient().loop(); // MQTT client loop  
  }
   
  // Connect to db
  //tryDbConnection();
  
  // Update screen
  updateScreen();
}

void tryWifiConnection() {
  // Connect to wifi if not already connected
  
  while (!isWifiConnected()) {
    wifiLoadingScreen(true);
    connectWifi();
  }
  wifiLoadingScreen(false);
  updateScreen();
}

void tryDbConnection() {
  // Connect to MySql if not already connected, while connected to wifi, each DB_CONNECTION_TIMER_MILLIS ms
  
  if (isWifiConnected() && !isMySqlConnected() && (millis() - timeDb) > DB_CONNECTION_TIMER_MILLIS) {
    dbLoadingScreen(true);
    connectToMySql();
    dbLoadingScreen(false);
    updateScreen();
    timeDb = millis();
  }
}

void tryMQTTBrokerConnection() {
  if (isWifiConnected() && !isMQTTBrokerConnected()) {
    MQTTLoadingScreen(true);
    connectToMQTTBroker();   // connect to MQTT broker (if not already connected)
    MQTTLoadingScreen(false);
  }
}

void updateScreen() {
  // Update screen each SCREEN_UPDATE_TIMER_MILLIS ms
  
  if ((millis() - timeScreen) > SCREEN_UPDATE_TIMER_MILLIS) {
    updateInfoScreenRows(0, isWifiConnected(), isMySqlConnected());
    timeScreen = millis();
  }
}
