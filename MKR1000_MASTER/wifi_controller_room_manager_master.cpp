#include "wifi_controller_room_manager_master.h"
#include "secrets_master.h"

#ifdef IP
IPAddress ip(IP);
IPAddress subnet(SUBNET);
IPAddress dns(DNS);
IPAddress gateway(GATEWAY);
#endif
WiFiClient client;

void connectWifi(char* ssid, char* pass){
  // connect to wifi if not already connected
  
  if (WiFi.status() != WL_CONNECTED) {
    #ifdef IP
    WiFi.config(ip, dns, gateway, subnet);   // by default network is configured using DHCP
    #endif
    WiFi.begin(ssid, pass);
  }
}

boolean isWifiConnected() {
  // check if wifi is connected
  
  return (WiFi.status() == WL_CONNECTED);
}

WiFiClient* getClient(){
  // get pointer to the wifi client
  
  return &client;  
}
