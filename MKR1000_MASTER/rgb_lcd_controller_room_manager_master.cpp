#include "rgb_lcd_controller_room_manager_master.h"
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
  B01110,
  B10001,
  B01110,
  B10001,
  B01110,
  B10001,
  B01110
};


void setupLcd() {
  // setup lcd initial state

  lcd.begin(16, 2); // 16 cols, 2 rows
  lcd.setRGB(255, 255, 255);
  lcd.createChar(0, wifiIcon);
  lcd.createChar(1, databaseIcon);

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
  // set database connection loading screen (or disable it)
  
  if (i==false){
    lcd.noBlink();
    lcd.setRGB(50,50,50);
    return;
  }
  lcd.clear();
  lcd.setRGB(100, 0, 100);
  lcd.print("Connecting to");
        lcd.setCursor(0, 1);

  lcd.print("database");  
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

void updateInfoScreenRows(int connectedDevices, boolean wifi, boolean isMySqlConnected) {
  // update rows for the info screen
  
  lcd.clear();  // clear text
  
  lcd.setCursor(0, 0);
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

  lcd.setCursor(0, 1);
  lcd.print("Devices: "); // show devices number
  lcd.print(connectedDevices);

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
