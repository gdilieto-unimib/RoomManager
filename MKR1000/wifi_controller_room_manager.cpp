#include "wifi_controller_room_manager.h"

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

// NTP cfg
const char* ntpServerName = "time.nist.gov";   // it.pool.ntp.org";   // NTP server name
const int NTP_PACKET_SIZE = 48;                // NTP time stamp is in the first 48 bytes of the message
byte packetBuffer[NTP_PACKET_SIZE];            // buffer to hold I/O NTP packets
IPAddress timeServerIP;                        // dynamically resolved IP of the NTP server
WiFiUDP udp;                                   // UDP instance to send and receive packets
unsigned int localPort = 2390; 

void connectWifi(){
  if (WiFi.status() != WL_CONNECTED) {
   #ifdef IP
    WiFi.config(ip, dns, gateway, subnet);   // by default network is configured using DHCP
    #endif
    WiFi.begin(ssid, pass);
  }
}

boolean isWifiConnected() {
  return (WiFi.status() == WL_CONNECTED);
}

void setupWiFi(){
  udp.begin(localPort);
}

WiFiClient * getClient(){
  return &client;  
}
