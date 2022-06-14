#include "action_controller_room_manager.h"

// action pursued in temperature's controller screen (based on pressed button)
void actionTempScreen(int pressedButton, int* displayRow, int* tempActivationThreshold, int* tempConfig, boolean* navigationMode) {
  switch (pressedButton) {
    case MENO:
      {
        // on the first row we can change the thermostat's configuration
        if (*displayRow == 0){
          *tempConfig = (NUMBER_OF_TEMP_CONFIGS + *tempConfig - 1) % NUMBER_OF_TEMP_CONFIGS;
        } else{
        // on the second row we can decrease the temperature's activation threshold
          if (*tempActivationThreshold>0){
            *tempActivationThreshold-=1;
          }
        }
        break;
      }
    case PIU:
      {
        // on the first row we can change the thermostat's configuration
        if (*displayRow == 0) {
          *tempConfig = (*tempConfig + 1) % NUMBER_OF_TEMP_CONFIGS;
        } else {
        // on the second row we can increment the temperature's activation threshold
          if (*tempActivationThreshold<50){
            *tempActivationThreshold+=1;
          }
        }
        break;
      }
    case OK:
      {
        //go to next row of the display
        *displayRow = (*displayRow + 1) % 2;
    
        // return to navigation mode if pressed on the last row
        if (*displayRow == 0){
          *navigationMode = true;
        } else {
          // send new temp configuration to master
          mqttSendTempConfig(*tempConfig);
        }
        break;
      }
  }
}

// action pursued in light's controller screen (based on pressed button)
void actionLightScreen(int pressedButton, int* displayRow, int* lightActivationThreshold, int* lightConfig, boolean* navigationMode) {
  switch (pressedButton) {
      case MENO: {
        // on the first row we can change the light's configuration
        if (*displayRow == 0){
          *lightConfig = (NUMBER_OF_LIGHT_CONFIGS + *lightConfig - 1) % NUMBER_OF_LIGHT_CONFIGS;
        } else{
        // on the second row we can decrease the light's activation threshold
          if (*lightActivationThreshold>0){
            *lightActivationThreshold-=10;
          }
        }
        break;
      }
      case PIU: {
        // on the first row we can change the light's configuration
        if (*displayRow == 0){
          *lightConfig = (*lightConfig + 1) % NUMBER_OF_LIGHT_CONFIGS;

        } else{
        // on the first row we can increment the light's activation threshold
          if (*lightActivationThreshold<1000){
            *lightActivationThreshold+=10;
          }
        }
        break;
      }
      case OK: {
        //go to next row of the display
        *displayRow = (*displayRow + 1) % 2;

        // return to navigation mode if pressed on the last row
        if (*displayRow == 0){
          *navigationMode = true;
        }else{
          // send new light configuration to master
          mqttSendLightConfig(*lightConfig);
        }
        
        break;
      }
  }
}

// action pursued in alarm's controller screen (based on pressed button)
void actionAlarmScreen(int pressedButton, boolean* navigationMode, boolean* fireAlarm) {
  switch (pressedButton) {
      case MENO: {
        // toggle fire alarm for buzzer
        *fireAlarm = !*fireAlarm;
        break;
      }
      case PIU: {
        // toggle fire alarm for buzzer
        *fireAlarm = !*fireAlarm;
        break;
      }
      case OK: {
        // return to navigation mode
        *navigationMode = true;
        break;
      }
  }
}


// action pursued in temperature controller's screen (based on pressed button)
void actionInfoScreen(int pressedButton, int* displayRow, boolean* navigationMode, boolean* monitoringActivated){
  switch (pressedButton) {
      case MENO: {
        // toggle monitoring
        *monitoringActivated = !*monitoringActivated;
        // send new monitoring configuration to master
        mqttSendMonitoringConfig(*monitoringActivated);
        break;
      }
      case PIU: {
        // toggle monitoring
        *monitoringActivated = !*monitoringActivated;
        // send new monitoring configuration to master
        mqttSendMonitoringConfig(*monitoringActivated);
        break;
      }
      case OK: {
        // return to navigation mode
        *navigationMode = true;
        break;
      }
  }
}
