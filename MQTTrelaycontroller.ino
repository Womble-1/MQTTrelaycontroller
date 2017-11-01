/*
Credit:  Code starting point from wemos forum
https://forum.wemos.cc/topic/374/solved-question-with-powering-and-triggering-relay-shield/8
 */

#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <variables.h>

//variables.h contains the following
//#define wifi_ssid ""
//#define wifi_password ""
//#define mqtt_server ""
//#define mqtt_user ""
//#define mqtt_password ""
//#define mqtt_port ""

#define mqtt_clientID "IOTtesting"
#define HostName "IOTtesting"
//#define relay1Pin 5//  Initialize D1 aka GPIO-5 as an output on the WeMOS board

// Connect to the WiFi
//const char* ssid = "MYSSID";
//const char* password = "MYPASSWORD";
//const char* mqtt_server = "192.168.0.62";

WiFiClient espClient;
PubSubClient client(espClient);

const int relay1Pin = D1; // FOR RED LED  CHANGED TO D1 FOR TESTING
// DONT FORGET TO CHANGE IT BACK TO D5
const int relay2Pin = D8; // FOR GREEN LED


// ************************************************************
/* WIRING SCHEMATIC
 *    
 *  LEDS:
 *  12V TO RED WIRES OF RED AND GREEN LEDS
 *  BLACK WIRES OF LED TO "NORMAL OFF" SIDE OF RELAY
 *  CENTER OUTPUT (+) ON RELAY TO GROUND
 *  TIE GROUNDS TOGETHER
 *  
 *  "VERTER":
 *  12V TO INPUT SIDE OF "BUCK VERTER"
 *  5V OUT TO WEMOS
 *  
 *  WEMOS:
 *  5V FROM BUCK VERTER TO 5V PIN ON WEMOS
 *  D5 TO D1 ON RELAY 1
 *  D8 TO D1 ON RELAY 2
 *  
 *  RELAYS:
 *  ?????????????????????
 */
// ************************************************************


void callback(char* topic, byte* payload, unsigned int length) {

  // ******************* relay 1 *****************************
  if (strcmp(topic, "testing/relay1") == 0) {
    for (int i = 0; i < length; i++) {
      char receivedChar = (char)payload[i];
      if (receivedChar == '0') {
        Serial.println (F("Relay 1 ON"));
        digitalWrite(relay1Pin, HIGH);
      }
      if (receivedChar == '1') {
        Serial.println (F("Relay 1 OFF"));
        digitalWrite(relay1Pin, LOW);
      }
    }
  }
  // ******************* relay 2 *****************************
  if (strcmp(topic, "testing/relay2") == 0) {
    for (int i = 0; i < length; i++) {
      char receivedChar = (char)payload[i];
      if (receivedChar == '0') {
        Serial.println (F("Relay 2 ON"));
        digitalWrite(relay2Pin, HIGH);
      }
      if (receivedChar == '1') {
        Serial.println (F("Relay 2 OFF"));
        digitalWrite(relay2Pin, LOW);
      }
    }
  }
}

boolean reconnect() {  // **********************************************************
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print (F("Contacting MQTT server..."));
    // Attempt to connect
    if (client.connect(mqtt_clientID)) {
      Serial.println (F("connected"));
      
      // ... SUBSCRIBE TO TOPICS
      client.subscribe("testing/relay1");
      client.subscribe("testing/relay2");
      return client.connected();

      Serial.print (F("Failed to connect. "));
      Serial.println (F(" Attempting connection again in 3 seconds"));
      // Wait 3 seconds before retrying
      delay(3000);
      return 0;
    }
  }
}


void setup()
{
  {
    Serial.begin(9600);
    client.setServer(mqtt_server, mqtt_port);
    client.setCallback(callback);
    pinMode(relay1Pin, OUTPUT);
    pinMode(relay2Pin, OUTPUT);
  }


  // Connect to WiFinetwork
  Serial.println();
  Serial.println();
  Serial.print (F("Connecting to "));
  Serial.println(wifi_ssid);

  WiFi.begin(wifi_ssid, wifi_password);
  WiFi.hostname(HostName);
  WiFi.mode(WIFI_STA);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    //Serial.begin(9600);
    Serial.print (F("."));
  }
  Serial.println (F(""));
  Serial.println (F("WiFi connected"));
  // Print the IP address
  Serial.print (F("Local IP: "));
  Serial.println(WiFi.localIP());
}


void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();
}
//digitalWrite(relay1Pin, HIGH);   
