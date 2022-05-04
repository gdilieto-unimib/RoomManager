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
#define BUTTON_PIU 3  // D3, digital pin used to read the button state
#define BUTTON_MENO 2 // D2, digital pin used to read the button state
#define BUTTON_OK 1 // D1, digital pin used to read the button state
#define BUZZER_GROVE 0  // D0, digital pin used to drive the buzzer
#define TEMP A0 // A0, analog pin used to read the temperature
#define LIGHT A1 // 1, analog pin used to read the light
#define LED_GROVE 4   // D4, digital pin used to drive the LED

#define PIU 3 // Action when button piu is pressed
#define MENO 2 // Action when button meno is pressed
#define OK 1 // Action when button ok is pressed
#define NO_OP 0 // Action when no button is pressed
#define NUMBER_OF_SCREENS 3
#define B 4275  // B-constant of the thermistor
#define R0 100000 // R0 = 100KOhm

#define LIGHT_STATUS_OFF 0
#define LIGHT_STATUS_ON 1

#define LIGHT_CONFIG_OFF 0
#define LIGHT_CONFIG_ON 1
#define LIGHT_CONFIG_AUTO 2
#define NUMBER_OF_LIGHT_CONFIGS 3

#include "rgb_lcd.h"

rgb_lcd lcd;
int screen = 0;
boolean navigationMode = true;
int lightStatus = LIGHT_STATUS_OFF;
int lightConfig = LIGHT_CONFIG_OFF;
int displayRow = 0;

void setup()
{
  // set buzzer pin as output
  pinMode(BUZZER_GROVE, OUTPUT);

  // turn buzzer off
  digitalWrite(BUZZER_GROVE, LOW);

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
                if (displayRow == 0)
                  lightConfig = (NUMBER_OF_LIGHT_CONFIGS + lightConfig - 1) % NUMBER_OF_LIGHT_CONFIGS;
                break;
              }
            case PIU:
              {
                if (displayRow == 0)
                  lightConfig = (lightConfig + 1) % NUMBER_OF_LIGHT_CONFIGS;
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

  if (pressedButton != NO_OP)
  {
    updateScreen();
  }
}

int getPressedButton()
{

  byte val_MENO = digitalRead(BUTTON_MENO); // read the button state
  byte val_PIU = digitalRead(BUTTON_PIU); // read the button state
  byte val_OK = digitalRead(BUTTON_OK); // read the button state

  if (val_MENO == HIGH)
  {
    delay(250);
    return MENO;
  }
  else if (val_PIU == HIGH)
  {
    delay(250);
    return PIU;
  }
  else if (val_OK == HIGH)
  {
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

void updateScreen()
{
  if (navigationMode) {
    lcd.setRGB(50,50,50);
  } else {
    lcd.setRGB(255,255,255);
  }
  switch (screen)
  {
    case 0:
      {
        Serial.println("Schermata 0");
        lcd.clear();  // clear text
        lcd.print("Schermata 0"); // show text
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
        char lightScreenRows[2][16] = {"Light: ", "Level: 100"};
        
        if (lightConfig == LIGHT_CONFIG_AUTO) {
          strcat(lightScreenRows[0], lightStatus == LIGHT_STATUS_ON ? "AUTO-ON" : "AUTO-OFF");
        } else {
          strcat(lightScreenRows[0], lightConfig == LIGHT_CONFIG_ON ? "ON" : "OFF");
        }
        lcd.clear();  // clear text
        lcd.setCursor(0,0);
        lcd.print(lightScreenRows[0]); // show text
        lcd.setCursor(0,1);
        lcd.print(lightScreenRows[1]); // show text
        
        if (!navigationMode) {
          lcd.setCursor(15, displayRow);
          lcd.blink();
        } else {
          lcd.noBlink();
        }
        break;
      }
  }
}
