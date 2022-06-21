#include "io_controller_room_manager.h"

void setupIO() {
  // set buzzer pin as output
  pinMode(BUZZER_GROVE, OUTPUT);

  // turn buzzer off
  digitalWrite(BUZZER_GROVE, LOW);

  // set Light sensor pin as input
  pinMode(PHOTORESISTOR, INPUT);

  // set pin as input
  pinMode(TEMP, INPUT);

  // set BUTTON_PIU pin as input
  pinMode(BUTTON_PIU, INPUT);

  // set BUTTON_MENOpin as input
  pinMode(BUTTON_MENO, INPUT);

  // set BUTTON_OK pin as input
  pinMode(BUTTON_OK, INPUT);

  // set BUTTON_WAKE_UP_LOWPOWERMODE pin as input
  pinMode(BUTTON_WAKE_UP_LOWPOWERMODE, INPUT);

  // set COLD_TEMP_LED pin as output
  pinMode(COLD_TEMP_LED, OUTPUT);

  // set HOT_TEMP_LED pin as output
  pinMode(HOT_TEMP_LED, OUTPUT);

  // set LIGHT_LED pin as output
  pinMode(LIGHT_LED, OUTPUT);
}

int getWifiRssi() {
  // get wifi rssi's value in decibel

  return (int) WiFi.RSSI();
}

int getTemp() {
  // get temperature's measure in celsius

  int a = analogRead(TEMP);

  float R = 1023.0 / ((float)a) - 1.0;
  R = R0 * R;
  float temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15);
  temperature = temperature - 273.15;   // kelvin to celsius
  return (int)temperature;
}

int getLight() {
  // get light's value in lux

  return analogRead(PHOTORESISTOR);
}

void setLightStatus(int lightStatus) {
  // set light's actuator based on light status

  if (lightStatus == LIGHT_STATUS_ON) {
    digitalWrite(LIGHT_LED, HIGH);
  } else {
    digitalWrite(LIGHT_LED, LOW);
  }
}

void setTempStatus(int tempStatus) {
  // set thermostat's actuators based on thermostat status

  if (tempStatus == TEMP_STATUS_UP) {
    //warm up temperature (using led to simulate)
    digitalWrite(HOT_TEMP_LED, HIGH);
  } else if (tempStatus == TEMP_STATUS_DOWN) {
    //cool down temperature (using led to simulate)
    digitalWrite(COLD_TEMP_LED, HIGH);
  } else {
    //temperature is ok or thermostat is configured to be off
    digitalWrite(HOT_TEMP_LED, LOW);
    digitalWrite(COLD_TEMP_LED, LOW);
  }
}

void updateLight(int light, int lightConfig, int* lightStatus, int lightActivationThreshold) {

  switch (lightConfig) {
    // set ligh's status automatically based on light's activaiton threshold
    case CONFIG_AUTO: {
        if (light < lightActivationThreshold) {

          *lightStatus = LIGHT_STATUS_ON;

        } else {

          *lightStatus = LIGHT_STATUS_OFF;

        }

        break;
      }
    // set ligh's status on
    case CONFIG_ON: {
        *lightStatus = LIGHT_STATUS_ON;
        break;
      }
    // set ligh's status off
    case CONFIG_OFF: {
        *lightStatus = LIGHT_STATUS_OFF;
        break;
      }
  }

  // set ligh's actuator
  setLightStatus(*lightStatus);

}

void updateTemp(int temp, int tempConfig, int* tempStatus, int tempActivationThreshold, boolean ecoMode, int externalTemperature) {

  switch (tempConfig) {
    // set thermostat's status automatically based on temperature's activation threshold

    case CONFIG_ON: {
        if (ecoMode == true && temp < tempActivationThreshold && externalTemperature >= tempActivationThreshold && externalTemperature >= (temp + TEMPERATURE_ISOLATION_CONSTANT) ) {
          *tempStatus = TEMP_STATUS_ECO;

        } else if (ecoMode == true && temp > tempActivationThreshold && externalTemperature <= tempActivationThreshold && externalTemperature <= (temp - TEMPERATURE_ISOLATION_CONSTANT) ) {
          *tempStatus = TEMP_STATUS_ECO;

        } else if (temp < tempActivationThreshold) {
          // UP to increase temperature
          *tempStatus = TEMP_STATUS_UP;
        } else if (temp > tempActivationThreshold) {
          // DOWN to decrease temperature
          *tempStatus = TEMP_STATUS_DOWN;
        } else {
          // stay off if the correct temperature is reached
          *tempStatus = TEMP_STATUS_OFF;
        }
        break;
      }
    // set thermostat's status off
    case CONFIG_OFF: {
        *tempStatus = TEMP_STATUS_OFF;
      }
  }

  // set thermostat's actuator
  setTempStatus(*tempStatus);

}

void setBuzzerAlarm(boolean active) {
  // set the buzzer for fire alarm

  digitalWrite(BUZZER_GROVE, active ? HIGH : LOW);
}
