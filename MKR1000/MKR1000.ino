/*
 * ****************************************************************************
  University of Milano-Bicocca
  DISCo - Department of Informatics, Systems and Communication
  Viale Sarca 336, 20126 Milano, Italy

  Assignment 1 for IOT Laboratory classroom by:
     Gabriele Di Lieto - g.dilieto@unimib.it
     Tiberio Falsiroli - t.falsiroli@unimib.it
 * ****************************************************************************

  Room manager: an IOT application that allows you to monitorate and control you room's temperature and light sensors and actuators.

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

int screen = 0;
boolean navigationMode = true;

int lightStatus = LIGHT_STATUS_OFF;
int lightConfig = CONFIG_OFF;
int lightActivationThreshold = 500;

int tempStatus = TEMP_STATUS_OFF;
int tempConfig = CONFIG_OFF;
int tempActivationThreshold = 28;

int displayRow = 0;
int lastLight = 0;
int lastTemp = 0;
int lastWifiRssi = 0;

boolean attemptDatabaseConnection = true;
boolean tooHotAlarmMonitored = false;
boolean tooColdAlarmMonitored = false;

boolean fireAlarm = true;
boolean monitoringActivated = false;
boolean dbConfigured = false;

int roomId = -1;

//Positions of ids ordered alphabetically for types: 1° - Light, 2° - Temperature, 3° - Wifi
int sensorsId[3] = { -1, -1, -1};

boolean firstStartWifi = true;
boolean firstStartConfig= true;
boolean startServer= true;

long timeDb;
long timeWifi;
long timeSensors;
long timeLogging;
long timeConfig;

int status = WL_IDLE_STATUS;
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
    if (startServer) {
      server.begin();
      startServer = false;
    }
    listenForEthernetClients();
  }
  
  // connect to WiFi (if not already connected) each 10 seconds
  if (( (millis() - timeWifi) > 10000 ) && !isWifiConnected()) {
    tryWifiConnection();
    timeWifi = millis();
  }
  
  // toggle connection to MySql database each 10 seconds
  if (( (millis() - timeDb) > 10000 ) && isWifiConnected()){
    tryToggleDbConnection();
    timeDb = millis();
  }
  
  // get configuration from database (if not already connected)
  if (( (millis() - timeConfig) > 10000 ) && isWifiConnected() && !dbConfigured) {
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
      updateScreenAndSensors();
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
  monitoringActivated = false;
  startServer = true;
  wifiLoadingScreen(true);
  connectWifi();
  wifiLoadingScreen(false);
  updateScreen();
}

void tryToggleDbConnection() {
  if (!isMySqlConnected() && attemptDatabaseConnection)
    connectToMySql();
  else if (isMySqlConnected() && !attemptDatabaseConnection)
    disconnectMySql();
}

void tryLogMeasures() {
  loggingLoadingScreen(true);
  logSensorsMeasure();
  loggingLoadingScreen(false);
  updateScreen();
}

void tryGetRoomConfig() {
  dbLoadingScreen(true);
  dbConfigured = setupConfig(&roomId, sensorsId);
  dbLoadingScreen(false);
  updateScreen();
}

void listenForEthernetClients() {
    WiFiClient client = server.available();   // listen for incoming clients
    
 if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:application/json");
            client.println();

            // the content of the HTTP response follows the header:
            //client.print("Click <a href=\"/L\">here</a> turn the Built in LED off<br>");

            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see which client request was done
        if (currentLine.endsWith("GET /IsConnected")) {
            client.print("{connected:true, ");
            if(isMySqlConnected()) {
              client.print("monitoringActivated: true}");
            } else {
              client.print("monitoringActivated: false}");
            }
        }
        if (currentLine.endsWith("GET /StartMonitoring")) {
            client.print("{monitoringActivated: true}");
            monitoringActivated = true;               // GET /StartMonitoring turns on the monitor logging
        }
        else if (currentLine.endsWith("GET /StopMonitoring")) {
            client.print("{monitoringActivated: false}");
            monitoringActivated = false;             // GET /StartMonitoring turns off the monitor logging
            disconnectMySql();
        }
        else if (currentLine.endsWith("GET /OnTemp")) {
            tempConfig = CONFIG_ON;
            client.print("{temp: 'on'}");
        }
        else if (currentLine.endsWith("GET /OffTemp")) {
            tempConfig = CONFIG_OFF;
            client.print("{temp: 'off'}");

        }
        else if (currentLine.endsWith("GET /OnLight")) {
            tempConfig = CONFIG_ON;
            client.print("{light: 'on'}");

        }
        else if (currentLine.endsWith("GET /OffLight")) {
            tempConfig = CONFIG_OFF;
            client.print("{light: 'off'}");

        }
        else if (currentLine.endsWith("GET /AutoLight")) {
            tempConfig = CONFIG_AUTO;
            client.print("{light: 'auto'}");
        }
        // The HTTP response ends with another blank line:
        client.println();
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

int getPressedButton(){

  byte val_MENO = digitalRead(BUTTON_MENO); // read the button state
  byte val_PIU = digitalRead(BUTTON_PIU); // read the button state
  byte val_OK = digitalRead(BUTTON_OK); // read the button state

  if (val_MENO == HIGH) {
    delay(250);
    return MENO;
  } else if (val_PIU == HIGH) {
    delay(250);
    return PIU;
  } else if (val_OK == HIGH) {
    delay(250);
    return OK;
  } else {
    return NO_OP;
  }
  
}

void navigate(int pressedButton) {
  switch (pressedButton) {
    case MENO: {
        screen = (NUMBER_OF_SCREENS + screen - 1) % NUMBER_OF_SCREENS;
        break;
      }
    case PIU: {
        screen = (screen + 1) % NUMBER_OF_SCREENS;
        break;
      }
    case OK: {
          navigationMode = false;
      }
  }
}

void action(int pressedButton) {
  switch (screen) {
      case TEMP_SCREEN: {
        actionTempScreen(pressedButton, &displayRow, &tempActivationThreshold, &tempConfig, &navigationMode);
        break;
      }

      case INFO_SCREEN: {
        actionInfoScreen(pressedButton, &displayRow, &navigationMode, isMySqlConnected(), &attemptDatabaseConnection);
        break;
      }

      case LIGHT_SCREEN: {
        actionLightScreen(pressedButton, &displayRow, &lightActivationThreshold, &lightConfig, &navigationMode);
        break;
      }
      
      case ALARM_SCREEN: {
        actionAlarmScreen(pressedButton, &navigationMode, &fireAlarm);
        break;
      }
    }
}

void updateScreenAndSensors() {
    int newTemp = getTemp();
    int newLight = getLight();
    int newWifiRssi = getWifiRssi();

    updateScreen();
    updateLight(newLight, lightConfig, &lightStatus, lightActivationThreshold);
    updateTemp(newTemp, tempConfig, &tempStatus, tempActivationThreshold);
    
    lastTemp = newTemp;
    lastLight = newLight;
    lastWifiRssi = newWifiRssi;

    timeSensors = millis();
}

void updateScreen() {
  setNavigationMode(navigationMode);
  switch (screen) {
    case INFO_SCREEN: {

      updateInfoScreenRows(lastTemp, lastLight, isWifiConnected(), isMySqlConnected());
      updateScreenCursor(!navigationMode, displayRow);
      
      break;
    }
    case TEMP_SCREEN: {

      updateTempScreenRows(lastTemp, tempConfig, tempActivationThreshold);
      updateScreenCursor(!navigationMode, displayRow);

      break;
    }
    case LIGHT_SCREEN: {

      updateLightScreenRows(lightStatus, lightConfig, lightActivationThreshold);
      updateScreenCursor(!navigationMode, displayRow);

      break;
    }
    case ALARM_SCREEN: {

      updateAlarmScreenRows(fireAlarm);
      updateScreenCursor(!navigationMode, displayRow);

      break;
    }
  }
}

void setHotColdAlarm(int temp) {
  if (temp > 35) {

    setTooHotAlarm(true);
    tooColdAlarmMonitored = false;
    
    if (!tooHotAlarmMonitored && monitoringActivated) {
      char alarmMessage[64] = {0};
      sprintf(alarmMessage, "Too hot! Temp:%d C", temp);
      logAlarm(alarmMessage, HOT_ALARM_CODE, roomId); 
      tooHotAlarmMonitored = true;
    }
     
    if (fireAlarm) {
      setBuzzerAlarm(true);
    } else {
      setBuzzerAlarm(false);
    }
    
  } else {
    
    tooHotAlarmMonitored = false;
    setBuzzerAlarm(false);

    if (temp < 25) {
      setTooColdAlarm(true);
      
      if (!tooColdAlarmMonitored && monitoringActivated) {
        char alarmMessage[64] = {0};
        sprintf(alarmMessage, "Too cold! Temp:%d C", temp);
        logAlarm(alarmMessage, COLD_ALARM_CODE, roomId); 
        tooColdAlarmMonitored = true; 
      }
      
    } else {
      tooColdAlarmMonitored = false; 
    }
    
  }
}

void logSensorsMeasure() {
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
