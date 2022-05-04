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
#define BUTTON_PIU 3  // D4, digital pin used to read the button state
#define BUTTON_MENO 2 // D3, digital pin used to read the button state
#define BUTTON_OK 1
#define BUZZER_GROVE 0  // D0, digital pin used to drive the buzzer
#define TEMP A0 // A0, analog pin used to read the temperature
#define PHOTORESISTOR A1 // 1, analog pin used to read the light
#define LED 4 // D4, digital pin used for the led

#define PIU 3
#define MENO 2
#define OK 1
#define NO_OP 0
#define NUMBER_OF_SCREENS 3
#define B 4275  // B-constant of the thermistor
#define R0 100000 // R0 = 100KOhm
#include "rgb_lcd.h"

rgb_lcd lcd;
int screen = 0;
boolean navigationMode = true;
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
  lcd.begin(16, 2); // 16 cols, 2 rows
  lcd.setRGB(255, 255, 255);

  Serial.begin(115200);
  Serial.println(F("\n\nSetup completed.\n\n"));

}

void loop()
{
  int pressedButton = getPressedButton();

  if (navigationMode)
  {
    navigate(pressedButton);
  }
  else
  {
    switch (screen)
    {

      case 1:
        {
          switch (pressedButton)
          {
            case MENO:
              {

                break;
              }
            case PIU:
              {

                break;
              }
            case OK:
              {
                navigationMode = true;
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

                break;
              }
            case PIU:
              {

                break;
              }
            case OK:
              {
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

  if (val_MENO == HIGH)
  {
    Serial.print("Bottone premuto1");
    delay(250);
    return MENO;
  }
  else if (val_PIU == HIGH)
  {
    Serial.print("Bottone premuto2");
    delay(250);
    return PIU;
  }
  else if (val_OK == HIGH)
  {
    Serial.print("Bottone premuto3");
    delay(250);
    return OK;
  }
  else
  {
    return NO_OP;
  }
}

void navigate(int pressedButton)
{
  switch (pressedButton)
  {
    case MENO:
      {
        screen = (NUMBER_OF_SCREENS + screen - 1) % NUMBER_OF_SCREENS;
        break;
      }
    case PIU:
      {
        screen = (screen + 1) % NUMBER_OF_SCREENS;
        break;
      }
    case OK:
      {
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
  Serial.print(screen);
  switch (screen)
  {
    case 0:
      {
        
        
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
          igitalWrite(BUZZER_GROVE, HIGH);
        }else{
          igitalWrite(BUZZER_GROVE, LOW);
        }

        
        
        lcd.clear();  // clear text
        lcd.print("TEMP: "); // show text
        lcd.print(getTemperature());
        lcd.setCursor(0, 1);
        lcd.print("LIGHT: "); // show text
        lcd.print(getLight());

        break;
      }
    case 1:
      {
        Serial.println("Schermata 1");
        lcd.clear();  // clear text
        lcd.print("Schermata 1"); // show text
        break;
      }
    case 2:
      {
        Serial.println("Schermata 2");
        lcd.clear();  // clear text
        lcd.print("Schermata 2"); // show text
        break;
      }
  }
}
