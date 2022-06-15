#include "time_controller_room_manager_master.h"

RTCZero rtc;
unsigned long epoch;

void setupRtc() {
  rtc.begin();

  do {
    epoch = WiFi.getTime();

    delay(500);
  }

  while ((epoch == 0));




  rtc.setEpoch(epoch);




}

int getTimeHour()
{

  return (rtc.getHours() + 2);

}

int getTimeMinute()
{
  return rtc.getMinutes();

}
