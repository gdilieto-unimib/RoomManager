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

int getTimeHour()
{

  return (rtc.getHours() +2);

}

int getTimeMinute()
{
  return rtc.getMinutes();

}
