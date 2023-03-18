/* Observações:
 *  1) Funcional para quantidades de componentes entre 0 e 99, caso a quantidade supere esse número, serão necessárias mudanças no código (quantity_arr, limit_arr)
 */  

//#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <SPI.h>

// ========================================================================
// ======== Configurações a serem recebidas do ESP de configuração ========
// ========================================================================
// Definições do Wifi
#ifndef STASSID
#define STASSID "Toriko" //"RIBEIRAO 2G"
#define STAPSK  "12345678" //"r24e56g18"
#endif
// Definições de conexão com o servidor
#define SERVER_NAME "http://192.168.43.83:8080"
// Definições do I2C
#define MODULO_1 "Capacitor eletrolitico 1" // MUDAR PRO SERVIDOR!!!!!!!!!!!!!!!!!
#define MODULO_2 "Capacitor eletrolitico 2"
typedef struct {
  String type;
  int addr;
}  moduleProprieties;
// Ordem: esquerda para direita, de cima para baixo
moduleProprieties modules[2] = { {MODULO_1, 0x22},   // 0x44 (Pic) >>> 0x22 // Placa 1 (mais antiga)
                                 {MODULO_2, 0x59} }; // 0xB2 (Pic) >>> 0x59 // Placa 2 (mais recente)
const int numModules = sizeof(modules) / sizeof(modules[0]);
// Definições do MQTT
#define MQTT_BROKER "192.168.43.83"
#define MQTT_PORT 1883
#define PROJECT_ID "/2" // Identificação do número do projeto -> Segundo projeto (o primeiro é o servidor)
class Dicionario { // Essa classe serve para armazenar strings comuns na criação dos tópicos
  public:
    String output = "/Outputs";
    String input = "/Inputs";
}
DICIONARIO = Dicionario();
class ComunicacaoEntreProjetosTopicos { // Essa classe serve para criar os tópicos de comunicação entre projetos -> Ex: Quando o Painel de Armazenamento ligar, publicar aqui
  public:
    String comunicacao    = "Comunicacao";
    String inicializacoes = comunicacao + "/Inicializacoes";
}
COMUNICACAO_TOPIC = ComunicacaoEntreProjetosTopicos();
class PainelDeArmazenamentoModularTopicos {
  public: 
    String projeto         = "PainelDeArmazenamentoModular";
    String hora            = projeto       + "/Hora";             // Publicado - Indica qual a hora atual
    String modulo          = projeto       + "/Modulo";           // Publicado - Indica qual módulo de armazenamento está selecionado
    String componente      = modulo        + "/NomeDoComponente"; // Publicado - Indica qual o nome do componente dentro do json do tipo de componente
    String moduloOutputs   = modulo        + DICIONARIO.output;
    String outDisp         = moduloOutputs + "/Display";          // Publicado - Output: display de 7 segmentos do módulo
    String outLeds         = moduloOutputs + "/Leds";             // Publicado - Output: matriz de leds rgb 3x3 do módulo
    String moduloInputs    = modulo        + DICIONARIO.input;


}
PAINEL_TOPIC = PainelDeArmazenamentoModularTopicos();
String subscriptionList[6] = {PAINEL_TOPIC.hora                + "/#",
                              PAINEL_TOPIC.modulo              + "/#",
                              PAINEL_TOPIC.componente          + "/#",
                              PAINEL_TOPIC.outDisp             + "/#",
                              PAINEL_TOPIC.outLeds             + "/#",
                              COMUNICACAO_TOPIC.inicializacoes + "/#"};
const int numSubscriptions = sizeof(subscriptionList) / sizeof(subscriptionList[0]);
// ========================================================================

// Definições de identificadores na comunicação I2C
#define id_Hora "H" // Horário atual
#define id_Slct "S" // Modo de seleção
#define id_Comp "C" // Compartimento de componentes
#define id_AQtd "A" // Todas as quantidades dos componentes
#define id_Qtd  "Q" // Quantidade de componentes no compartimento
#define id_Lim  "L" // Limite da quantidade de componentes

