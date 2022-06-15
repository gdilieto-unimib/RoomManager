#include "wifi_controller_room_manager.h"

#include "accesspoint_controller_room_manager.h"

String wifiList = "";
char ssidAP[] = "Room_Manager_Slave_AP"; // your network SSID (name)
int keyIndex = 0; // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

char apssid[] = "MKR1000AP";
WiFiServer serverAP(80);
String HTTP_req;
boolean readingNetwork = false;
boolean readingPassword = false;
boolean readingPubkey = false;
boolean readingSubkey = false;
boolean readingChannel = false;
String password = "";
String network = "";
String pubkey = "";
String subkey = "";
String channel = "";
boolean needCredentials = true;
boolean needWiFi = false;
boolean connectPubNub = false;

void setupAP() {

  Serial.println("Access Point Web Server\n");
  listNetworks();

  // print the network name (SSID);
  Serial.print("Creating access point named: ");
  Serial.println(ssidAP);

  // Create open network. Change this line if you want to create an WEP network:
  status = WiFi.beginAP(ssidAP);
  if (status != WL_AP_LISTENING) {
    Serial.println("Creating access point failed");
    // don't continue
    while (true);
  }

  // wait 10 seconds for connection:
  delay(10000);

  // start the web server on port 80
  serverAP.begin();

  // you're connected now, so print out the status
  printWiFiStatus();
}

String connectToWifiAP() {
  // compare the previous status to the current status
  if (status != WiFi.status()) {
    // it has changed update the variable
    status = WiFi.status();

    if (status == WL_AP_CONNECTED) {
      // a device has connected to the AP
      Serial.println("Device connected to AP");
    } else {
      // a device has disconnected from the AP, and we are back in listening mode
      Serial.println("Device disconnected from AP");
    }
  }

  WiFiClient client = serverAP.available(); // listen for incoming clients

  if (client) { // if you get a client,
    Serial.println("new client 1"); // print a message out the serial port
    String currentLine = ""; // make a String to hold incoming data from the client
    while (client.connected()) { // loop while the client's connected
      if (client.available()) { // if there's bytes to read from the client,

        if (needCredentials) {
          getCredentials();
        }
        if (needWiFi) {
          getWiFi();
        }

      } else {
        client = serverAP.available();
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
  }
  return password;
}

void printWiFiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print where to go in a browser:
  Serial.print(wifiList);

  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);

}

String getWifiList() {
  String reti = "";
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1) {
    reti = reti + "NON CE NULLA";
  }

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {

    reti = reti + String(WiFi.SSID(thisNet));

  }
}

