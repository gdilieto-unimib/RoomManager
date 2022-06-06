#include "weather_controller_room_manager.h"

const char weather_server[] = "api.openweathermap.org";
const char weather_query[] = "GET /data/2.5/weather?q=%s,%s&units=metric&APPID=%s";



float getExternalTemperature() {
    WiFiClient client;
  // Current weather api documentation at: https://openweathermap.org/current
  
  // call API for current weather
  if (client.connect(weather_server, 80)) {
    char request[100];
    sprintf(request, weather_query, WEATHER_CITY, WEATHER_COUNTRY, WEATHER_API_KEY);
    client.println(request);
    client.println(F("Host: api.openweathermap.org"));
    client.println(F("User-Agent: ArduinoWiFi/1.1"));
    client.println(F("Connection: close"));
    client.println();
  } else {
    Serial.println(F("Connection to api.openweathermap.org failed!\n"));
  }

  while (client.connected() && !client.available()) delay(1);   // wait for data
  String result;
  while (client.connected() || client.available()) {   // read data
    char c = client.read();
    result = result + c;
  }

  client.stop();   // end communication
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
