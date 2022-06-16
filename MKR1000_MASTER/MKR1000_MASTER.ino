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

#include "accesspoint_controller_room_manager_master.h"

#include "mqtt_controller_room_manager_master.h"

#include "flashmem_controller_room_manager_master.h"

#include "api_controller_room_manager_master.h"

#include "weather_controller_room_manager_master.h"

#include "time_controller_room_manager_master.h"


long timeDb, timeLogging, timeScreen, timeDevices, timeConfiguration, timeExtenalTemperature, timeSendSleepSchedule,workingTime;

FlashStorage(my_flash_store, WiFi_Credentials);
WiFi_Credentials MyWiFi_Credentials;

boolean singleMode;
boolean ecoMode;
int externalTemperature;
String schedule = "";
boolean sleepMode;

boolean configureWifi = false;

int devices;

void setup() {
  timeDb = timeLogging = timeScreen = workingTime = timeConfiguration = timeExtenalTemperature = timeSendSleepSchedule = millis();
  timeExtenalTemperature -= EXTERNAL_TEMP_UPDATE_TIMER_MILLIS;

  setupLcd();
  MQTTSetup( & externalTemperature, & ecoMode, & sleepMode);
  MyWiFi_Credentials = my_flash_store.read();

  if (!MyWiFi_Credentials.valid && configureWifi) {
    setupAP();
  }

  Serial.begin(115200);

}

void loop() {

  printWorkingStatus();
  
  // Connect to wifi
  tryWifiConnection();

  // Connect to db
  tryDbConnection();

  // Connect to mqtt broker
  tryMQTTBrokerConnection();

  // Loop for mqtt messages
  loopMqttClient();

  // Update the number of devices configured
  updateDevicesNumber();

  // Update the configuration of the app
  updateConfiguration();

  // Update screen
  updateScreen();

  //update sleep schedule for slave
  trySendSleepSchedule();
  
  //if connected send external temperature to slave
  trySendExternalTemperature();

}

void tryWifiConnection() {
  // try to connect to wifi (with loading screen)
  if (!isWifiConnected()) {
    String password;

    wifiLoadingScreen(true);
    if (MyWiFi_Credentials.valid == true) {
      while (!isWifiConnected()) {
        connectWifi(MyWiFi_Credentials.ssid_RM, MyWiFi_Credentials.pssw_RM);
      }
      setupRtc();
      setupApiServer( & ecoMode);

    } else {


      if (!configureWifi) {
        while (!isWifiConnected()) {
          connectWifi(SECRET_SSID, SECRET_PASS);
        }
        setupRtc();
        setupApiServer( & ecoMode);
      } else {
        while (!isWifiConnected()) {
          // activate the access point until wifi is connected
          password = connectToWifiAP();
        }

        MyWiFi_Credentials.valid = true;
        String ssidfl = WiFi.SSID();

        ssidfl.toCharArray(MyWiFi_Credentials.ssid_RM, 100);
        password.toCharArray(MyWiFi_Credentials.pssw_RM, 100);


        my_flash_store.write(MyWiFi_Credentials);

        // try to connect to wifi
        wifiLoadingScreen(false);
        NVIC_SystemReset();
      }
    }
  } else {
    listenForClients();
  }
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
  // Connect to MQTTBroker if not already connected and if wifi is connected

  if (isWifiConnected() && !isMQTTBrokerConnected()) {
    MQTTLoadingScreen(true);
    connectToMQTTBroker(); // connect to MQTT broker (if not already connected)
    MQTTLoadingScreen(false);
    updateScreen();
  }
}

void updateDevicesNumber() {
  // Updated the number of devices configured

  if (isWifiConnected() && isMySqlConnected() && (millis() - timeDevices) > DEVICES_UPDATE_TIMER_MILLIS) {
    getDevices( & devices);
    timeDevices = millis();
  }
}

void updateConfiguration() {
  // Updated the configuration of the app

  if (isWifiConnected() && isMySqlConnected() && (millis() - timeConfiguration) > CONFIGURATION_UPDATE_TIMER_MILLIS) {
    getConfiguration( & singleMode, & ecoMode, & schedule, & sleepMode);
    timeConfiguration = millis();
  }
}

void updateScreen() {
  // Update screen each SCREEN_UPDATE_TIMER_MILLIS ms

  if ((millis() - timeScreen) > SCREEN_UPDATE_TIMER_MILLIS) {
    updateInfoScreenRows(devices, isWifiConnected(), isMySqlConnected(), ecoMode);
    timeScreen = millis();
  }
}

void trySendExternalTemperature() {
  if (isWifiConnected() && isMQTTBrokerConnected() && (millis() - timeExtenalTemperature) > EXTERNAL_TEMP_UPDATE_TIMER_MILLIS) {
    externalTemperature = round(getExternalTemperature());
    mqttSendExternalTemperature(externalTemperature);
    timeExtenalTemperature = millis();
  }

}

void trySendSleepSchedule() {

  if (isWifiConnected() && isMQTTBrokerConnected() && (millis() - timeSendSleepSchedule) > SEND_SLEEP_SCHEDULE_TIMER_MILLIS) {
    if (sleepMode && ( & schedule[0])[getTimeHour()] == '1') {
      mqttSendSleepSchedule((60 - getTimeMinute()) * 3600000);
    }
    timeSendSleepSchedule = millis();
  }

}

void printWorkingStatus(){
    if(millis() - workingTime > 10000) {Serial.println("Master is Working..."); workingTime = millis();} 
}
