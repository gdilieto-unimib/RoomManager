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

  // set BUTTON pin as input
  pinMode(BUTTON_PIU, INPUT);

  // set BUTTON pin as input
  pinMode(BUTTON_MENO, INPUT);

  // set BUTTON pin as input
  pinMode(BUTTON_OK, INPUT);

    // set LED pin as output
  pinMode(LIGHT_LED, OUTPUT);
}

int getWifiRssi() {
  return (int) WiFi.RSSI();
}

int getTemp(){
  int a = analogRead(TEMP);

  float R = 1023.0 / ((float)a) - 1.0;
  R = R0 * R;
  float temperature = 1.0 / (log(R / 100000.0) / B + 1 / 298.15);
  temperature = temperature - 273.15;   // kelvin to celsius
  return (int)temperature;
}

int getLight(){
  return analogRead(PHOTORESISTOR);
}

void setLightStatus(int lightStatus) {
  if (lightStatus==LIGHT_STATUS_ON){
    digitalWrite(LIGHT_LED, HIGH);
  }else{
    digitalWrite(LIGHT_LED, LOW);
  }
}

void setTempStatus(int tempStatus) {
  if (tempStatus==TEMP_STATUS_UP){
    digitalWrite(TEMP_LED, HIGH);
  }else if (tempStatus==TEMP_STATUS_DOWN) {
    digitalWrite(TEMP_LED, HIGH);
  } else {
    digitalWrite(TEMP_LED, LOW);
  }
}

void updateLight(int light, int lightConfig, int* lightStatus, int lightActivationThreshold) {
  
  //Se la luce è sotto la treshold impostata, accendo la luce
  switch(lightConfig) {
    case CONFIG_AUTO:{
      if(light<lightActivationThreshold){
      
        *lightStatus=LIGHT_STATUS_ON;
      
      }else{
      
        *lightStatus=LIGHT_STATUS_OFF;
      
      }

      break;
    }
    case CONFIG_ON: {
      *lightStatus = LIGHT_STATUS_ON;
      break;
    }
    case CONFIG_OFF: {
      *lightStatus = LIGHT_STATUS_OFF;
      break;
    }
  }

  setLightStatus(*lightStatus);
  
}

void updateTemp(int temp, int tempConfig, int* tempStatus, int tempActivationThreshold) {
  
  //Se la temperatura è sotto la treshold impostata, aumento o abbasso la temperatura
  switch(tempConfig) {
    case CONFIG_ON: {
      if (temp < tempActivationThreshold) {
        *tempStatus = TEMP_STATUS_UP;
      } else if (temp > tempActivationThreshold) {
        *tempStatus = TEMP_STATUS_DOWN;
      } else {
        *tempStatus = TEMP_STATUS_OFF;
      }
      break;
    }
    case CONFIG_OFF: {
      *tempStatus = TEMP_STATUS_OFF;
    }
  }
  
  setTempStatus(*tempStatus);
  
}

void setBuzzerAlarm(boolean active) {
  digitalWrite(BUZZER_GROVE, active ? HIGH : LOW);
}
