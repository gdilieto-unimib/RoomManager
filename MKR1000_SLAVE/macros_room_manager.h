#ifndef MACROS_ROOM_MANAGER

#define MACROS_ROOM_MANAGER
#include <Arduino.h>

#define BUTTON_isLowPowerMode 3  // D3, digital pin used to read the button state

#define BUTTON_PIU 3  // D3, digital pin used to read the button state
#define BUTTON_MENO 2 // D2, digital pin used to read the button state
#define BUTTON_OK 1 // D1, digital pin used to read the button state
#define BUZZER_GROVE 0  // D0, digital pin used to drive the buzzer
#define TEMP A2 // A0, analog pin used to read the temperature
#define PHOTORESISTOR A1 // A1, analog pin used to read the light
#define LIGHT_LED 4 // D4, digital pin used for the light led
#define HOT_TEMP_LED 5 // D5, digital pin used for the hot temp led
#define COLD_TEMP_LED 5 // A0, analogical pin used as digital pin for the cold temp led ( PROVVISORIO SU PIN 5 )

#define TOO_HOT_TEMP_THRESHOLD 35 // Too hot threshold for the alarm
#define TOO_COLD_TEMP_THRESHOLD 10 // Too cold threshold for the alarm

#define PIU 3 // Action when button piu is pressed
#define MENO 2 // Action when button meno is pressed
#define OK 1 // Action when button ok is pressed
#define NO_OP 0 // Action when no button is pressed

#define INFO_SCREEN 0 //id for info screen
#define TEMP_SCREEN 1 //id for temperature control screen
#define LIGHT_SCREEN 2 //id for light control screen
#define ALARM_SCREEN 3 //id for fire alarm screen
#define NUMBER_OF_SCREENS 4 //total number of screens

#define B 4275  // B-constant of the thermistor
#define R0 100000 // R0 = 100KOhm

#define LIGHT_STATUS_OFF 0 // id for light status off
#define LIGHT_STATUS_ON 1 // id for light status on

#define TEMP_STATUS_OFF 0 // id for temp status off
#define TEMP_STATUS_UP 1 // id for temp status increasing
#define TEMP_STATUS_DOWN 2 // id for temp status decreasing
#define TEMP_STATUS_ECO 3 // id for temp status eco

#define TEMPERATURE_ISOLATION_CONSTANT 2 // temperature isolation constant for the eco mode

#define CONFIG_OFF 0 // id for generic configuration off
#define CONFIG_ON 1 // id for generic configuration on
#define CONFIG_AUTO 2 // id for generic configuration automatic
#define NUMBER_OF_TEMP_CONFIGS 2 // number of configuration for temp actuators
#define NUMBER_OF_LIGHT_CONFIGS 3 // number of configuration for light actuators

#define LIGHT_SENSOR 0 // id of the index of the light sensor among sensors list
#define TEMP_SENSOR 1 // id of the index of the temperature sensor among sensors list
#define WIFI_SENSOR 2 // id of the index of the wifi sensor among sensors list

#define HOT_ALARM_CODE 0 // code logged for the hot alarm
#define COLD_ALARM_CODE 1 // code logged for the cold alarm

#define CONTROL_FREQUENCE_LOW_POWER_MODE 10000



#endif
