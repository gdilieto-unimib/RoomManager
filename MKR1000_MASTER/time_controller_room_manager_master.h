#ifndef TIME_CONTROLLER_ROOM_MANAGER_MASTER

#define TIME_CONTROLLER_ROOM_MANAGER_MASTER
#include <Arduino.h>
#include "macros_room_manager_master.h"

#include <WiFiUdp.h>
#include <RTCZero.h>
#include <SPI.h>
#include <WiFi101.h>




int  getTimeHour();
int  getTimeMinute();

void  setupRtc();
#endif
