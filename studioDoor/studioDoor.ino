/*********
  STOP and PLAY

  Program Algorithm 

    When the camera recongnizes the face it will send a request for esp and then the door will open
    When the door open, an pir sensor will be installed ahead of the door to see when the person left of the door range. When it happen, door will close 
  
*********/

#include <ESP8266WiFi.h>      // Load Wi-Fi library
#include <ESP8266WebServer.h> // Web server library
#include <ArduinoOTA.h>       // Over the air
#include <Servo.h>

// Replace with your network credentials
const char* ssid = "MAPLE ADM";
const char* password = "mapleadm";
const int pirSensor = D6;

Servo door;
ESP8266WebServer server(80);  // Iniciate an objetc of ESP8266WebServer class

IPAddress staticIP(192, 168, 10, 132);
IPAddress gateway(192, 168, 10, 1);
IPAddress subnet(255, 255, 255, 0); 

const int greenLed = D3;
const int redLed = D4;

bool door1 = false;
bool sensorStatus = false;

////////////////////////////////////////////////////////////////////////
void setup() {
  Serial.begin(115200); // Inicialize a Serial communication with the computer with 9600 of baud rate
  door.attach(D5);
  // Initialize the output variables as outputs

  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(pirSensor, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.config(staticIP, gateway, subnet);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  pisca(10, 200);
  
  server.on("/opendoor", ligar1);
  server.begin();

  beginOTA();
}

void loop() {
  ArduinoOTA.handle();    // Function who listen if a WiFi boot is comming
  server.handleClient();  // Function who listen the requests pre-configured on setup funciont

  // When the wifi signal is interrupted the esp's led goes on 
  if (WiFi.status() == true){  
    digitalWrite(LED_BUILTIN, LOW);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
  }
  
  if (door1 == true){ // Set on the led for 3 seconds 
    sensorStatus = digitalRead(pirSensor);
    if (sensorStatus == true){
      door.write(0);
    }
  }

  yield(); // Function that ensures that the background functions of esp will keep responding, like tcpip protocol, wifi connection etc... 
}

//////////////////////// FUNCTIONS ////////////////////////////////////
void ligar1() {                 // Open the First Door
  door.write(90);  // set HIGH the D1 port 
  delay(500);                   // Delay for 0.3 seconds
  server.send(200, "text/html", "{\"status\":\"recebido\"}"); //send a json response to the client 
  door1 = true;
}

void pisca(int vezes, int tempo) {
  for (int d = 0; d <= vezes; d++) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(tempo);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(tempo);
  }
}

// Function OTA - Over the Air
void startOTA(){
  String type;
  
  // If the update is being engravin on flash mame it returns "flash"
  if (ArduinoOTA.getCommand() == U_FLASH) type = "flash";
  else type = "filesystem"; // U_SPIFFS // If the update is being engraving by intern memory it returns "filesystem"
  
  //Show the returned information
  Serial.println("Start updating " + type);
}

void endOTA(){
  Serial.println("\nEnd");
}

// It shows the progress of engraving 
void progressOTA(unsigned int progress, unsigned int total){
  Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
}

void errorOTA(ota_error_t error){
  Serial.printf("Error[%u]: ", error);

  if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
  else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
  else if (error ==  OTA_CONNECT_ERROR) Serial.println("Connect Failed");
  else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
  else if (error == OTA_END_ERROR) Serial.println("End Failed");
}

void beginOTA(){
  //define what function will be run when the OTA process begins
  ArduinoOTA.onStart(startOTA);

  //define what function will be run when the OTA process ends
  ArduinoOTA.onEnd(endOTA);

  //define what will be run when the OTA process is in gravin process
  ArduinoOTA.onProgress(progressOTA);

  //define what will be run when the OTA process find an error
  ArduinoOTA.onError(errorOTA);

  //Inicializate ArduinoOTA
  ArduinoOTA.begin();
}
