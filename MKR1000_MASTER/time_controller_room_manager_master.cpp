#include "time_controller_room_manager_master.h"

   RTCZero rtc;
  unsigned long epoch;

void setupRtc(){
rtc.begin();

do {
    epoch = WiFi.getTime();
    Serial.println("Try Epoch");
    delay(500);
  }
  
  while ((epoch == 0));
 

    Serial.print("Epoch received: ");

    Serial.println(epoch);

    rtc.setEpoch(epoch);

    Serial.println();



}

int getTimeHour()
{

  return (rtc.getHours() +2);

}

int getTimeMinute()
{
  return rtc.getMinutes();

}