void getCredentials() {

  WiFiClient client = serverAP.available();
  if (client) {
    boolean firstExclamative = true;
    Serial.println("new client 2");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.print(c);
        if (c == '?') readingNetwork = true;
        if (readingNetwork) {
          if (c == '!') {
            readingPassword = true;
            readingNetwork = false;
          } else if (c != '?') {
            network += c;
          }
        }
        if (readingPassword) {
          if (c == ',') {
            readingPubkey = true;
            readingPassword = false;
          } else {
            if (firstExclamative) {
              firstExclamative = false;
            } else {
              password += c;
            }
          }

        }
        if (readingPubkey) {
          if (c == '*') {
            readingSubkey = true;
            readingPubkey = false;
          } else if (c != ',') {
            pubkey += c;
          }
        }
        if (readingSubkey) {
          if (c == '!') {
            readingChannel = true;
            readingSubkey = false;
          } else if (c != '*') {
            subkey += c;
          }
        }
        if (readingChannel) {
          if (c == ',') {
            Serial.println();
            Serial.print("Network Name: ");
            Serial.println(network);
            Serial.print("Password: ");
            Serial.println(password);
            client.stop();
            WiFi.end();
            readingChannel = false;
            needCredentials = false;
            needWiFi = true;
          } else if (c != '!') {
            channel += c;
          }
        }
        if (c == '\n') {
          if (currentLine.length() == 0) {
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<html>");
            client.println("<head>");
            client.println("<style type=\"text/css\"> body {font-family: sans-serif; margin:50px; padding:20px; line-height: 250% } </style>");
            client.println("<style>.loader {margin: auto; border: 16px solid #f3f3f3;  border-radius: 50%;  border-top: 16px solid #3498db;  width: 20px;  height: 20px;  -webkit-animation: spin 2s linear infinite; /* Safari */  animation: spin 2s linear infinite;}@-webkit-keyframes spin {  0% { -webkit-transform: rotate(0deg); }  100% { -webkit-transform: rotate(360deg); }}@keyframes spin {  0% { transform: rotate(0deg); }  100% { transform: rotate(360deg); }}</style>");
            client.println("<style>.center {text-align: center;}</style>");
            client.println("<title>Arduino Setup</title>");
            client.println("</head>");
            client.println("<body>");
            client.print("<div class=\"center\">");
            client.print("<div style=\"border-style: solid; border-color: green;\">");
            client.print("<div class=\"loader\"></div>");
            client.println("<h2>ROOM MANAGER WIFI SETUP</h2>");
            client.print("NETWORK NAME: ");
            client.print("<input id=\"network\"/><br>");
            client.print("PASSWORD: ");
            client.print("<input type=\"password\" id=\"password\"/><br>");
            client.print("<div id=\"divCheckbox\" style=\"display: none;\">");
            client.println("<h2>PUBNUB CREDENTIALS</h2>");
            client.print("PUBLISH KEY: ");
            client.print("<input id=\"subkey\"/><br>");
            client.print("SUBSCRIBE KEY: ");
            client.print("<input id=\"pubkey\"/><br>");
            client.print("CHANNEL: ");
            client.print("<input id=\"channel\"/><br>");
            client.print("<br>");
            client.print("</div>");
            client.print("<button type=\"button\" onclick=\"SendText()\">Enter</button>");
            client.print("<div style=\"margin: auto; text-align:center;border-style: solid; border-color: coral;width:70%;\">");
            client.println("<h3>LIST OF AVAILABLE NETWORKS: </h3>");
            client.println(wifiList);
            client.print("</div>");
            client.print("<br>");
            client.print("</div>");
            client.println("</div>");
            client.println("</body>");
            client.println("<script>");
            client.println("var network = document.querySelector('#network');");
            client.println("var password = document.querySelector('#password');");
            client.println("var pubkey = document.querySelector('#pubkey');");
            client.println("var subkey = document.querySelector('#subkey');");
            client.println("var channel = document.querySelector('#channel');");
            client.println("function SendText() {");
            client.println("nocache=\"&nocache=\" + Math.random() * 1000000;");
            client.println("var request =new XMLHttpRequest();");
            client.println("netText = \"&txt=\" + \"?\" + network.value + \"!\" + password.value + \",\" + pubkey.value + \"*\" + subkey.value + \"!\" + channel.value + \",&end=end\";");
            client.println("request.open(\"GET\", \"ajax_inputs\" + netText + nocache, true);");
            client.println("request.send(null)");
            client.println("network.value=''");
            client.println("password.value=''");
            client.println("pubkey.value=''");
            client.println("subkey.value=''");
            client.println("channel.value=''");
            client.println("document.body.innerHTML = '<h1>WIFI CONFIGURED</h1>'");
            client.println("alert(\"WIFI CONFIGURED\");}");
            client.println("</script>");
            client.println("</html>");
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      } else {
        client = serverAP.available();
      }
    }

    client.stop();
    Serial.println("client disconnected");
    Serial.println();
  }
}
void getWiFi() {
  if (network == ""
      or password == "") {
    Serial.println("Invalid WiFi credentials");
    while (true);
  }
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(network);
    WiFi.begin(network, password);
    delay(5000);

  }
  Serial.println("WiFi connection successful");
  printWiFiStatus();
  needWiFi = false;
  delay(1000);
}

void printAPStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("signal strength (RSSI):");
  Serial.print(WiFi.RSSI());
  Serial.println(" dBm");
  Serial.print("To connect, open a browser to http://");
  Serial.println(ip);
}


void listNetworks() {    // scan for nearby networks:
  Serial.println("** Scan Networks **");
  int numSsid = WiFi.scanNetworks();
  if (numSsid == -1)
  {
    Serial.println("Couldn't get a wifi connection");
    while (true);
  }

  // print the network number and name for each network found:
  for (int thisNet = 0; thisNet < numSsid; thisNet++) {
    wifiList = wifiList + WiFi.SSID(thisNet);
    wifiList = wifiList + "<br>";
    Serial.flush();
  }
}
