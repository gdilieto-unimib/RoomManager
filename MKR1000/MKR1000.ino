/*
 * ****************************************************************************
 *University of Milano-Bicocca
 *DISCo - Department of Informatics, Systems and Communication
 *Viale Sarca 336, 20126 Milano, Italy
 *
 *Copyright © 2019-2022 by:
 *   Davide Marelli   - davide.marelli@unimib.it
 *   Paolo Napoletano - paolo.napoletano@unimib.it
 * ****************************************************************************
 *
 *Demo code, read the temperature with an Arduino MKR 1000 board.
 *
 *Components:
 *  1x MKR1000
 *  1x MKR connector carrier
 *  1x GROVE 4 pins cable
 *  1x GROVE Temperature Sensor
 *
 *Notes:
 *  - MKR pin mapping -> https://docs.arduino.cc/static/a5b251782d7fa7d0212e7a8b34c45a9e/ABX00004-pinout.png
 *  - Temperature sensor specs:
 *     - Zero power resistance -> 100KOhm
 *     - Resistance Tolerance -> ±1%
 *     - Operating temperature range -> -40 ~ +125 °C
 *     - Nominal B-Constant -> 4250 ~ 4299K
 *
 */

#include "macros_room_manager.h"
#include "rgb_lcd_controller_room_manager.h"
#include "navigation_controller_room_manager.h"
#include "action_controller_room_manager.h"
#include "wifi_controller_room_manager.h"
#include "io_controller_room_manager.h"


int screen = 0;
boolean navigationMode = true;

int lightStatus = STATUS_OFF;
int lightConfig = CONFIG_OFF;
int lightActivationThreshold = 500;

int tempStatus = STATUS_OFF;
int tempConfig = CONFIG_OFF;
int tempActivationThreshold = 28;

int displayRow = 0;
int lastLight=0;
int lastTemp=0;

long time;

void setup()
{
  time=millis();

  setupWiFi();
  setupLcd();
  setupIO();
  
  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));

}

void loop()
{

  // connect to WiFi (if not already connected)
  if (millis()-time > 10000) { connectWifi(); time = millis(); }
 
  int pressedButton = getPressedButton();

  if (navigationMode){
    navigate(pressedButton);
  } else{
    action(screen, pressedButton, &displayRow, &tempActivationThreshold, &lightActivationThreshold, &tempConfig, &lightConfig, &navigationMode);
  }

  int newTemp = getTemp();
  int newLight = getLight();
  
  //controllo se i valori sono cambiati
  if (pressedButton != NO_OP  || lastTemp !=  newTemp || newLight > lastLight+100 || newLight < lastLight-100){

    updateLight(lightConfig, &lightStatus, lightActivationThreshold);
    updateScreen(newTemp, newLight);
    lastTemp=newTemp;
    lastLight=newLight;
    
  }
  
}

int getPressedButton()
{
  
  byte val_MENO = digitalRead(BUTTON_MENO); // read the button state
  byte val_PIU = digitalRead(BUTTON_PIU); // read the button state
  byte val_OK = digitalRead(BUTTON_OK); // read the button state

  if (val_MENO == HIGH){
    Serial.print("Bottone premuto1");
    delay(250);
    return MENO;
  }
  else if (val_PIU == HIGH){
    Serial.print("Bottone premuto2");
    delay(250);
    return PIU;
  }
  else if (val_OK == HIGH){
    Serial.print("Bottone premuto3");
    delay(250);
    return OK;
  }
  else{
    return NO_OP;
  }
}

void navigate(int pressedButton) {
  switch (pressedButton){
    case MENO:{
        screen = (NUMBER_OF_SCREENS + screen - 1) % NUMBER_OF_SCREENS;
        break;
      }
    case PIU:{
        screen = (screen + 1) % NUMBER_OF_SCREENS;
        break;
      }
    case OK:{
        if (screen != 0)
          navigationMode = false;
        break;
      }
  }
}



void updateScreen(int temp, int light)
{
  setNavigationMode(navigationMode);
  switch (screen){
    case 0:{
 
        setHotColdAlarm(temp);
        updateInfoScreenRows(temp, light, isWifiConnected());

        break;
      }
    case 1:{
      
        updateTempScreenRows(temp, tempConfig, tempActivationThreshold);
        updateScreenCursor(!navigationMode, displayRow);
        
        break;
      }
    case 2:{        
      
        updateLightScreenRows(lightStatus, lightConfig, lightActivationThreshold);
        updateScreenCursor(!navigationMode, displayRow);
        
        break;
      }
  }
}

void setHotColdAlarm(int temp) {
  if(temp>30){
          
    setTooHotAlarm(true);
    setBuzzerAlarm(true);
    
  } else {
    
    setBuzzerAlarm(false);

    if(temp<25) {
      setTooColdAlarm(true);  
    } else {
      setTooColdAlarm(false);
    }
    
  }
}
