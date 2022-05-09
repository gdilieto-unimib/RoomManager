#include "action_controller_room_manager.h"

void actionTempScreen(int pressedButton, int* displayRow, int* tempActivationThreshold, int* tempConfig, boolean* navigationMode) {
  switch (pressedButton) {
    case MENO:
      {
        if (*displayRow == 0){
          *tempConfig = (NUMBER_OF_TEMP_CONFIGS + *tempConfig - 1) % NUMBER_OF_TEMP_CONFIGS-1;
        } else{
          if (*tempActivationThreshold>0){
            *tempActivationThreshold-=1;
          }
        }
        break;
      }
    case PIU:
      {
        if (*displayRow == 0) {
          *tempConfig = (*tempConfig + 1) % NUMBER_OF_TEMP_CONFIGS;
        } else {
          if (*tempActivationThreshold<40){
            *tempActivationThreshold+=1;
          }
        }
        break;
      }
    case OK:
      {
        *displayRow = (*displayRow + 1) % 2;
    
        if (*displayRow == 0){
          *navigationMode = true;
        }
        break;
      }
  }
}

void actionLightScreen(int pressedButton, int* displayRow, int* lightActivationThreshold, int* lightConfig, boolean* navigationMode) {
  switch (pressedButton) {
      case MENO: {
        if (*displayRow == 0){
          *lightConfig = (NUMBER_OF_LIGHT_CONFIGS + *lightConfig - 1) % NUMBER_OF_LIGHT_CONFIGS;
        } else{
          if (*lightActivationThreshold>0){
            *lightActivationThreshold-=10;
          }
        }
        break;
      }
      case PIU: {
        if (*displayRow == 0){
          *lightConfig = (*lightConfig + 1) % NUMBER_OF_LIGHT_CONFIGS;
        } else{
          if (*lightActivationThreshold<1000){
            *lightActivationThreshold+=10;
          }
        }
        break;
      }
      case OK: {
        *displayRow = (*displayRow + 1) % 2;
    
        if (*displayRow == 0)
          *navigationMode = true;
        
        break;
      }
  }
}

void action(int screen, int pressedButton, int* displayRow, int* tempActivationThreshold, int* lightActivationThreshold, int* tempConfig, int* lightConfig, boolean* navigationMode) {
  switch (screen)
    {
      case TEMP_SCREEN: {
        actionTempScreen(pressedButton, displayRow, tempActivationThreshold, tempConfig, navigationMode);
        break;
      }

      case LIGHT_SCREEN: {
        actionLightScreen(pressedButton, displayRow, lightActivationThreshold, lightConfig, navigationMode);
        break;
      }
    }
}
