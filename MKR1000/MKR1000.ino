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

//#include "rgb_lcd.h"
#include "macros_room_manager.h"
#include "rgb_lcd_controller_room_manager.h"
#include "navigation_controller_room_manager.h"

//rgb_lcd lcd;
int screen = 0;
boolean navigationMode = true;

int lightStatus = STATUS_OFF;
int lightConfig = CONFIG_OFF;
int lightActivationThreshold = 0;

int tempStatus = STATUS_OFF;
int tempConfig = CONFIG_OFF;
int tempActivationThreshold = 0;

int displayRow = 0;
int last_light=0;
int last_temperature=0;
boolean too_hot_alarm=false;
boolean too_cold_alarm=false;

void setup()
{
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

  // set up the LCD
  setupLcd();
  
  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));

}

void loop()
{
  int pressedButton = getPressedButton();

  if (navigationMode){
    navigate(pressedButton);
  }
  else{
    switch (screen)
    {

      case 1:
        {
          switch (pressedButton)
          {
            case MENO:
              {
                if (displayRow == 0){
                  tempConfig = (NUMBER_OF_CONFIGS + tempConfig - 1) % NUMBER_OF_CONFIGS;
                } else{
                  if (tempActivationThreshold>0){
                    tempActivationThreshold-=1;
                  }
                }
                break;
              }
            case PIU:
              {
                if (displayRow == 0) {
                  tempConfig = (tempConfig + 1) % NUMBER_OF_CONFIGS;
                } else {
                  if (tempActivationThreshold<40){
                    tempActivationThreshold+=1;
                  }
                }
                break;
              }
            case OK:
              {
                displayRow = (displayRow + 1) % 2;
            
                if (displayRow == 0){
                  navigationMode = true;
                }
                break;
              }
          }
          break;
        }

      case 2:
        {
          switch (pressedButton)
          {
            case MENO:
              {
                if (displayRow == 0){
                  lightConfig = (NUMBER_OF_CONFIGS + lightConfig - 1) % NUMBER_OF_CONFIGS;
                } else{
                  if (lightActivationThreshold>0){
                    lightActivationThreshold-=10;
                  }
                }
                break;
              }
            case PIU:
              {
                if (displayRow == 0){
                  lightConfig = (lightConfig + 1) % NUMBER_OF_CONFIGS;
                } else{
                  if (lightActivationThreshold<1000){
                    lightActivationThreshold+=10;
                  }
                }
                break;
              }
            case OK:
              {
                displayRow = (displayRow + 1) % 2;
            
                if (displayRow == 0)
                  navigationMode = true;
                
                break;
              }
          }
          break;
        }
    }
  }

  
  //controllo se i valori sono cambiati
  if (pressedButton != NO_OP  || (int)last_temperature != (int)getTemperature() || (int)getLight()+110 < (int)last_light || (int)getLight()-110 > (int)last_light)
  {
    updateScreen();
    last_temperature=(int)getTemperature();
    last_light=(int)getLight();
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

void navigate(int pressedButton)
{
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

int getTemperature(){
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

void updateScreen()
{
  setNavigationMode(navigationMode);
  switch (screen){
    case 0:{
        
        
        if(getTemperature()>30){
          too_hot_alarm=true;
        }else{
          too_hot_alarm=false;
        }
        if(too_hot_alarm==false){
          lcd.setRGB(0, 255, 0);
        }else{
          lcd.setRGB(255, 0, 0);
        }



        if(getTemperature()<25){
          too_cold_alarm=true;
        }else{
          too_cold_alarm=false;
        }
        if(too_cold_alarm==false){
          lcd.setRGB(0, 255, 0);
        }else{
          lcd.setRGB(0, 0, 255);
        }

        if(too_cold_alarm==true || too_hot_alarm==true){
          digitalWrite(BUZZER_GROVE, HIGH);
        }else{
          digitalWrite(BUZZER_GROVE, LOW);
        }

        
        
        lcd.clear();  // clear text
        lcd.print("TEMP: "); // show text
        lcd.print(getTemperature());
        lcd.setCursor(0, 1);
        lcd.print("LIGHT: "); // show text
        lcd.print(getLight());

        
        break;
      }
    case 1:{
        updateTempScreenRows(tempStatus, tempConfig, tempActivationThreshold);
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
