#include "time_controller_room_manager_master.h"

   RTCZero rtc;
  unsigned long epoch;
  int numberOfTries = 0, maxTries = 10;

  
  


void setupRtc(){
rtc.begin();

do {
  delay(500);
    epoch = WiFi.getTime();

    numberOfTries++;

  }

  while ((epoch == 0) && (numberOfTries < maxTries));
  

  if (numberOfTries == maxTries) {
    Serial.print("NTP unreachable!!");

    

  }

  else {

    Serial.print("Epoch received: ");

    Serial.println(epoch);

    rtc.setEpoch(epoch);

    Serial.println();

  }


}

void printTime()
{

  print2digits(rtc.getHours() +2 );

  Serial.print(":");

  print2digits(rtc.getMinutes());

  Serial.print(":");

  print2digits(rtc.getSeconds());

  Serial.println();
}

void print2digits(int number) {

  if (number < 10) {

    Serial.print("0");

  }

  Serial.print(number);
}
