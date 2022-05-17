#include "wifi_controller_room_manager_master.h"

// WiFi cfg
char ssid[] = SECRET_SSID;   // your network SSID (name)
char pass[] = SECRET_PASS;   // your network password
#ifdef IP
IPAddress ip(IP);
IPAddress subnet(SUBNET);
IPAddress dns(DNS);
IPAddress gateway(GATEWAY);
#endif
WiFiClient client;

void connectWifi(){
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
