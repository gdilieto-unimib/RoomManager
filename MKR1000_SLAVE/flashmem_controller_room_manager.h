#ifndef FLASHMEM_CONTROLLER_ROOM_MANAGER

#define FLASHMEM_CONTROLLER_ROOM_MANAGER
#include <Arduino.h>
#include <FlashStorage.h>

#include "macros_room_manager.h"
#include "accesspoint_controller_room_manager.h"


typedef struct {
  boolean valid;
  char ssid_RM[100];
  char pssw_RM[100];
} WiFi_Credentials;


#endif
