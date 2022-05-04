#ifndef MACROS_ROOM_MANAGER

#define MACROS_ROOM_MANAGER
#include <Arduino.h>

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

#define STATUS_OFF 0
#define STATUS_ON 1
#define STATUS_NEUTRAL 2

#define CONFIG_OFF 0
#define CONFIG_ON 1
#define CONFIG_AUTO 2
#define NUMBER_OF_CONFIGS 3

#endif
