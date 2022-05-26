// Use this file to store all of the private credentials and connection details

// WiFi configuration
#define SECRET_SSID "IoTLabThingsU14"                  // SSID
#define SECRET_PASS "L@b%I0T*Ui4!P@sS**0%Lessons!"              // WiFi password

// ONLY if static configuration is needed

#define IP {149, 132, 182, 43}                    // IP address
#define SUBNET {255, 255, 255, 0}                // Subnet mask
#define DNS {149, 132, 2, 3}                     // DNS
#define GATEWAY {149, 132, 182, 1}               // Gateway


// MySQL access
#define MYSQL_IP {149, 132, 178, 180}              // IP address of the machine running MySQL
#define MYSQL_USER "gdilieto"                  // db user
#define MYSQL_PASS "iot874143"     //         // db user's password


// MQTT access
#define MQTT_BROKERIP "149.132.178.180"           // IP address of the machine running the MQTT broker
#define MQTT_CLIENTID "mkr2_tf"                 // client identifier
#define MQTT_USERNAME "tfalsiroli"            // mqtt user's name
#define MQTT_PASSWORD "iot874971"            // mqtt user's password
