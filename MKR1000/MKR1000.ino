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
#define PHOTORESISTOR A1 // A1, analog pin used to read the light
#define LED 4 // D4, digital pin used for the led

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

#include <SPI.h>
#include <WiFi101.h>
#include <WiFiUdp.h>

#include "rgb_lcd.h"
#include "secrets.h"

// WiFi cfg
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
#ifdef IP
IPAddress ip(IP);
IPAddress subnet(SUBNET);
IPAddress dns(DNS);
IPAddress gateway(GATEWAY);
#endif
WiFiClient client;

// NTP cfg
const char* ntpServerName = "time.nist.gov";   // it.pool.ntp.org";   // NTP server name
const int NTP_PACKET_SIZE = 48;                // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];            // buffer to hold I/O NTP packets
IPAddress timeServerIP;                        // dynamically resolved IP of the NTP server
WiFiUDP udp;                                   // UDP instance to send and receive packets
unsigned int localPort = 2390; 

rgb_lcd lcd;
int screen = 0;
boolean navigationMode = true;
int lightStatus = LIGHT_STATUS_OFF;
int lightConfig = LIGHT_CONFIG_OFF;
int displayRow = 0;
int last_light=0;
int last_temperature=0;
boolean too_hot_alarm=false;
boolean too_cold_alarm=false;
long time;

void setup()
{
  time=millis();
  udp.begin(localPort);
  
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




  // connect to WiFi (if not already connected)
  if (millis()-time >10000 && WiFi.status() != WL_CONNECTED) {
    time=millis();
    Serial.print("ATTEMPTIONNNNNNN");
   
#ifdef IP
    WiFi.config(ip, dns, gateway, subnet);   // by default network is configured using DHCP
#endif
    WiFi.begin(ssid, pass);
  }



  
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
  if (navigationMode) {
    lcd.setRGB(50,50,50);
  } else {
    lcd.setRGB(255,255,255);
  }
  switch (screen)
  {
    case 0:
      {
        
        
        if(getTemperature()>30){
          too_hot_alarm=true;
        }else{
          too_hot_alarm=false;
        }
        if(!too_cold_alarm) {
          if(too_hot_alarm==false){
            lcd.setRGB(50, 50, 50);
          }else{
            lcd.setRGB(255, 0, 0);
          }
        }
        
        if(getTemperature()<25){
          too_cold_alarm=true;
        }else{
          too_cold_alarm=false;
        }
        if(!too_hot_alarm) {
          if(too_cold_alarm==false){
            lcd.setRGB(50, 50, 50);
          }else{
            lcd.setRGB(0, 0, 255);
          }
        }
        
        if(too_hot_alarm==true){
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

        if(WiFi.status() == WL_CONNECTED){
          lcd.setCursor(10, 0);
            lcd.print(" Wi-Fi");
        }else{
          lcd.setCursor(10, 0);
            lcd.print(" NO Wi-Fi");
        }

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
