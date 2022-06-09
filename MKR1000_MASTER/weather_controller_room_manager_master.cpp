#include "weather_controller_room_manager_master.h"
#include "wifi_controller_room_manager_master.h"
#include "database_controller_room_manager_master.h"
#include "mqtt_controller_room_manager_master.h"

const char weather_server[] = "api.openweathermap.org";
const char weather_query[] = "GET /data/2.5/weather?q=%s,%s&units=metric&APPID=%s";

WiFiClient* client2 = getClient(); 
boolean firstAccess = true;

float getExternalTemperature() {
  // Current weather api documentation at: https://openweathermap.org/current
  
  // call API for current weather
  if (client2->connect(weather_server, 80)) {
    char request[100];
    sprintf(request, weather_query, WEATHER_CITY, WEATHER_COUNTRY, WEATHER_API_KEY);
    client2->println(request);
    client2->println(F("Host: api.openweathermap.org"));
    client2->println(F("User-Agent: ArduinoWiFi/1.1"));
    client2->println(F("Connection: close"));
    client2->println();
  } else {
    Serial.println(F("Connection to api.openweathermap.org failed!\n"));
  }

  while (client2->connected() && !client2->available()) {delay(1);}   // wait for data
  String result;
  while (client2->connected() || client2->available()) {   // read data
    char c = client2->read();
    result = result + c;
  }
  
  client2->stop();   // end communication

  if (firstAccess) {
    firstAccess = false;
  } else {
    disconnectWifi();
  }
  
  
  // Serial.println(result);  // print JSON

  char jsonArray[result.length() + 1];
  result.toCharArray(jsonArray, sizeof(jsonArray));
  jsonArray[result.length() + 1] = '\0';
  StaticJsonDocument<1024> doc;
  DeserializationError error = deserializeJson(doc, jsonArray);

  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return -1;
  }
  Serial.print(F("Temperature (°C): "));
  Serial.println((float)doc["main"]["temp"]);
  return ((float)doc["main"]["temp"]);

}