// Definições de pinagem
/* I2C */
#define SDA_I2C_PIN D2 // Default pin
#define SCL_I2C_PIN D1 // Default pin
/* Encoder */
#define ROTARY_PIN1       3  // Pino RX -> CLK
#define ROTARY_PIN2       D4 // -> DT
#define ROTARY_BUTTON_PIN D0 // -> SW
/* Display */
#define OLED_MOSI  D7 // D1
#define OLED_CLK   D5 // D0
#define OLED_DC    D3
#define OLED_CS    D8
#define OLED_RESET 1 // TX

// Outras definições
#define SEND_TENTATIVES   1
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Inicializações de classes
 
// Configurações de rede, do servidor e do MQTT
const char* ssid       = STASSID;
const char* password   = STAPSK;
const char* serverName = SERVER_NAME;
const char* mqttBroker = MQTT_BROKER;
const int   mqttPort   = MQTT_PORT;
WiFiClient wifi;
//HTTPClient http;
PubSubClient mqtt(wifi);

// Variáveis de envio de mensagens, seleção de compartimento, chave rotativa e objeto json
volatile bool send_selected_led = false;
volatile bool send_selection_mode = true;
volatile bool send_server_update = false;
volatile bool send_deselect_module = false;
volatile bool send_selected_quantity = false;
volatile byte send_tries = 0;

volatile byte selectionMode = 0; // 0 = sem seleção, 1 = selecionando módulo, 2 = selecionando componente, 3 = alterando valor do componente
volatile int selectedLed = 1;
volatile int selectedModule = 1;
volatile int selectedQuantity = 0;
volatile byte selectedQttLast = 0;
volatile byte selectedLedLast  = 1;
volatile byte selectedModuleLast = 1;

volatile bool btn_click = false;
volatile bool btn_reset = false;
volatile byte loop_counter = 0;

StaticJsonDocument<512> jsonConfig;

// Inicia variáveis de tempo
unsigned long millisTarefa1 = millis();
unsigned long update_time = 0; // Inicia em 0 para atualizar imediatamente, depois muda para o intervalo

// ================================================
// ======== Wifi and I2C Initializations ==========
// ================================================
void init_wifi(){
  // We start by connecting to a WiFi network

  //Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
  // would try to act as both a client and an access-point and could cause
  // network-issues with your other WiFi-devices on your WiFi-network.


  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  byte wifi_tries = 0;
  byte status = 1;

  while (WiFi.status() != WL_CONNECTED and wifi_tries < 42) {
    delay(500);
    if (wifi_tries%2 == 0) {
      status++;
      if (status == 4) status = 1;
    }
    wifi_tries++;
  }

  if (WiFi.status() != WL_CONNECTED) for (;;); // Trava a execução
}


void establish_i2c() {
  for (int module = 0; module < numModules; module++) {
    String limit_arr = get_limit_array(module);
    String return_msg = post_all_limits(limit_arr, module);
  }
}

