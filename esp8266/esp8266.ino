#include <ESP8266HTTPClient.h>

#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>

#include "SSD1306Wire.h"
#include "ESPRotary.h";
#include "Button2.h";

// Definições do Wifi
#ifndef STASSID
#define STASSID "RIBEIRAO 2G"
#define STAPSK  "r24e56g18"
#endif

// Definições de identificadores na comunicação I2C
#define id_Hora "H" // Horário atual
#define id_Comp "C" // Compartimento de componentes
#define id_Qtd  "Q" // Quantidade de componentes no compartimento
#define id_Prop "P" // Proporcionalidade da quantidade de componentes
 
// Configurações de rede e do servidor
const char* ssid     = STASSID;
const char* password = STAPSK;
IPAddress server(192,168,15,8);
const uint16_t port = 8080;
const char* serverName = "http://192.168.15.8:8080";

// Endereços de cada módulo
const int addr1 = 0x58;// 0xB0 >>> 0x58

// Inicialização da tela LCD
//SSD1306Wire  display(0x3c, D1, D2);

// ================================================
void init_wifi(){
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.print("Connecting to ");
  Serial.println(serverName);
}

// ================================================
JSONVar get_server_update(){
  String jsonConfigString = httpGETRequest(serverName);
  JSONVar jsonConfig = JSON.parse(jsonConfigString);
  if (JSON.typeof(jsonConfig) == "undefined") {
    Serial.println("Parsing input failed!"); 
  }
  return jsonConfig;
}

String httpGETRequest(const char* serverName) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.getString();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }
  // Free resources
  http.end();

  return payload;
}

// ================================================
void post_server_update(JSONVar jsonConfig){
  String jsonString = JSON.stringify(jsonConfig);
  httpPOSTRequest(serverName, jsonString);
}

void httpPOSTRequest(const char* serverName, String jsonString) {
  WiFiClient client;
  HTTPClient http;
    
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(jsonString);
  Serial.print("HTTP Response code: ");
  Serial.println(httpResponseCode);

  http.end();
}

// ================================================
void post_slave_update(JSONVar jsonSlaveUpdate, String word1, String word2, String word3, const int addr){
   // Começo da transmissão I2C com o escravo
  Wire.beginTransmission(addr);

  // Transforma a String da informação a ser enviada em Char
  String dataSlave = JSON.stringify(jsonSlaveUpdate[word1][word2][word3]);
  char buffer[10];
  dataSlave.toCharArray(buffer, 10);

  Serial.print("Buffer de envio: ");
  Serial.print(buffer);
  Serial.println(".");

  // Envia buffer
  Wire.write(buffer); //jsonSlaveUpdate["Resistores 1"]["Resistores 1k"]["Quantidade"]

  // Fim da transmissão I2C com o escravo
  Wire.endTransmission();

  delay(100);
}

// ================================================
String get_slave_update(const int addr, const int bytes){
  String slaveString;

  // Requisita bytes do escravo
  Wire.requestFrom(addr, bytes);

  // Recebe Chars enquanto o escravo estiver enviando
  while(Wire.available()){
     char c = Wire.read();
     slaveString = slaveString + c;     
  }
  
  return slaveString;
}

// ================================================

void post_current_time(JSONVar jsonConfig, const int addr) {
  Serial.print("Hora recebida: ");
  Serial.println(jsonConfig["Funcionalidades"]["Horario"]["Atual"]);

  // Envia identificador da hora para o escravo
  Wire.beginTransmission(addr);
  Wire.write(id_Hora); // Identificador "H"
  Wire.endTransmission();

  // Envia hora para o escravo
  post_slave_update(jsonConfig, "Funcionalidades", "Horario", "Atual", addr);

  // Envia apenas Chars numéricos
  //Wire.write(buffer[1]); // Pula primeiro char porque é "
  //Wire.write(buffer[2]);
  //Wire.write(buffer[3]);
  //Wire.write(buffer[4]);
}

// ================================================
void setup() {
  Serial.begin(115200); /* begin serial for debug */
  
  // Wifi
  init_wifi();

  // Módulos
  Wire.begin(D1, D2); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  //Wire.setClock(100000);
  delay(500);
}

void loop() {
  JSONVar jsonConfig = get_server_update();

  post_current_time(jsonConfig, addr1);
  
  String hora = get_slave_update(addr1, 4);
  Serial.print("Hora do escravo: ");
  Serial.print(hora);
  Serial.println(".");
  
  
  delay(5000);
}
