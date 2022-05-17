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

  Components for a room device:
    1x MKR1000
    1x MKR connector carrier
    6x GROVE 4 pins cable
    1x GROVE Temperature Sensor
    1x GROVE Light Sensor
    1x GROVE LED
    1x GROVE Buzzer

  Notes:
    - MKR pin mapping -> https://docs.arduino.cc/static/a5b251782d7fa7d0212e7a8b34c45a9e/ABX00004-pinout.png

*/

#include "macros_room_manager.h"
#include "rgb_lcd_controller_room_manager.h"
#include "action_controller_room_manager.h"
#include "wifi_controller_room_manager.h"
#include "io_controller_room_manager.h"
#include "database_controller_room_manager.h"
#include "mqtt_controller_room_manager.h"

int screen = INFO_SCREEN;
boolean navigationMode = true;

int lightStatus = LIGHT_STATUS_OFF;
int lightConfig = CONFIG_OFF;
int lightActivationThreshold = 500;

int tempStatus = TEMP_STATUS_OFF;
int tempConfig = CONFIG_OFF;
int tempActivationThreshold = 28;

int tooHotTempThreshold = 32;
int tooColdTempThreshold = 20;

int displayRow = 0;
int lastLight = 0;
int lastTemp = 0;
int lastWifiRssi = 0;

boolean tooHotAlarmMonitored = false;
boolean tooColdAlarmMonitored = false;
boolean fireAlarm = true;

boolean attemptDatabaseConnection = true;
boolean monitoringActivated = false;
boolean dbConfigured = false;
boolean startServer= true;

int roomId = -1;

//Positions of sensor ids ordered alphabetically for types: 1° - Light, 2° - Temperature, 3° - Wifi
int sensorsId[3] = { -1, -1, -1};

long timeDb, timeWifi, timeSensors, timeLogging, timeConfig;

WiFiServer server(80);



void setup()
{
  timeWifi = timeDb = timeSensors = timeLogging = timeConfig = millis();
  setupWiFi();
  setupLcd();
  setupIO();
  
  
  tryWifiConnection();
  if (isWifiConnected()) {
    tryToggleDbConnection();
  }
  if (isMySqlConnected()) {
    tryGetRoomConfig();
  }
  
  lastLight = getLight();
  lastTemp = getTemp();
  lastWifiRssi = getWifiRssi();
  
  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
}

void loop()
{
  if (isWifiConnected()) {
    // if wifi is connected, start http server
    if (startServer) {
      server.begin();
      startServer = false;
    }
    // listen for http requests
    listenForClients();
  MQTTSetup();
  connectToMQTTBroker();   // connect to MQTT broker (if not already connected)
  getMqttClient().loop();       // MQTT client loop

  getMqttClient().publish(MQTT_TOPIC_STATUS, "SONO CONNESSO!");   // publish new MQTT led status (ON)
  Serial.print("Messaggio inviato");
    
  }
  
  // connect to WiFi (if not already connected) every 10 seconds
  if (( (millis() - timeWifi) > 10000 ) && !isWifiConnected()) {
    
    // if wifi was disconnected, monitoring and http server are resetted
    monitoringActivated = false;
    startServer = true;

    // try to connect to wifi
    tryWifiConnection();
    
    timeWifi = millis();
  }
  
  // eventually toggle connection to MySql database every 10 seconds
  if (( (millis() - timeDb) > 10000 ) && isWifiConnected()){
    // try to enable/disable db connection
    tryToggleDbConnection();
    timeDb = millis();
  }
  
  // get configuration from database (if not already connected) every 10 seconds
  if (( (millis() - timeConfig) > 10000 ) && isWifiConnected() && !dbConfigured) {
    // try to download room's device configuration
    tryGetRoomConfig();
    timeConfig = millis();
  }

  // get the actual pressed button
  int pressedButton = getPressedButton();

  if (navigationMode) {
    // if navigating though screen
    navigate(pressedButton);
  } else {
    // if acting on a screen
    action(pressedButton);
  }

  //update screen if an operation has been made
  if (pressedButton != NO_OP) {
      updateScreen();
  }  

  //check (each second) if sensors are changed, if so update the screen
  if ( (millis() - timeSensors) > 2000 ) {    
      updateSensors();
      updateScreen();
      
      timeSensors = millis();
  }

  //log (each ten seconds) measures of the sensors
  if ( (millis() - timeLogging) > 10000 && dbConfigured && monitoringActivated && isMySqlConnected()) {
      tryLogMeasures();
      timeLogging = millis();
  }

  //set hot or cold alarm
  setHotColdAlarm(lastTemp);
}

