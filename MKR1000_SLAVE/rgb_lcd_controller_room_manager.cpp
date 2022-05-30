#include "rgb_lcd_controller_room_manager.h"
#include "rgb_lcd.h"
rgb_lcd lcd;

// character for wifi icon
byte wifiIcon[] = {
  B01110,
  B11011,
  B10001,
  B00000,
  B01110,
  B01010,
  B00000,
  B00100
};

// character for db icon
byte databaseIcon[] = {
  B00000,
  B00000,
  B01110,
  B10101,
  B01110,
  B00000,
  B00000,
  B00000
};

byte grade[] = {
  B01110,
  B01010,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

void setupLcd() {
  // setup lcd initial state

  lcd.begin(16, 2); // 16 cols, 2 rows
  lcd.setRGB(255, 255, 255);
  lcd.createChar(0, wifiIcon);
  lcd.createChar(1, databaseIcon);
  lcd.createChar(2, grade);

}

void MQTTLoadingScreen(boolean i) {
  // set mqtt connection loading screen (or disable it)
  
  if (i==false){
    lcd.noBlink();
    lcd.setRGB(50,50,50);
    return;
  }
  lcd.clear();
  lcd.setRGB(100, 0, 100);
  lcd.print("Connecting to");
        lcd.setCursor(0, 1);

  lcd.print("mqtt broker");  
  lcd.blink();
}

void setNavigationMode(int active) {
  // set lcd color based on navigation mode
  
  if (active) {
    lcd.setRGB(50,50,50);
  } else {
    lcd.setRGB(255,255,255);
  }
}

void setTooHotAlarm(boolean active) {
  // set lcd color based on too hot alarm (red)
  
  if (!active){
     lcd.setRGB(50, 50, 50); 
  } else {
     lcd.setRGB(255, 0, 0); 
  }
}

void setTooColdAlarm(boolean active) {
  // set lcd color based on too cold alarm (blue)
  
  if (!active){
     lcd.setRGB(50, 50, 50); 
  } else {
     lcd.setRGB(0, 0, 255); 
  }
}

void wifiLoadingScreen(boolean i){
  // set wifi loading screen (or disable it)
  
  if (i==false){
    lcd.noBlink();
    lcd.setRGB(50,50,50);
    return;
  }
  lcd.clear();
  lcd.setRGB(100, 0, 100);
  lcd.print("Connecting to");
        lcd.setCursor(0, 1);

  lcd.print("wifi");  
  lcd.blink();
}

void dbLoadingScreen(boolean i){
  // set database configuration loading screen (or disable it)
  
  if (i==false){
    lcd.noBlink();
    lcd.setRGB(50,50,50);
    return;
  }
  lcd.clear();
  lcd.setRGB(100, 0, 100);
  lcd.print("Loading config");
        lcd.setCursor(0, 1);

  lcd.print("from database");  
  lcd.blink();
}

void loggingLoadingScreen(boolean i){
  // set logging loading screen (or disable it)
  
  if (i==false){
    lcd.noBlink();
    lcd.setRGB(50,50,50);
    return;
  }
  
  // set screen's color green
  lcd.setRGB(100, 100, 0);
}

void updateInfoScreenRows(int temp, int light, boolean wifi, boolean isMySqlConnected) {
  // update rows for the info screen
  
  lcd.clear();  // clear text
  lcd.print("T: "); // show temp
  lcd.print(temp);
  lcd.write(2);
  lcd.setCursor(0, 1);
  lcd.print("L: "); // show light
  lcd.print(light);

  lcd.setCursor(9, 1);
  lcd.write((unsigned char)0);  // show wifi connection
  
  if(wifi){
    lcd.print(": ON");
  }else{
    lcd.print(": OFF");
  }
  
  lcd.setCursor(9, 0);
  lcd.write(1); // show database connection

  if(isMySqlConnected){
    lcd.print(": ON");
  }else{
    lcd.print(": OFF");
  }
}

void updateTempScreenRows(int temp, int tempConfig, int tempActivationThreshold) {
  // update rows for the temperature's controller screen
  
  char tempScreenRows[2][16] = {"Temp: ", ""};
  
  if (tempConfig == CONFIG_ON) {
    // show thermostat status based on temperature's activation threshold
    strcat(tempScreenRows[0], temp < tempActivationThreshold ? "ON +" : temp > tempActivationThreshold ? "ON -" : "ON =");
  } else {
    // show thermostat status off
    strcat(tempScreenRows[0], "OFF");
  }
  
   // show temperature's activation threshold
  sprintf(tempScreenRows[1], "Level: %d", tempActivationThreshold);
  
  updateScreenRowsText(tempScreenRows);
}

void updateLightScreenRows(int lightStatus, int lightConfig, int lightActivationThreshold) {
  // update rows for the light's controller screen
  
  char lightScreenRows[2][16] = {"Light: ", ""};
        
  if (lightConfig == CONFIG_AUTO) {
    // show light status based on light's activation threshold
    strcat(lightScreenRows[0], lightStatus == LIGHT_STATUS_ON ? "AUTO ON" : "AUTO OFF");
  } else {
    // show light status ON or OFF based on the configuration set
    strcat(lightScreenRows[0], lightConfig == CONFIG_ON ? "ON" : "OFF");
  }
  
   // show light's activation threshold
  sprintf(lightScreenRows[1], "Level: %d", lightActivationThreshold);
  
  updateScreenRowsText(lightScreenRows);
}

void updateAlarmScreenRows(boolean fireAlarm) {
  // update rows for the alarm's controller screen
  
  char alarmScreenRows[2][16] = {"Alarm: ", ""};
        
  strcat(alarmScreenRows[0], fireAlarm ? "ON" : "OFF");
  
  updateScreenRowsText(alarmScreenRows);
}

void updateScreenCursor(int active, int displayRow) {
  // update the screen cursor if in navigation mode
  
  if (active) { 
    // if in screen action mode blink on the actual display row
    
    lcd.setCursor(15, displayRow);
    lcd.blink();
  } else {
    // if navigation mode don't blink
    
    lcd.noBlink();
  }
}

void updateScreenRowsText(char screenRows[2][16]) {
  // update the screen rows
  
  lcd.clear();  // clear text
  
  lcd.setCursor(0,0);
  lcd.print(screenRows[0]); // show first row
  
  lcd.setCursor(0,1);
  lcd.print(screenRows[1]); // show second row
}
