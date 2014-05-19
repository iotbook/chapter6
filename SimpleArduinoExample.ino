#include <SPI.h>
#include <PubSubClient.h>
#include <WiFi.h>

#define  USER_ID   "<user ID>"
#define  TOKEN     "<authorization token>"

// interval to read "sensor" and publish value
#define PUBLISH_INTERVAL_MS  2000

#define MQTT_SERVER "q.m2m.io"

char ssid[] = "<WiFi SSID>";     // your network SSID (name) 
char pass[] = "<WiFi Passphrase>";  // your network password
int status = WL_IDLE_STATUS;  // the Wifi radio's status

WiFiClient wifiClient;
PubSubClient iotBookClient(MQTT_SERVER, 1883, iotBookCallback, wifiClient);

// define pins
int potPin = A0;
int greenLedPin = 3;
int redLedPin = 8;

// reserve space for building MQTT message payloads
char msg[50];

void setup()
{
  Serial.begin(9600);
  
  // setup I/O pins
  pinMode(greenLedPin, OUTPUT);
  pinMode(redLedPin, OUTPUT);
  digitalWrite(greenLedPin, LOW);
  digitalWrite(redLedPin, LOW);
      
  // connect to WiFi access point
  if (connectToWiFi() == false) {
    while(true);
  }
  
  // connect MQTT client
  iotBookClient.connect("I", USER_ID, TOKEN);
  delay(500);
  iotBookClient.subscribe("<domain>/apress/arduino/cmd");
}

void loop()
{
  // read potentiomenter "sensor"
  int potValue = analogRead(potPin);
  
  // publish value to the cloud over MQTT
  String sensorMsg = "{\"pot\":";
  sensorMsg.concat(potValue);
  sensorMsg.concat("}");
  Serial.println(sensorMsg);
  sensorMsg.toCharArray(msg, sensorMsg.length()+1);
  iotBookClient.publish("<domain>/apress/arduino", msg);
  
  // give MQTT client a chance to process
  iotBookClient.loop();
  
  // delay
  delay(PUBLISH_INTERVAL_MS);
}

void iotBookCallback(char* topic, byte* payload, unsigned int length) {
  // message received on subscribed topic
  // parse message to see if we need to act upon it

  // look for green LED command
  if (payload[2] == 'g') {
    if (payload[5] == '1') {
      digitalWrite(greenLedPin, HIGH);
    } else {
      digitalWrite(greenLedPin, LOW);
    }
  }

  // look for red LED command  
  if (payload[2] == 'r') {
    if (payload[5] == '1') {
      digitalWrite(redLedPin, HIGH);
    } else {
      digitalWrite(redLedPin, LOW);
    }
  }
}

boolean connectToWiFi() {
  Serial.print("Attempting to connect to WiFi...");
  status = WiFi.begin(ssid, pass);

  if (status != WL_CONNECTED) {
    Serial.print("ERROR: couldn't get a wifi connection.");
    return false;
  } else {
    Serial.print("connected.");
    return true;
  }
}
