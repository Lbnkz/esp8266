#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h>
#include "dht.h" //INCLUSÃO DE BIBLIOTECA

int prodX = 23;
int prodY = 25;

const int pinoDHT11 = 16; 
dht DHT; 

const int sensor_hc_trig = 12;
const int sensor_hc_eco = 14;

const char ssid[] = "tecnicoN";
const char password[] = "1234567L";

ESP8266WebServer server(80);

void printNetworkInfo() {
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void sendServerSucessReponse(String response) {
  server.sendHeader("Content-Type", "application/json");
  server.send(200, "application/json", response); 
}

void sendServerErrorReponse(String response) {
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["error"] = true;
  jsonDoc["message"] = response;
  String jsonStr;
  server.sendHeader("Content-Type", "application/json");
  server.send(400, "application/json", response); 
}

void routeNotFound() {
  sendServerErrorReponse("Resource not found.");
}

void routeSimpleGet() {
  DynamicJsonDocument jsonDoc(200);
  jsonDoc["temperatura"] = temperatura();
  jsonDoc["umidade"] = humidade();
  jsonDoc["prodX"] = prodX;
  jsonDoc["prodY"] = prodY;
  String jsonStr;
  serializeJson(jsonDoc, jsonStr);
  sendServerSucessReponse(jsonStr);
}

void connectNetwork(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  delay(1000);

  Serial.print("Connecting...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("Connected!");
}

void initializeServer() {
  // Create endpoints
  server.on("/simples-get", HTTP_GET, routeSimpleGet);
  server.onNotFound(routeNotFound);

  // Initialize server
  server.begin();
  Serial.println("HTTP server started");
}

// Main
void setup() {
  Serial.begin(115200);
  pinMode(sensor_hc_trig, OUTPUT);
  pinMode(sensor_hc_eco, INPUT);
  connectNetwork();
  initializeServer();
}

void loop() {
  productController();
  printNetworkInfo();
  server.handleClient();
}

int temperatura(){
  DHT.read11(pinoDHT11); 
  int temp = DHT.temperature;
  return temp;
}

int humidade(){
  DHT.read11(pinoDHT11); 
  int hum = DHT.humidity;
  return hum;
}
void productController() {
  double esteiraD = sensorHc();
  if(esteiraD < 11 && esteiraD > 9){
    prodY = prodY + 1;
  }
  else if(esteiraD < 9){
    prodX = prodX + 1;
  }
  
  delay(1500);
}


double sensorHc(){
  digitalWrite(sensor_hc_trig, HIGH);
  delayMicroseconds(10);
  digitalWrite(sensor_hc_trig, LOW);
  double duracao = pulseIn(sensor_hc_eco, HIGH);
  double dist = duracao * (0.034/2);
  
  return dist;
}
