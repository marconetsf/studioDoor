/***************************************************
  Adafruit MQTT Library ESP8266 Example

  Must use ESP8266 Arduino from:
    https://github.com/esp8266/Arduino

  Works great with Adafruit's Huzzah ESP board & Feather
  ----> https://www.adafruit.com/product/2471
  ----> https://www.adafruit.com/products/2821

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Tony DiCola for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <Servo.h>

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "2G_PLAY"
#define WLAN_PASS       "18091986"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "m11.cloudmqtt.com"
#define AIO_SERVERPORT  15939                   // use 8883 for SSL
#define AIO_USERNAME    "akvvmyon"
#define AIO_KEY         "wioEGgmZ518b"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/statusDoor");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe door = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/studioDoor");

/*************************** Sketch Code ************************************/
const int pirSensor = D6;
const int greenLed = D1;
const int redLed = D4;
bool door1 = false;
bool sensorStatus = false;

Servo servoDoor;
// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup() {
  Serial.begin(115200);
  delay(10);

  servoDoor.attach(D2);
  servoDoor.write(0);

  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(pirSensor, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  mqtt.subscribe(&door);
}

uint32_t x = 0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();

  // this is our 'wait for incoming subscription packets' busy subloop
  // try to spend your time here

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &door) {
      Serial.print(F("Got: "));
      Serial.println((char *)door.lastread);
      if (strcmp((char *)door.lastread, "open") == 0) {
        servoDoor.write(90);
        door1 = true;
        if (! photocell.publish("DoorOpen")) {
          //Serial.println(F("Failed"));
        } else {
          //Serial.println(F("OK!"));
        }
      }
    }
    if (door1 == true) {
      sensorStatus = digitalRead(pirSensor); // Checking the sensor status
      if (sensorStatus == true) {
        Serial.println("Porta Fechada");
        servoDoor.write(0);
        door1 = false;
        if (!photocell.publish("DoorClose")) {
          //Serial.println(F("Failed"));
        } else {
          //Serial.println(F("OK!"));
        }
      }
    }
  }

  if (door1 == true) {
    sensorStatus = digitalRead(pirSensor); // Checking the sensor status
    if (sensorStatus == true) {
      Serial.println("Porta Fechada");
      servoDoor.write(0);
      door1 = false;
      if (!photocell.publish("DoorClose")) {
        //Serial.println(F("Failed"));
      } else {
        //Serial.println(F("OK!"));
      }
    }
  }

  if (! mqtt.ping()) {
    mqtt.disconnect();
  }

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
