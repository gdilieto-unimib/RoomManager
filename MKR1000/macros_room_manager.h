#ifndef MACROS_ROOM_MANAGER

#define MACROS_ROOM_MANAGER
#include <Arduino.h>

#define BUTTON_PIU 3  // D3, digital pin used to read the button state
#define BUTTON_MENO 2 // D2, digital pin used to read the button state
#define BUTTON_OK 1 // D1, digital pin used to read the button state
#define BUZZER_GROVE 0  // D0, digital pin used to drive the buzzer
#define TEMP A0 // A0, analog pin used to read the temperature
#define PHOTORESISTOR A1 // A1, analog pin used to read the light
#define LIGHT_LED 4 // D4, digital pin used for the light led
#define TEMP_LED 5 // D5, digital pin used for the light led

#define PIU 3 // Action when button piu is pressed
#define MENO 2 // Action when button meno is pressed
#define OK 1 // Action when button ok is pressed
#define NO_OP 0 // Action when no button is pressed

#define INFO_SCREEN 0
#define TEMP_SCREEN 1
#define LIGHT_SCREEN 2
#define ALARM_SCREEN 3
#define NUMBER_OF_SCREENS 4

#define B 4275  // B-constant of the thermistor
#define R0 100000 // R0 = 100KOhm

#define LIGHT_STATUS_OFF 0
#define LIGHT_STATUS_ON 1

#define TEMP_STATUS_OFF 0
#define TEMP_STATUS_UP 1
#define TEMP_STATUS_DOWN 2

#define CONFIG_OFF 0
#define CONFIG_ON 1
#define CONFIG_AUTO 2
#define NUMBER_OF_TEMP_CONFIGS 2
#define NUMBER_OF_LIGHT_CONFIGS 3

#define LIGHT_SENSOR 0
#define TEMP_SENSOR 1
#define WIFI_SENSOR 2

#endif
