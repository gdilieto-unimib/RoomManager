// Use this file to store all of the private credentials and connection details

// WiFi configuration
#define SECRET_SSID "IoTLabThingsU14"                  // SSID
#define SECRET_PASS "L@b%I0T*Ui4!P@sS**0%Lessons!"    // WiFi password


// ONLY if static configuration is needed
#define IP {149, 132, 182, 33}                    // IP address 
#define SUBNET {255, 255, 255, 0}                // Subnet mask 
#define DNS {149, 132, 2, 3}                    // DNS 
#define GATEWAY {149, 132, 182, 1}             // Gateway


// MySQL access
#define MYSQL_IP {149, 132, 178, 180}           // IP address of the machine running MySQL
#define MYSQL_USER "gdilieto"                  // db user
#define MYSQL_PASS "iot874143"                // db user's password

// MQTT access
#define MQTT_BROKERIP "149.132.178.180"           // IP address of the machine running the MQTT broker
#define MQTT_CLIENTID "room_manager_master"                 // client identifier
#define MQTT_USERNAME "gdilieto"            // mqtt user's name
#define MQTT_PASSWORD "iot874143"            // mqtt user's password


#define WEATHER_API_KEY "20bf48d2e0b208a53dcf3c35bfb81edf"           // api key form https://home.openweathermap.org/api_keys
#define WEATHER_CITY "Milan"                     // city
#define WEATHER_COUNTRY "it"                     // ISO3166 country code 
