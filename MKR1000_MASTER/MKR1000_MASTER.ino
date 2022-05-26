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

long timeDb, timeLogging, timeScreen;

FlashStorage(my_flash_store, WiFi_Credentials);
WiFi_Credentials MyWiFi_Credentials;

void setup()
{
  timeDb = timeLogging = timeScreen = millis();
  
  setupLcd();
  MQTTSetup();
  MyWiFi_Credentials = my_flash_store.read();
  
  if(!MyWiFi_Credentials.valid)
      setupAP();
      
  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
}

void loop()
{
  // Connect to wifi
  tryWifiConnection();
  
  // Connect to db
  tryDbConnection();
  
  // Connect to mqtt broker
  tryMQTTBrokerConnection();

  // Loop for mqtt messages
  loopMqttClient();

  // Check new messages if connected to the Broker
  //Serial.print("CICAI"); // MQTT client loop  
  
  // Update screen
  updateScreen();
}

void tryWifiConnection() {
  // try to connect to wifi (with loading screen)
  if (!isWifiConnected()) {
    String password; 
  
    wifiLoadingScreen(true);
    if (MyWiFi_Credentials.valid == true) {
          Serial.println("STO USANDO LE CREDENZIALI PERSISTENTI");
          Serial.println(MyWiFi_Credentials.ssid_RM);
          Serial.println( MyWiFi_Credentials.pssw_RM);
          connectWifi(MyWiFi_Credentials.ssid_RM, MyWiFi_Credentials.pssw_RM);
          Serial.println("CONNESSO!");
    
    
    } else {
      Serial.println("TRY WIFI CONNECTION: ");
  
      while(!isWifiConnected()) {
        // activate the access point until wifi is connected
        password  = connectToWifiAP();
      }
    
      MyWiFi_Credentials.valid=true;
      String ssidfl = WiFi.SSID();
      
      ssidfl.toCharArray(MyWiFi_Credentials.ssid_RM, 100);
      password.toCharArray(MyWiFi_Credentials.pssw_RM, 100);
  
      Serial.println("SCRIVO......");
      my_flash_store.write(MyWiFi_Credentials);
      delay(1000);
    
      // try to connect to wifi
      wifiLoadingScreen(false);
    }
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
    connectToMQTTBroker();   // connect to MQTT broker (if not already connected)
    MQTTLoadingScreen(false);
    updateScreen();
  }
}

void updateScreen() {
  // Update screen each SCREEN_UPDATE_TIMER_MILLIS ms
  
  if ((millis() - timeScreen) > SCREEN_UPDATE_TIMER_MILLIS) {
    updateInfoScreenRows(0, isWifiConnected(), isMySqlConnected());
    timeScreen = millis();
  }
}