// ================================================
// =============== MQTT Functions =================
// ================================================
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // Payload to String conversion
  int i = 0;
  char strArrayPayload[length+1];
  for (i=0; i < length; i++) {
    strArrayPayload[i]=(char)payload[i];
  }
  strArrayPayload[i] = 0;
  // Array of char with null termination = String

  String topicString = String(topic);

  // Importante: A ordem de verificação do tópico recebido é do mais específico para o menos específico
  // Formato do tópico: PainelDeArmazenamentoModular/Modulo/.../'PROJECT_ID'[+ payload = /'Value']
  // TÓPICO COMPONENTE -> Identifica o tipo de componente (Ex: Capacitor Eletrolitico 1)
  if (topicString.indexOf(PAINEL_TOPIC.componente) >= 0 && topicString.substring(PAINEL_TOPIC.componente.length()) != PROJECT_ID) {   
    deserializeJson(jsonConfig, strArrayPayload); // Modifica a string original
    
    const char* id1 = jsonConfig["1. 1uF"]["ID"];
    mqtt.publish("Result", strArrayPayload);
  }

  // TÓPICO MODULO -> Determina o módulo selecionado
  else if (topicString.indexOf(PAINEL_TOPIC.modulo) >= 0 && topicString.substring(PAINEL_TOPIC.modulo.length()) != PROJECT_ID) { 
    String idOfSender = topicString.substring(PAINEL_TOPIC.modulo.length());
    String topicToPublish = PAINEL_TOPIC.modulo + PROJECT_ID;
    
    // -> Convert string to char array to send in mqtt
    int strLen = topicToPublish.length() + 1; 
    char charArrayTopic[strLen];
    topicToPublish.toCharArray(charArrayTopic, strLen);
    
    mqtt.publish(charArrayTopic, strArrayPayload);

    // FAZER: ATUALIZAR MODULO SELECIONADO  
  }

}

void mqtt_publish() {
  //int str_len = teste.length() + 1; 
  //char char_array[str_len];
  //teste.toCharArray(char_array, str_len);

  //mqtt.publish(char_array, strArrayPayload);
}

void mqtt_reconnect() {
  String clientId = "ESP8266Client-1";
  //clientId += String(random(0xffff), HEX);
  
  mqtt.connect(clientId.c_str());
  mqtt_resubscribe();
}

void mqtt_resubscribe() {
  for (int i = 0; i<numSubscriptions; i++) {
    char buff[128];
    subscriptionList[i].toCharArray(buff, 128);
    mqtt.subscribe(buff);
  }
}

// ================================================
// ============ Get request to server =============
// ================================================
/*
void get_server_update() {
  httpGETRequest(serverName); // Indicate to server the connection, to get the config json values for current module
}

String httpGETRequest(const char* serverName) {  
  // Your IP address with path or Domain name with URL path 
  http.begin(wifi, serverName);
  
  // Send HTTP GET request
  int httpResponseCode = http.GET();
  
  String payload = "{}"; 
  
  if (httpResponseCode>0) {
    payload = http.getString(); 
  }
  else {
    for(;;);
  }
  // Free resources
  http.end();

  return payload;
}
*/

// =========================================================
// ====== Generic post and get slave update functions ======
// =========================================================
void post_slave_update_from_server(String word1, String word2, String word3, char* msg_id, const int addr) {
  String msg;
  char buff[22];

  // Transforma a String da informação a ser enviada em Char
  msg = String(jsonConfig[word2][word3]);
  msg = msg_id + msg + msg_id;
  msg.toCharArray(buff, 22);

  // Começo da transmissão I2C com o escravo
  Wire.beginTransmission(addr);

  // Envia identificador da mensagem e a mensagem
  Wire.write(buff);

  // Fim da transmissão I2C com o escravo
  Wire.endTransmission();

  delay(50);
}

void post_slave_update_from_esp(String pkg, char* msg_id, const int addr) {
  // Buffer
  pkg = msg_id + pkg + msg_id;
  char buff[22];
  pkg.toCharArray(buff, 22);

  // Começo da transmissão I2C com o escravo
  Wire.beginTransmission(addr);
  
  // Envia identificador da mensagem e a mensagem
  Wire.write(buff);

  // Fim da transmissão I2C com o escravo
  Wire.endTransmission();

  delay(50);
}

String get_slave_update(const int addr, const int bytes) {
  String return_msg;

  // Requisita bytes do escravo
  Wire.requestFrom(addr, bytes);

  // Recebe Chars enquanto o escravo estiver enviando
  while(Wire.available()){
     char c = Wire.read();
     return_msg = return_msg + c;     
  }
  
  return return_msg;
}

// ================================================
// ======= Specific slave update functions ========
// ================================================

