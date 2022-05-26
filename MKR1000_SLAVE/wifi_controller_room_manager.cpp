#include "wifi_controller_room_manager.h"

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
String mac=""; 

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

String getMac() {
  if (mac=="") {
    byte macBytes[6];
    WiFi.macAddress(macBytes);
    char macChars[19]={0};
    sprintf(macChars, "%02X:%02X:%02X:%02X:%02X:%02X", macBytes[0], macBytes[1], macBytes[2], macBytes[3], macBytes[4], macBytes[5]);
    macChars[18]=0;
    Serial.println(macChars);
    Serial.println(macBytes[3]);
    Serial.println(String(macChars));
    mac = String(macChars);
    Serial.println(mac);
  }
  return mac;
}
