#include "rgb_lcd_controller_room_manager.h"
#include "rgb_lcd.h"
rgb_lcd lcd;

void setupLcd() {
  lcd.begin(16, 2); // 16 cols, 2 rows
  lcd.setRGB(255, 255, 255);
}

void setNavigationMode(int active) {
  if (active) {
    lcd.setRGB(50,50,50);
  } else {
    lcd.setRGB(255,255,255);
  }
}

void updateInfoScreenRows(int tempStatus, int tempConfig, int tempActivationThreshold) {
  char tempScreenRows[2][16] = {"Temp: ", "Level: "};
  
  if (tempConfig == CONFIG_AUTO) {
    strcat(tempScreenRows[0], tempStatus == STATUS_ON ? "AUTO +" : tempStatus == STATUS_OFF ? "AUTO -" : "AUTO x");
  } else {
    strcat(tempScreenRows[0], tempConfig == CONFIG_ON ? "ON" : "OFF");
  }
  
  sprintf(tempScreenRows[1], tempScreenRows[1], tempActivationThreshold);

  updateScreenRowsText(tempScreenRows);
}

void updateTempScreenRows(int tempStatus, int tempConfig, int tempActivationThreshold) {
  char tempScreenRows[2][16] = {"Temp: ", "Level: "};
  
  if (tempConfig == CONFIG_AUTO) {
    strcat(tempScreenRows[0], tempStatus == STATUS_ON ? "AUTO +" : tempStatus == STATUS_OFF ? "AUTO -" : "AUTO x");
  } else {
    strcat(tempScreenRows[0], tempConfig == CONFIG_ON ? "ON" : "OFF");
  }
  
  sprintf(tempScreenRows[1], tempScreenRows[1], tempActivationThreshold);

  updateScreenRowsText(tempScreenRows);
}

void updateLightScreenRows(int lightStatus, int lightConfig, int lightActivationThreshold) {
  char lightScreenRows[2][16] = {"Light: ", "Level: %d"};
        
  if (lightConfig == CONFIG_AUTO) {
    strcat(lightScreenRows[0], lightStatus == STATUS_ON ? "AUTO ON" : "AUTO OFF");
  } else {
    strcat(lightScreenRows[0], lightConfig == CONFIG_ON ? "ON" : "OFF");
  }
  
  sprintf(lightScreenRows[1], lightScreenRows[1], lightActivationThreshold);
  
  updateScreenRowsText(lightScreenRows);
}

void updateScreenCursor(int active, int displayRow) {
  if (active) {
    lcd.setCursor(15, displayRow);
    lcd.blink();
  } else {
    lcd.noBlink();
  }
}

void updateScreenRowsText(char screenRows[2][16]) {
  lcd.clear();  // clear text
  
  lcd.setCursor(0,0);
  lcd.print(screenRows[0]); // show first row
  
  lcd.setCursor(0,1);
  lcd.print(screenRows[1]); // show second row
}
