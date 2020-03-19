
/*
  Este programa ira utilizar o módulo com 2 reles e o protocolo MQTT.
  Para realizar o teste deste programa será utlizado um aplicativo
  cliente MQTT chamado MQTT.fx e para Celular Android e IOS

  Neste exemplo será utilizado o broker www.cloudmqtt.com

  Autores: Norberto Hideaki Enomoto - norberto.enomoto@gmail.com
           Vitor Mazuco             - vitor.mazuco@gmail.com

  Data criação: 19/11/2016
*/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Servo.h>


const char* SSID          = "2G_PLAY"; //Nome da Rede
const char* PASSWORD      = "18091986"; //Senha da Rede

//Parâmetros do protocolo MQTT
const char* MQTT_SERVER   = "m11.cloudmqtt.com";
const int   MQTT_PORT     = 15939;
const char* MQTT_USER     = "akvvmyon";
const char* MQTT_PASSWORD = "wioEGgmZ518b";
const char* MQTT_CLIENT   = "ESP8266-Client";

//Tópicos de comunicação
const char* TOPICO_ESCUTA  = "akvvmyon/studioDoor";
const char* TOPICO_ENVIO   = "akvvmyon/statusDoor";

Servo servoDoor;

const int pirSensor = D6;
const int greenLed = D1;
const int redLed = D4;
bool door1 = false;
bool sensorStatus = false;

// Instanciamento do cliente
WiFiClient espClient;
PubSubClient MQTT(espClient);

//Função de conexão da Esp ao Wifi
void reconnectWiFi() {
  if (WiFi.status() == WL_CONNECTED)
    return;

  Serial.println("Conectando-se na rede: " + String(SSID));
  Serial.println("Aguarde");
  WiFi.begin(SSID, PASSWORD); //Estabelecimento de conexão

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi conectado");
  Serial.println("Endereco IP: ");
  Serial.println(WiFi.localIP());
}

// Função de recebimento de mensagem externa
void callback(char* topic, byte* payload, unsigned int length) {
  String msg;
  for (int i = 0; i < length; i++) {
    char c = (char)payload[i];
    msg += c;
  }

  Serial.print("Mensagem [");
  Serial.print(topic);
  Serial.print("]: " + msg);
  Serial.println();

  if (msg.equals("close")) {
    digitalWrite(LED_BUILTIN, LOW);
    servoDoor.write(0);
    MQTT.publish(TOPICO_ENVIO, "doorClosed");
  }
  else if (msg.equals("open")) {
    digitalWrite(LED_BUILTIN, HIGH);
    servoDoor.write(90);
    door1 = true;
    MQTT.publish(TOPICO_ENVIO, "doorOpen");
  }
}

void initMQTT() {
  MQTT.setServer(MQTT_SERVER, MQTT_PORT);
  MQTT.setCallback(callback);
}

// Função para conexão
void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("Tentado conectar ao broker MQTT: " + String(MQTT_SERVER) + ":" + String(MQTT_PORT) + " :");
    if (MQTT.connect(MQTT_CLIENT, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println("conectado");
      MQTT.publish(TOPICO_ENVIO, "Cliente conectado ao broker");
      MQTT.subscribe(TOPICO_ESCUTA);
    }
    else {
      Serial.print("falha, rc=");
      Serial.print(MQTT.state());
      Serial.println(" tenta novamente após 5 segundos");
      delay(5000);
    }
  }
}

void setup() {
  Serial.begin(115200);
  servoDoor.attach(D2);
  servoDoor.write(0);
  pinMode(greenLed, OUTPUT);
  pinMode(redLed, OUTPUT);
  pinMode(pirSensor, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);
  reconnectWiFi();
  initMQTT();
}


void loop() {
  if (door1 == true) {
    sensorStatus = digitalRead(pirSensor); // Checking the sensor status
    if (sensorStatus == true) {
      Serial.println("Porta Fechada");
      servoDoor.write(0);
      MQTT.publish(TOPICO_ENVIO, "doorClosed");
      door1 = false;
    }
  }
  reconnectWiFi();
  reconnectMQTT();
  MQTT.loop();
}