void tryWifiConnection() {
  // try to connect to wifi (with loading screen)
  
  wifiLoadingScreen(true);
  connectWifi();
  wifiLoadingScreen(false);
  updateScreen();
}

void tryToggleDbConnection() {
  // try to toggle database connection
  
  if (!isMySqlConnected() && attemptDatabaseConnection)
    connectToMySql();
  else if (isMySqlConnected() && !attemptDatabaseConnection)
    disconnectMySql();
  monitoringActivated = attemptDatabaseConnection;
}

void tryLogMeasures() {
  // try to log measures into db (with loading screen)
  
  loggingLoadingScreen(true);
  logSensorsMeasure();
  loggingLoadingScreen(false);
  updateScreen();
}

void tryGetRoomConfig() {
  // try to get device's room configuration (with loading screen)
  
  dbLoadingScreen(true);
  dbConfigured = setupConfig(&roomId, sensorsId);
  dbLoadingScreen(false);
  updateScreen();
}

void listenForClients() {
  // listen for http requests from a webapp
  
 WiFiClient client = server.available();   // listen for incoming clients
    
 if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n' && currentLine.length() == 0) {                    // if the byte is a newline character
          break;
        }
        else if (c == '\r'){      // if you got a newline, then clear currentLine:
          currentLine = "";
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see which client request was sent
        if (currentLine.endsWith("GET /IsConnected")) {
            String response = "{\"connected\":true, ";
            if(monitoringActivated) {
              response += "\"monitoringActivated\": true}";
            } else {
              response += "\"monitoringActivated\": false}";
            }
            client.print(response);
            client.println();
        }
        if (currentLine.endsWith("GET /StartMonitoring")) {
            client.print("{\"monitoringActivated\": true}");
            monitoringActivated = true;               // GET /StartMonitoring turns on the monitor logging
            attemptDatabaseConnection = true;
            client.println();
        }
        else if (currentLine.endsWith("GET /StopMonitoring")) {
            client.print("{\"monitoringActivated\": false}");
            monitoringActivated = false;             // GET /StartMonitoring turns off the monitor logging
            attemptDatabaseConnection = false;
            client.println();
        }
        else if (currentLine.endsWith("GET /OnTemp")) {
            tempConfig = CONFIG_ON;                 // GET /OnTemp turns on the thermostat
            client.print("{\"temp\": \"on\"}");
            client.println();
        }
        else if (currentLine.endsWith("GET /OffTemp")) {
            tempConfig = CONFIG_OFF;                // GET /OnTemp turns off the thermostat
            client.print("{\"temp\": \"off\"}");
            client.println();

        }
        else if (currentLine.endsWith("GET /OnLight")) {
            lightConfig = CONFIG_ON;                // GET /OnLight turns on the thermostat
            client.print("{\"light\": \"on\"}");
            client.println();

        }
        else if (currentLine.endsWith("GET /OffLight")) {
            lightConfig = CONFIG_OFF;               // GET /OffLight turns off the room's light
            client.print("{\"light\": \"off\"}");
            client.println();

        }
        else if (currentLine.endsWith("GET /AutoLight")) {
            lightConfig = CONFIG_AUTO;              // GET /Autoght turns on/off the room's light automatically based on the light sensor's threshold
            client.print("{\"light\": \"auto\"}");
            client.println();
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

int getPressedButton(){

  byte val_MENO = digitalRead(BUTTON_MENO); // read the meno button state
  byte val_PIU = digitalRead(BUTTON_PIU); // read the piu button state
  byte val_OK = digitalRead(BUTTON_OK); // read the ok button state

  if (val_MENO == HIGH) { // meno button is pressed
    delay(250);
    return MENO;
  } else if (val_PIU == HIGH) { // piu button is pressed
    delay(250);
    return PIU;
  } else if (val_OK == HIGH) { // ok button is pressed
    delay(250);
    return OK;
  } else {
    return NO_OP; // no button is pressed
  }
  
}

void navigate(int pressedButton) {
  switch (pressedButton) {
    case MENO: {
        // navigate to the previous screen (or go to the last screen if you are on the first screen)
        screen = (NUMBER_OF_SCREENS + screen - 1) % NUMBER_OF_SCREENS;
        break;
      }
    case PIU: {
        // navigate to the following screen (or go to the first screen if you are on the last screen)
        screen = (screen + 1) % NUMBER_OF_SCREENS;
        break;
      }
    case OK: {
        // deactivate the navigation mode (ready to pursue actions on the screen)
        navigationMode = false;
        break;
      }
  }
}

void action(int pressedButton) {
  switch (screen) {
      
      // do specific actions for the info screen
      case INFO_SCREEN: {
        actionInfoScreen(pressedButton, &displayRow, &navigationMode, isMySqlConnected(), &attemptDatabaseConnection);
        break;
      }
      
      // do specific actions for the temperature's control screen
      case TEMP_SCREEN: {
        actionTempScreen(pressedButton, &displayRow, &tempActivationThreshold, &tempConfig, &navigationMode);
        break;
      }
      
      // do specific actions for the light's control screen
      case LIGHT_SCREEN: {
        actionLightScreen(pressedButton, &displayRow, &lightActivationThreshold, &lightConfig, &navigationMode);
        break;
      }
      
      // do specific actions for the alarm control screen
      case ALARM_SCREEN: {
        actionAlarmScreen(pressedButton, &navigationMode, &fireAlarm);
        break;
      }
    }
}

void updateSensors() {
    // get new measures
    lastTemp = getTemp();
    lastLight = getLight();
    lastWifiRssi = getWifiRssi();
    
    // update actuators based on thresholds
    updateLight(lastLight, lightConfig, &lightStatus, lightActivationThreshold);
    updateTemp(lastTemp, tempConfig, &tempStatus, tempActivationThreshold);
}

void updateScreen() {
  setNavigationMode(navigationMode);
  switch (screen) {
    // screen update if you are on info screen
    case INFO_SCREEN: {

      updateInfoScreenRows(lastTemp, lastLight, isWifiConnected(), isMySqlConnected());
      updateScreenCursor(!navigationMode, displayRow);
      
      break;
    }
    
    // screen update if you are on temperature's control screen
    case TEMP_SCREEN: {

      updateTempScreenRows(lastTemp, tempConfig, tempActivationThreshold);
      updateScreenCursor(!navigationMode, displayRow);

      break;
    }
    
    // screen update if you are on light's control screen
    case LIGHT_SCREEN: {

      updateLightScreenRows(lightStatus, lightConfig, lightActivationThreshold);
      updateScreenCursor(!navigationMode, displayRow);

      break;
    }
    
    // screen update if you are on alarm's control screen
    case ALARM_SCREEN: {

      updateAlarmScreenRows(fireAlarm);
      updateScreenCursor(!navigationMode, displayRow);

      break;
    }
  }
}

void setHotColdAlarm(int temp) {
  // set alarm if temperature is too hot (fire risk)
  if (temp > tooHotTempThreshold) {

    setTooHotAlarm(true);
    tooColdAlarmMonitored = false;

    if (!tooHotAlarmMonitored && monitoringActivated) {
      // log too hot alarm if monitoring is enabled
    
      char alarmMessage[64] = {0};
      sprintf(alarmMessage, "Too hot! Temp:%d C", temp);
      logAlarm(alarmMessage, HOT_ALARM_CODE, roomId); 
      tooHotAlarmMonitored = true;
    }
     
    // buzzer alarm is activated only if user allows to on the alarm's control page
    if (fireAlarm) {
      setBuzzerAlarm(true);
    } else {
      setBuzzerAlarm(false);
    }
    
  } else {
    // disable eventual activated too hot alarm (freezing risk)
    tooHotAlarmMonitored = false;
    setBuzzerAlarm(false);

    // set alarm if temperature is too cold (fire risk)
    if (temp < tooColdTempThreshold) {
      setTooColdAlarm(true);
      
      if (!tooColdAlarmMonitored && monitoringActivated) {
        // log too cold alarm if monitoring is enabled
        
        char alarmMessage[64] = {0};
        sprintf(alarmMessage, "Too cold! Temp:%d C", temp);
        logAlarm(alarmMessage, COLD_ALARM_CODE, roomId); 
        tooColdAlarmMonitored = true; 
      }
      
    } else {
      // disable eventual activated too cold alarm
      tooColdAlarmMonitored = false; 
    }
    
  }
}

void logSensorsMeasure() {
  
  // for each configured sensor (with id != -1), log last measure
  for (int i = 0; i < 3; i++) {
    if (sensorsId[i] != -1) {
      char value[128] = {0};
      switch(i) {
        case LIGHT_SENSOR: {
          sprintf(value, "%d lux", lastLight);
          break;
        }
        case TEMP_SENSOR: {
          sprintf(value, "%d C", lastTemp);
          break;
        }
        case WIFI_SENSOR: {
          sprintf(value, "%d dB", lastWifiRssi);
          break;
        }
      }
      logSensorMeasure(sensorsId[i], value);
    }
  }
}
