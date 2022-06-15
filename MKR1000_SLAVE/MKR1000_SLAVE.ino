/*
 * ****************************************************************************
  University of Milano-Bicocca
  DISCo - Department of Informatics, Systems and Communication
  Viale Sarca 336, 20126 Milano, Italy

  Assignment 1 for IOT Laboratory classroom by:
     Gabriele Di Lieto - g.dilieto@unimib.it
     Tiberio Falsiroli - t.falsiroli@unimib.it
 * ****************************************************************************

  Room manager: an  IOT application that allows you to monitorate your rooms' temperature and light sensors and control your actuators (thermostats and lights).

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
#include <ArduinoJson.h>

#include <ArduinoLowPower.h>

#include "macros_room_manager.h"

#include "rgb_lcd_controller_room_manager.h"

#include "action_controller_room_manager.h"

#include "wifi_controller_room_manager.h"

#include "io_controller_room_manager.h"

#include "mqtt_controller_room_manager.h"

#include "accesspoint_controller_room_manager.h"

#include "flashmem_controller_room_manager.h"


int screen = INFO_SCREEN;
boolean navigationMode = true;

int lightStatus = LIGHT_STATUS_OFF;
int lightConfig = CONFIG_OFF;
int lightActivationThreshold = 500;

int tempStatus = TEMP_STATUS_OFF;
int tempConfig = CONFIG_OFF;
int tempActivationThreshold = 28;

int tooHotTempThreshold = 30;
int tooColdTempThreshold = 10;

int displayRow = 0;
int lastLight = 0;
int lastTemp = 0;
int lastWifiRssi = 0;
int externalTemperature = 0;

int sleepCycleDuration = 0;
int lowPowerModeMillis = 0;

long scheduleDuration = 0;

boolean tooHotAlarmMonitored = false;
boolean tooColdAlarmMonitored = false;
boolean fireAlarm = true;
boolean low_Power_Mode = false;
boolean lowPowerButtonPressed = false;
boolean mode_just_changed = false;
boolean ecoMode = false;

boolean monitoringActivated = false;
boolean startServer = true;
boolean configureWifi = false;

FlashStorage(my_flash_store, WiFi_Credentials);
WiFi_Credentials MyWiFi_Credentials;

long timeDb, timeWifi, timeSensors, timeLogging, timeConfig, timeMqtt, timeSendHearthbeat;

void setup() {

  timeWifi = timeDb = timeSensors = timeLogging = timeConfig = timeMqtt = timeSendHearthbeat = millis();
  setupLcd();
  setupIO();
  MQTTSetup(&monitoringActivated, &tempConfig, &lightConfig, &externalTemperature, &ecoMode , &scheduleDuration);
  updateSensors();

  MyWiFi_Credentials = my_flash_store.read();

  if (!MyWiFi_Credentials.valid && configureWifi)
    setupAP();

  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));
  LowPower.attachInterruptWakeup(BUTTON_OK, wakeUp, CHANGE);
}

void loop() {
  if (low_Power_Mode  &&  (getTemp() > tooHotTempThreshold == false) ) {
    lowPowerModeMillis = millis();
    mode_just_changed = true;
    lowPowerModeLCD();

    if (getTemp() > tooHotTempThreshold) {
      Serial.print("ALLARME TROPPO CALDO DA LOW POWER MODE");
      low_Power_Mode = false;
    }

    Serial.println("\n\nBEGIN SLEEPING");
    sleepCycleDuration = sleepCycleDuration + CONTROL_FREQUENCE_LOW_POWER_MODE;
    lowPowerModeMillis = millis() - lowPowerModeMillis;
    sleepCycleDuration = sleepCycleDuration + lowPowerModeMillis;

    LowPower.sleep(CONTROL_FREQUENCE_LOW_POWER_MODE);

    lowPowerModeMillis = millis();
    Serial.println("STOP SLEEPING");
    Serial.print("Total low power time: ");
    Serial.print(sleepCycleDuration);
    Serial.println(" MILLISECONDS");

    Serial.print("Temperature: ");
    Serial.println(getTemp());


    if (scheduleDuration > 0) {
      Serial.print("Time reamaining until wakeup: ");
      Serial.println(scheduleDuration - sleepCycleDuration);
      if (sleepCycleDuration > scheduleDuration )low_Power_Mode = false;
    }

    lowPowerModeMillis = millis() - lowPowerModeMillis;
    sleepCycleDuration = sleepCycleDuration + lowPowerModeMillis;

  } else {
    if (mode_just_changed) {
      delay(1000);
      mode_just_changed = false;
      Serial.print("Resetto durata lowPowerModeCycle");
      sleepCycleDuration = 0;
      scheduleDuration = 0;

      //screen = INFO_SCREEN;
      notLowPowerModeLCD();
    }

    if (scheduleDuration > 0) {
      if (getTemp() > tooHotTempThreshold == false) low_Power_Mode = true;
    }

    // Connect to wifi
    tryWifiConnection();

    // Connect to mqtt broker
    tryMQTTBrokerConnection();

    // Log measures to master
    tryLogMeasures();

    // Send hearthbeat if connected to wifi and mqtt broker to get configuration
    trySendRoomHearthbeat();

    // Loop for mqtt messages
    loopMqttClient();

    // get the actual pressed button
    int pressedButton = getPressedButton();

    if (navigationMode) {
      // if navigating though screen
      navigate(pressedButton);
    } else {
      // if acting on a screen
      action(pressedButton);
    }

    // update screen if an operation has been made
    if (pressedButton != NO_OP) {
      updateScreen();
    }

    // refresh sensors and screen periodically
    refreshScreenInfo();

    //set hot or cold alarm
    setHotColdAlarm(lastTemp);

  }
}
void tryWifiConnection() {
  String password;

  // try to connect to wifi (with loading screen)

  if (((millis() - timeWifi) > 10000) && !isWifiConnected()) {
    wifiLoadingScreen(true);
    if (MyWiFi_Credentials.valid == true) {
      Serial.println("Loading existing WiFi credentials");
      connectWifi(MyWiFi_Credentials.ssid_RM, MyWiFi_Credentials.pssw_RM);

    } else {
      Serial.println("Waiting for WiFi credentials");
      if (!configureWifi) {
        connectWifi(SECRET_SSID, SECRET_PASS);
      } else {
        password = connectToWifiAP();
        if (isWifiConnected()) {
          MyWiFi_Credentials.valid = true;
          String ssidfl = WiFi.SSID();

          ssidfl.toCharArray(MyWiFi_Credentials.ssid_RM, 100);
          password.toCharArray(MyWiFi_Credentials.pssw_RM, 100);

          Serial.println("Writing WiFi credentials");
          my_flash_store.write(MyWiFi_Credentials);
          delay(1000);
        }
      }

      // if wifi was disconnected, monitoring is resetted
      monitoringActivated = false;

      // try to connect to wifi
      wifiLoadingScreen(false);
      updateScreen();
      timeWifi = millis();
    }
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

void tryLogMeasures() {
  // try to log measures into db (with loading screen)

  if (((millis() - timeMqtt) > 10000) && isWifiConnected() && isRoomConfigured() && monitoringActivated) {
    loggingLoadingScreen(true);
    mqttSendData(lastTemp, lastLight, lastWifiRssi);
    loggingLoadingScreen(false);
    updateScreen();

    timeMqtt = millis();
  }
}

void trySendRoomHearthbeat() {
  // try to get device's room configuration (with loading screen)

  if (((millis() - timeSendHearthbeat) > 10000) && isWifiConnected() && isMQTTBrokerConnected()) {
    // try to send an hearthbeat to master
    mqttSendMac();

    timeSendHearthbeat = millis();
  }
}

int getPressedButton() {

  byte val_MENO = digitalRead(BUTTON_MENO); // read the meno button state
  byte val_PIU = digitalRead(BUTTON_PIU); // read the piu button state
  byte val_OK = digitalRead(BUTTON_OK); // read the ok button state

  if (val_MENO == HIGH && val_PIU == HIGH) {
    delay(2000);
    if (val_MENO == HIGH && val_PIU == HIGH) {
      low_Power_Mode = !low_Power_Mode;
      Serial.print("Changing power mode to: ");
      Serial.println(low_Power_Mode);

      delay(3000);
    }
  }

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
        actionInfoScreen(pressedButton, & displayRow, & navigationMode, & monitoringActivated);
        break;
      }

    // do specific actions for the temperature's control screen
    case TEMP_SCREEN: {
        actionTempScreen(pressedButton, & displayRow, & tempActivationThreshold, & tempConfig, & navigationMode);
        break;
      }

    // do specific actions for the light's control screen
    case LIGHT_SCREEN: {
        actionLightScreen(pressedButton, & displayRow, & lightActivationThreshold, & lightConfig, & navigationMode);
        break;
      }

    // do specific actions for the alarm control screen
    case ALARM_SCREEN: {
        actionAlarmScreen(pressedButton, & navigationMode, & fireAlarm);
        break;
      }
  }
}

void refreshScreenInfo() {
  if ((millis() - timeSensors) > 2000) {
    updateSensors();
    updateScreen();

    timeSensors = millis();
  }
}

void updateSensors() {
  // get new measures
  lastTemp = getTemp();
  lastLight = getLight();
  lastWifiRssi = getWifiRssi();

  // update actuators based on thresholds
  updateLight(lastLight, lightConfig, & lightStatus, lightActivationThreshold);
  updateTemp(lastTemp, tempConfig, & tempStatus, tempActivationThreshold, ecoMode, externalTemperature);
}

void updateScreen() {
  setNavigationMode(navigationMode);
  switch (screen) {
    // screen update if you are on info screen
    case INFO_SCREEN: {

        updateInfoScreenRows(lastTemp, lastLight, isWifiConnected(), monitoringActivated, ecoMode);
        updateScreenCursor(!navigationMode, displayRow);

        break;
      }

    // screen update if you are on temperature's control screen
    case TEMP_SCREEN: {

        updateTempScreenRows(lastTemp, tempConfig, tempActivationThreshold, tempStatus);
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

      char alarmMessage[64] = {
        0
      };
      sprintf(alarmMessage, "Too hot! Temp:%d C", temp);
      mqttSendAlarm(alarmMessage, HOT_ALARM_CODE);
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

        char alarmMessage[64] = {
          0
        };
        sprintf(alarmMessage, "Too cold! Temp:%d C", temp);
        mqttSendAlarm(alarmMessage, COLD_ALARM_CODE);
        tooColdAlarmMonitored = true;
      }

    } else {
      // disable eventual activated too cold alarm
      tooColdAlarmMonitored = false;
    }

  }
}

void wakeUp() {
  low_Power_Mode = false;
  sleepCycleDuration = 0;
}
