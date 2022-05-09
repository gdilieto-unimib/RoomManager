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
  pinMode(LED, OUTPUT);
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

void setLight(int lightStatus) {
  if (lightStatus==STATUS_ON){
    digitalWrite(LED, HIGH);
  }else{
    digitalWrite(LED, LOW);
  }
}

void updateLight(int lightConfig, int* lightStatus, int lightActivationThreshold) {
  
  //Se la luce è sotto la treshold impostata, accendo la luce
  switch(lightConfig) {
    case CONFIG_AUTO:{
      if(getLight()<lightActivationThreshold){
      
        *lightStatus=STATUS_ON;
      
      }else{
      
        *lightStatus=STATUS_OFF;
      
      }

      break;
    }
    case CONFIG_ON: {
      *lightStatus = STATUS_ON;
      break;
    }
    case CONFIG_OFF: {
      *lightStatus = STATUS_OFF;
      break;
    }
  }

  setLight(*lightStatus);
  
}

void setBuzzerAlarm(boolean active) {
  digitalWrite(BUZZER_GROVE, active ? HIGH : LOW);
}