String post_current_time(int module) { 
  // Envia hora para o escravo e retorna
  post_slave_update_from_server("Funcionalidades", "Horario", "Atual", id_Hora, modules[module].addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_deselect() {
  String msg = "0";
  post_slave_update_from_esp(msg, id_Slct, modules[selectedModuleLast-1].addr);
  //return get_slave_update(modules[selectedModuleLast-1].addr, 6);
  return "";
}

String post_selection_mode() {
  String msg = String(selectionMode);
  post_slave_update_from_esp(msg, id_Slct, modules[selectedModule-1].addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_selected_led() {
  String msg = String(selectedLed);
  post_slave_update_from_esp(msg, id_Comp, modules[selectedModule-1].addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_quantity(int current_quantity) {
  String msg = "00";
  if (current_quantity < 10) msg = "0" + String(current_quantity);
  else if (current_quantity < 100) msg = String(current_quantity);
  post_slave_update_from_esp(msg, id_Qtd, modules[selectedModule-1].addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_all_quantities(String qtt_arr, int module) {
  post_slave_update_from_esp(qtt_arr, id_AQtd, modules[module].addr);
  //return get_slave_update(modules[selectedModule-1].addr, 20);
  return "";
}

String post_all_limits(String lim_arr, int module) {
  post_slave_update_from_esp(lim_arr, id_Lim, modules[module].addr);
  //return get_slave_update(modules[module].addr, 20);
  return "";
}


// ================================================
// ============ Json config functions =============
// ================================================

String get_quantity_array(int module) {
  JsonObject root = jsonConfig.as<JsonObject>();
  String quantidade_str = "000000000000000000";
  String categoria, componente, quantidade;
  int j, compartimento;


  return quantidade_str;
}

String get_limit_array(int module) {
  JsonObject root = jsonConfig.as<JsonObject>();
  String limite_str = "000000000000000000";
  String categoria, componente, limite;
  int j, compartimento;


  return limite_str;
}

/*
void generic_config_scan() {
  const int numCategorias = jsonConfig.keys().length();
  String categoria, componente;
  int i, j;

  for (i=0; i<numCategorias; i++) {
    categoria = JSON.stringify(jsonConfig.keys()[i]);
    categoria = categoria.substring(1,categoria.length()-1);

    if (categoria != "Funcionalidades") {
      for (j=0; j<9; j++) {
        componente = JSON.stringify(jsonConfig[categoria].keys()[j]);
        componente = componente.substring(1,componente.length()-1);

        //Serial.println(jsonConfig[categoria][componente]["ID"]);
      }
    }
  }
}
*/

// ==========================================
// ================== Main loops ==================
// ================================================

void setup() {
  //Serial.begin(115200);

  // ============= Peripherals setup =============

  // ============= Connections setup =============
  // Connect to Wifi
  init_wifi();
  delay(250);
  // Connect to Server => ***********  Disabling so we only use MQTT for communication ***********
  //String payload = httpGETRequest(serverName);
  //if (payload == "{}") {
  //  for(;;); // Trava a execução
  //}

  //delay(250);
  // Connect to MQTT Broker
  mqtt.setServer(mqttBroker, mqttPort);
  delay(250);
  // ==========================================
  
  // ============= Initializations setup ============= 
  // Configure screen for main menu
  // Begin I2C communication
  Wire.begin(SDA_I2C_PIN, SCL_I2C_PIN);
  // Establish I2C connection with every slave and send quantities limits
  establish_i2c();
  // Set callback function of MQTT for receiving messages, check state and subscribe
  mqtt.setCallback(mqtt_callback);
  mqtt_resubscribe();
  delay(250);
}

void loop() {
  static String quantity_arr;

  if (!mqtt.connected()) mqtt_reconnect();
  mqtt.loop();
  

  if((millis() - millisTarefa1) > update_time){
     for (int module = 0; module < numModules; module++) {
      quantity_arr = get_quantity_array(module);
      post_all_quantities(quantity_arr, module);
      post_current_time(module);
     }  
     update_time = 6000;
     millisTarefa1 = millis();
  }


}
