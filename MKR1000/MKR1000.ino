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

boolean tooHotAlarmMonitored = false;
boolean tooColdAlarmMonitored = false;

boolean fireAlarm = true;
boolean deviceConfigured = false;
boolean monitoringActivated = true;

int roomId = -1;

//Positions of ids ordered alphabetically for types: 1° - Light, 2° - Temperature, 3° - Wifi
int sensorsId[3] = { -1, -1, -1};

boolean firstStartWifi = true;
boolean firstStartDb= true;

long timeDb;
long timeWifi;
long timeSensors;
long timeLogging;

void setup()
{
  timeWifi = timeDb = timeSensors = timeLogging = millis();
  setupWiFi();
  setupLcd();
  setupIO();
  
  lastLight = getLight();
  lastTemp = getTemp();
  lastWifiRssi = getWifiRssi();
  
  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
}

void loop()
{
  // connect to WiFi (if not already connected)
  if (!isWifiConnected() && ( (millis() - timeWifi > 10000) || firstStartWifi )) {
    wifiLoadingScreen(true);
    connectWifi();
    wifiLoadingScreen(false);
    updateScreen();
    firstStartWifi = false;
    timeWifi = millis();
  }
  
  // connect to database (if not already connected)
  if (( (millis() - timeDb) > 10000 || firstStartDb ) && isWifiConnected() && !deviceConfigured) {
    dbLoadingScreen(true);
    deviceConfigured = setupConfig(&roomId, sensorsId);
    dbLoadingScreen(false);
    updateScreen();
    firstStartDb = false;
    timeDb = millis();
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
  if ( (millis() - timeSensors) > 1000 ) {    
      updateScreenAsSensorsChange();
      timeSensors = millis();
  }

  //log (each ten seconds) measures of the sensors
  if ( (millis() - timeLogging) > 5000 && deviceConfigured && monitoringActivated ) {
      loggingLoadingScreen(true);
      logSensorsMeasure();
      loggingLoadingScreen(false);
      updateScreen();
      timeLogging = millis();
  }

  //set hot or cold alarm
  setHotColdAlarm(lastTemp);
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
        if (screen != 0)
          navigationMode = false;
        break;
      }
  }
}

void action(int pressedButton) {
  switch (screen) {
      case TEMP_SCREEN: {
        actionTempScreen(pressedButton, &displayRow, &tempActivationThreshold, &tempConfig, &navigationMode);
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

void updateScreenAsSensorsChange() {
    int newTemp = getTemp();
    int newLight = getLight();
    int newWifiRssi = getWifiRssi();

    if (lastTemp !=  newTemp || newLight > lastLight + 100 || newLight < lastLight - 100){
      updateScreen();
      updateLight(newLight, lightConfig, &lightStatus, lightActivationThreshold);
      updateTemp(newTemp, tempConfig, &tempStatus, tempActivationThreshold);
      
      lastTemp = newTemp;
      lastLight = newLight;
      lastWifiRssi = newWifiRssi;
    }

    timeSensors = millis();
}

void updateScreen() {
  setNavigationMode(navigationMode);
  switch (screen) {
    case INFO_SCREEN: {

      updateInfoScreenRows(lastTemp, lastLight, isWifiConnected(), deviceConfigured);

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
