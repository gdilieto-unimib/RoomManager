#include "rgb_lcd_controller_room_manager.h"
#include "rgb_lcd.h"
rgb_lcd lcd;

byte WifiIcon[] = {
  B01110,
  B11011,
  B10001,
  B00000,
  B01110,
  B01010,
  B00000,
  B00100
};

byte NoWifiIcon[] = {
  B11011,
  B11011,
  B11011,
  B01110,
  B01110,
  B11011,
  B11011,
  B11011
};


void setupLcd() {

  lcd.begin(16, 2); // 16 cols, 2 rows
  lcd.setRGB(255, 255, 255);
  lcd.createChar(0, WifiIcon);
  lcd.createChar(1, NoWifiIcon);

}

void setNavigationMode(int active) {
  if (active) {
    lcd.setRGB(50,50,50);
  } else {
    lcd.setRGB(255,255,255);
  }
}

void setTooHotAlarm(boolean active) {
  if (!active){
     lcd.setRGB(50, 50, 50); 
  } else {
     lcd.setRGB(255, 0, 0); 
  }
}

void setTooColdAlarm(boolean active) {
  if (!active){
     lcd.setRGB(50, 50, 50); 
  } else {
     lcd.setRGB(0, 0, 255); 
  }
}


void updateInfoScreenRows(int temp, int light, int wifi) {
  
      lcd.clear();  // clear text
      lcd.print("T: "); // show temp
      lcd.print(temp);
      lcd.setCursor(0, 1);
      lcd.print("L: "); // show light
      lcd.print(light);

      lcd.setCursor(10, 0);
      lcd.write((unsigned char)0);
      
      if(wifi){
        lcd.print(": ON");
      }else{
        lcd.print(": OFF");
      }
}

void updateTempScreenRows(int temp, int tempConfig, int tempActivationThreshold) {
  char tempScreenRows[2][16] = {"Temp: ", ""};
  
  if (tempConfig == CONFIG_ON) {
    strcat(tempScreenRows[0], temp < tempActivationThreshold ? "ON +" : temp > tempActivationThreshold ? "ON -" : "ON =");
  } else {
    strcat(tempScreenRows[0], "OFF");
  }
  
  sprintf(tempScreenRows[1], "Level: %d", tempActivationThreshold);
  
  updateScreenRowsText(tempScreenRows);
}

void updateLightScreenRows(int lightStatus, int lightConfig, int lightActivationThreshold) {
  char lightScreenRows[2][16] = {"Light: ", ""};
        
  if (lightConfig == CONFIG_AUTO) {
    strcat(lightScreenRows[0], lightStatus == STATUS_ON ? "AUTO ON" : "AUTO OFF");
  } else {
    strcat(lightScreenRows[0], lightConfig == CONFIG_ON ? "ON" : "OFF");
  }
  
  sprintf(lightScreenRows[1], "Level: %d", lightActivationThreshold);
  
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
