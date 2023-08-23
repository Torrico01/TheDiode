/* Observações:
 *  1) Funcional para quantidades de componentes entre 0 e 99, caso a quantidade supere esse número, serão necessárias mudanças no código (quantity_arr, limit_arr)
 */  

#include <ESP8266HTTPClient.h>
#include <ErriezRotaryFullStep.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ArduinoJson.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <Dictionary.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <SPI.h>
#include "Button2.h";
#define len(arr) sizeof (arr)/sizeof (arr[0])

// ========================================================================
// ======== Configurações a serem recebidas do ESP de configuração ========
// ========================================================================
// -------- Definições do Wifi --------
#ifndef STASSID
#define STASSID "RIBEIRAO 2.4G" // "Toriko"
#define STAPSK  "r28e61g23" // "12345678"
#endif
// -------- Definições do servidor --------
#define SERVER_NAME "http://192.168.15.16:8080"
#define BASE "Painel Base 1"
#define BASE_ID "2"
#define MODULO_1 "Capacitor Eletrolitico 1"
#define MODULO_1_ID "3"
#define MODULO_1_ADDR "22" // 0x44 (Pic) >>> 0x22 (Esp) // Placa 1 (mais antiga)
#define MODULO_2 "Capacitor Eletrolitico 2"
#define MODULO_2_ID "4"
#define MODULO_2_ADDR "59" // 0xB2 (Pic) >>> 0x59 (Esp) // Placa 2 (mais recente)
// Identifica os projectos (base + todos os painéis conectados)
Dictionary &projectIds = *(new Dictionary(3));

// -------- Definições do I2C --------
// Identifica os painéis e seus endereços I2C. Ordem: esquerda para direita, de cima para baixo
Dictionary &moduleAddrs = *(new Dictionary(2));
int numModules;

// -------- Definições do MQTT --------
#define MQTT_BROKER "192.168.15.16"
#define MQTT_PORT 1883

class CommunicationTopic {
  public:
    String communication = "Communication";
    String connection = "Connection";
}
COMMUNICATION_TOPIC = CommunicationTopic();
class ModularStoragePanelBaseTopic {
  public: 
    String project   = "ModularStoragePanelBase";
    String module    = "Module";
    String outScreen = "Outputs/Screen";
}
PANEL_BASE_TOPIC = ModularStoragePanelBaseTopic();
class ModularStoragePanelTopic {
  public: 
    String project       = "ModularStoragePanel";
    String configuration = "Configuration";
    String outDisplay    = "Outputs/Display";
}
PANEL_TOPIC = ModularStoragePanelTopic();
String subscriptionList[7] = {PANEL_BASE_TOPIC.project + "/" + BASE + "/" + PANEL_BASE_TOPIC.module + "/#",
                              PANEL_BASE_TOPIC.project + "/" + BASE + "/" + PANEL_BASE_TOPIC.outScreen + "/#",
                              PANEL_TOPIC.project + "/" + MODULO_1 + "/" + PANEL_TOPIC.configuration + "/#",
                              PANEL_TOPIC.project + "/" + MODULO_1 + "/" + PANEL_TOPIC.outDisplay + "/#",
                              PANEL_TOPIC.project + "/" + MODULO_2 + "/" + PANEL_TOPIC.configuration + "/#",
                              PANEL_TOPIC.project + "/" + MODULO_2 + "/" + PANEL_TOPIC.outDisplay + "/#",
                              COMMUNICATION_TOPIC.communication + "/" + COMMUNICATION_TOPIC.connection + "/#"}; // No project name cause requester identfies the connection
const int numSubscriptions = len(subscriptionList);
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
#define SDA_I2C_PIN D2 // Default pin (green)
#define SCL_I2C_PIN D1 // Default pin (yellow)
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
/* Encoder */
RotaryFullStep rotary(ROTARY_PIN1, ROTARY_PIN2);
Button2 btnRotary;
/* Display */
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
 
// Configurações de rede, do servidor e do MQTT
const char* ssid       = STASSID;
const char* password   = STAPSK;
const char* serverName = SERVER_NAME;
const char* mqttBroker = MQTT_BROKER;
const int   mqttPort   = MQTT_PORT;
WiFiClient wifi;
HTTPClient http;
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
String quantity_arr;
String limit_arr;

volatile bool btn_click = false;
volatile bool btn_reset = false;
volatile byte loop_counter = 0;

StaticJsonDocument<1024> jsonConfig;

// Declaração de funções
void rotaryInterrupt();
void wifi_connection_screen(byte);

// Inicia variáveis de tempo
unsigned long millisTarefa1 = millis();
unsigned long update_time = 0; // Inicia em 0 para atualizar imediatamente, depois muda para o intervalo

// ================================================
// ======== Wifi and I2C Initializations ==========
// ================================================
void init_wifi(){
  wifi_connection_screen(0);

  // We start by connecting to a WiFi network
  /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  byte wifi_tries = 0;
  byte status = 1;

  while (WiFi.status() != WL_CONNECTED and wifi_tries < 42) {
    delay(500);
    if (wifi_tries%2 == 0) {
      wifi_connection_screen(status);
      status++;
      if (status == 4) status = 1;
    }
    wifi_tries++;
  }

  if (WiFi.status() == WL_CONNECTED) wifi_connection_screen(4); // Status 4 = Conectado
  else {
    wifi_connection_screen(5); // Status 5 = Erro de conexão
    for (;;); // Trava a execução
  }
}

// ================================================
// =============== MQTT Functions =================
// ================================================

unsigned long hexStrToInt(String str) {
   char buffer [256];
   unsigned long ul;
   str.toCharArray(buffer, str.length() + 1);
   ul = strtoul(buffer, 0, 16);
   return ul;
}

void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  // Payload to String conversion
  int i = 0;
  char strArrayPayload[length+1];
  for (i=0; i < length; i++) {
    strArrayPayload[i]=(char)payload[i];
  }
  strArrayPayload[i] = 0;
  // Array of char with null termination = String

  // Split topic string
  String topicString = String(topic);
  String topicStringToSplit = String(topic);
  String topicSplitString[20];
  String lastTopicSplitString;
  int StringCount = 0;
  while (topicStringToSplit.length() > 0) {
    int index = topicStringToSplit.indexOf("/");
    if (index == -1) // No / found
    {
      topicSplitString[StringCount++] = topicStringToSplit;
      lastTopicSplitString = topicStringToSplit;
      break;
    }
    else
    {
      topicSplitString[StringCount++] = topicStringToSplit.substring(0, index);
      topicStringToSplit = topicStringToSplit.substring(index+1);
    }
  }

  // If message received was not published by itself
  if (lastTopicSplitString != BASE_ID) {
    // ModularStoragePanelBase
    if (topicString.indexOf(PANEL_BASE_TOPIC.project) >= 0) {
      // Module
      if (topicString.indexOf(PANEL_BASE_TOPIC.module) >= 0) selectedModule = atoi(strArrayPayload);
      // Screen
      if (topicString.indexOf(PANEL_BASE_TOPIC.outScreen) >= 0) { }
    }
    // ModularStoragePanel
    if (topicString.indexOf(PANEL_TOPIC.project) >= 0) {
      // Configuration
      if (topicString.indexOf(PANEL_TOPIC.configuration) >= 0) {
        String requested_project_name = topicSplitString[1];
        String requester_project_number = topicSplitString[3];
        String requested_id_str = projectIds[requested_project_name];
        int requested_id = hexStrToInt(requested_id_str);
        
        deserializeJson(jsonConfig, payload, length); // Modifica a string original
        get_limit_array();
        get_quantity_array();
        post_all_limits(limit_arr, requested_id);
        post_all_quantities(quantity_arr, requested_id);
      }
    }
  }
  
}

void mqtt_reconnect() {
  String clientId = BASE; 
  if (mqtt.connect(clientId.c_str())) {
    mqtt.setBufferSize(1024);
    mqtt_resubscribe();
  }
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
    server_connection_screen(2); // Não conectado
    for(;;);
  }
  // Free resources
  http.end();

  return payload;
}

// =========================================================
// ====== Generic post and get slave update functions ======
// =========================================================
void post_slave_update_from_server(String word1, String word2, String word3, char* msg_id, const int addr) {
  String msg;
  char buff[22];

  DynamicJsonDocument jsonConfig(1024); // delete

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
  String module_addr_str = moduleAddrs[module-1];
  int module_addr = hexStrToInt(module_addr_str);
  // Envia hora para o escravo e retorna
  post_slave_update_from_esp("1234", id_Hora, module_addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_deselect() {
  String msg = "0";
  String module_addr_str = moduleAddrs[selectedModuleLast-1];
  int module_addr = hexStrToInt(module_addr_str);
  post_slave_update_from_esp(msg, id_Slct, module_addr);
  //return get_slave_update(modules[selectedModuleLast-1].addr, 6);
  return "";
}

String post_selection_mode() {
  String msg = String(selectionMode);
  String module_addr_str = moduleAddrs[selectedModule-1];
  int module_addr = hexStrToInt(module_addr_str);
  post_slave_update_from_esp(msg, id_Slct, module_addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_selected_led() {
  String msg = String(selectedLed);
  String module_addr_str = moduleAddrs[selectedModule-1];
  int module_addr = hexStrToInt(module_addr_str);
  post_slave_update_from_esp(msg, id_Comp, module_addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_quantity(int current_quantity) {
  String msg = "00";
  if (current_quantity < 10) msg = "0" + String(current_quantity);
  else if (current_quantity < 100) msg = String(current_quantity);
  String module_addr_str = moduleAddrs[selectedModule-1];
  int module_addr = hexStrToInt(module_addr_str);
  post_slave_update_from_esp(msg, id_Qtd, module_addr);
  //return get_slave_update(modules[selectedModule-1].addr, 6);
  return "";
}

String post_all_quantities(String qtt_arr, int module) {
  String module_addr_str = moduleAddrs[module-1];
  int module_addr = hexStrToInt(module_addr_str);
  post_slave_update_from_esp(qtt_arr, id_AQtd, module_addr);
  //return get_slave_update(modules[selectedModule-1].addr, 20);
  return "";
}

String post_all_limits(String lim_arr, int module) {
  String module_addr_str = moduleAddrs[module-1];
  int module_addr = hexStrToInt(module_addr_str);
  post_slave_update_from_esp(lim_arr, id_Lim, module_addr);
  //return get_slave_update(modules[module].addr, 20);
  return "";
}

// ================================================
// ============= Rotary switch loops ==============
// ================================================

// single click
void click(Button2& btn) {
  btn_click = true;
}

// long click
void resetPosition(Button2& btn) {
  btn_reset = true;
}

void button_changes() {
  if (btn_reset == true) { // Reset press
    if (selectionMode > 0) selectionMode--;
  
    send_tries = 0;
    send_selection_mode = true;
    update_screen_values();
  }
  else if (btn_click == true) { // Click
    if (selectionMode == 3) send_server_update = true;
    if (selectionMode < 3) {
      selectionMode++;
      send_selection_mode = true;
    }
    send_tries = 0;
    update_screen_values();
  }
}

// ================================================
// ============ Json config functions =============
// ================================================

void get_quantity_array() {
  JsonObject root = jsonConfig.as<JsonObject>();
  String quantidade_str = "000000000000000000";
  String categoria, componente;
  int j, compartimento;
  const char* quantidade;

  for (JsonPair key_value : root) {
    componente = String(key_value.key().c_str());

    compartimento = componente.substring(0,2).toInt();
    quantidade = jsonConfig[componente]["Qtd"];
    if (atoi(quantidade) < 10) {
      quantidade_str[2*compartimento-2] = '0';
      quantidade_str[2*compartimento-1] = quantidade[0];
    }
    else {
      quantidade_str[2*compartimento-2] = quantidade[0];
      quantidade_str[2*compartimento-1] = quantidade[1];
    }
  }
  
  quantity_arr = quantidade_str;
  return ;
}

void get_limit_array() {
  JsonObject root = jsonConfig.as<JsonObject>();
  String limite_str = "000000000000000000";
  String categoria, componente;
  int j, compartimento;
  const char* limite;

  for (JsonPair key_value : root) {
    componente = String(key_value.key().c_str());

    compartimento = componente.substring(0,2).toInt();
    limite = jsonConfig[componente]["Lim"];
    if (atoi(limite) < 10) {
      limite_str[2*compartimento-2] = '0';
      limite_str[2*compartimento-1] = limite[0];
    }
    else {
      limite_str[2*compartimento-2] = limite[0];
      limite_str[2*compartimento-1] = limite[1];
    }
  }  
  
  limit_arr = limite_str;
  return ;
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

// ================================================
// ================== OLED screen =================
// ================================================

void wifi_connection_screen(byte status) {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  if (status < 4) {
    display.print("Conectando no WiFi");
    display.drawRect(16,16,96,32,SSD1306_WHITE); //display.drawRect(x0, y0, x1 -> 128, y1 -> 64, SSD1306_WHITE);
    // Infill
    byte bar = map(status,0,3,23,92) ;
    display.fillRect(18,18,bar,28,SSD1306_WHITE);
  }
  else if (status == 4) {
    display.setTextColor(SSD1306_INVERSE);
    display.print("Wifi conectado!");
  }
  else if (status == 5) {
    display.setTextColor(SSD1306_INVERSE);
    display.print("Erro de conexao WiFi");
  }

  display.display();
}

void server_connection_screen(byte status) {
  display.clearDisplay();
  display.setTextSize(1);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.println("Conectando no Server");
  if (status == 1) {
    display.setTextColor(SSD1306_INVERSE);
    display.print("Server conectado!");
  }
  else if (status == 2) {
    display.setTextColor(SSD1306_INVERSE);
    display.print("Erro de conexao com o servidor");
  }

  display.display();
}

void update_screen_values(void) {
  display.clearDisplay();

  display.setTextSize(2);      // Normal 1:1 pixel scale
  display.setTextColor(SSD1306_WHITE); // Draw white text
  display.setCursor(0, 0);     // Start at top-left corner
  display.cp437(true);         // Use full 256 char 'Code Page 437' font

  display.print("Modo: ");
  display.println(selectionMode);
  display.print("LED: ");
  display.println(selectedLed);

  display.display();
}

// ================================================
// ================== Main loops ==================
// ================================================

void setup() {
  //Serial.begin(115200);

  // ============= Peripherals setup =============
  // OLED Screen
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    for(;;); // Don't proceed, loop forever
  }
  display.display(); // Show initial display buffer contents on the screen the library initializes this with an Adafruit splash screen.
  delay(2000);
  display.clearDisplay();  
  // Rotary switch
  // Initialize pin change interrupt on both rotary encoder pins
  //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN1), rotaryInterrupt, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN2), rotaryInterrupt, CHANGE);
  btnRotary.begin(ROTARY_BUTTON_PIN);
  btnRotary.setTapHandler(click);
  btnRotary.setLongClickHandler(resetPosition);
  
  // ============= Connections setup =============
  // Connect to Wifi
  init_wifi();
  delay(250);
  // Connect to Server
  server_connection_screen(0);// Conectando no Servidor
  String payload = httpGETRequest(serverName);
  if (payload == "{}") {
    server_connection_screen(2); // Erro de conexão
    for(;;); // Trava a execução
  }
  else server_connection_screen(1); // Server conectado
  delay(250);
  // Connect to MQTT Broker
  mqtt.setServer(mqttBroker, mqttPort);
  delay(250);
  // ==========================================
  
  // ============= Initializations setup =============
  // Projects ids dictionary
  projectIds(BASE, BASE_ID);
  projectIds(MODULO_1, MODULO_1_ID);
  projectIds(MODULO_2, MODULO_2_ID);
  // Modules addresses dictionary
  moduleAddrs(MODULO_1, MODULO_1_ADDR); 
  moduleAddrs(MODULO_2, MODULO_2_ADDR);
  numModules = moduleAddrs.count();
  // Configure screen for main menu
  update_screen_values();
  // Begin I2C communication
  Wire.begin(SDA_I2C_PIN, SCL_I2C_PIN);
  // Set callback function of MQTT for receiving messages, check state and subscribe
  mqtt.setCallback(mqtt_callback);
  //mqtt_resubscribe();
  delay(250);
}

void loop() {
  bool mqttConnected = mqtt.connected();
  
  if (!mqttConnected) {
    mqtt_reconnect();//------------------------------------------------------>
    String topic = COMMUNICATION_TOPIC.communication + "/" + COMMUNICATION_TOPIC.connection + "/" + BASE_ID;
    mqtt.publish(topic.c_str(), "ON");
  }
  mqtt.loop();//------------------------------------------------------>
  
  btnRotary.loop(); // Checa estado do botão
  if (loop_counter > 10) { // Checa 10 vezes antes de decidir
    button_changes(); // Atualiza variáveis do botão
    loop_counter = 0;
    btn_click = false;
    btn_reset = false;
  }
  loop_counter++;

  rotaryInterrupt();

  //------------------------------------------------------>
  if((millis() - millisTarefa1) > update_time and mqttConnected){
     for (int module = 1; module <= numModules; module++) {
      // Request from server json config response of the specific panel
      String topic = PANEL_TOPIC.project + "/" + projectIds(module) + "/" + PANEL_TOPIC.configuration + "/" + BASE_ID;
      mqtt.publish(topic.c_str(), "Request");
      post_current_time(module);
     }  
     update_time = 10000;
     millisTarefa1 = millis();
  }

  /*  
  if (send_deselect_module and send_tries < SEND_TENTATIVES) {
    millisTarefa1 = millis();
    String return_msg = post_deselect();

    //if (return_msg[1] == '0') {
    send_deselect_module = false;
    send_selection_mode = true;
    send_tries = 0;

    selectedModuleLast = selectedModule;
    //}
    //else send_tries++;

    if (send_tries == SEND_TENTATIVES) selectedModuleLast = selectedModule;
  }

  if (send_selection_mode and send_tries < SEND_TENTATIVES) {
    millisTarefa1 = millis();
    String return_msg = post_selection_mode();

    //if (return_msg[1] == selectionMode + 0x30) {
    send_selection_mode = false;
    if (selectionMode == 2) send_selected_led = true;
    send_tries = 0;
    //}
    //else send_tries++;
  }

  if (send_selected_led and send_tries < SEND_TENTATIVES) {
    millisTarefa1 = millis();
    send_selected_quantity = false;
    String return_msg = post_selected_led();

    //if (return_msg[1] == selectedLed + 0x30) {
    send_selected_quantity = true;
    send_selected_led = false;
    send_tries = 0;
    //}
    //else send_tries++;
    delay(3);
  }

  if (send_selected_quantity and send_tries < SEND_TENTATIVES) {
    millisTarefa1 = millis(); // Send to slave current time and all quantities asap
    quantity_arr = get_quantity_array(selectedModule-1); // Get quantity array from current selected slave
    int current_quantity = quantity_arr.substring(2*selectedLed-2,2*selectedLed).toInt(); // Get two digit integer quantity of selected cell
    String return_msg = "abc";
    if (selectionMode == 2) {
      selectedQuantity = current_quantity;
      return_msg = post_quantity(current_quantity);

      //if (return_msg[2] == String(current_quantity)[0]) {
      send_selected_quantity = false;
      send_tries = 0;
      //}
      //else send_tries++;
    }
    if (selectionMode == 3) {
      return_msg = post_quantity(selectedQuantity);
      
      //if (return_msg[2] == String(selectedQuantity)[0]) {
      send_selected_quantity = false;
      send_tries = 0;
      //}
      //else send_tries++;
    }
    

  }

  if (send_server_update) {
    JsonObject root = jsonConfig.as<JsonObject>();
    millisTarefa1 = millis();
    String componente;
    
    for (JsonPair key_value : root) {
      componente = key_value.key().c_str();
      componente = componente.substring(1,componente.length()-1);

      if (selectedLed == componente.substring(0,2).toInt())
        jsonConfig[componente]["Quantidade"] = selectedQuantity;
    }

    //post_server_update();
    selectionMode = 2;
    send_selection_mode = true;
    quantity_arr = get_quantity_array(selectedModule-1);
    post_all_quantities(quantity_arr, selectedModule-1);
    
    send_server_update = false;
    update_screen_values();
  }
  */
  //------------------------------------------------------>

}

//#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
//ICACHE_RAM_ATTR
//#endif
void rotaryInterrupt() {
    int rotaryState;

    // Read rotary state (Counter clockwise) -2, -1, 0, 1, 2 (Clockwise)
    rotaryState = rotary.read();

    // Count up or down by using rotary speed
    if (rotaryState == 0) {
        // No change
        return;
    } else {
        if (selectionMode == 1) selectedModule += rotaryState;
        if (selectionMode == 2) selectedLed += rotaryState;
        if (selectionMode == 3) selectedQuantity += rotaryState;
    }

    // Limit count to a minimum and maximum value
    if (selectedQuantity < 0) selectedQuantity = 0;
    if (selectedModule > numModules) selectedModule = numModules;
    if (selectedModule < 1) selectedModule = 1;
    if (selectedLed > 9) selectedLed = 9;
    if (selectedLed < 1) selectedLed = 1;

    // Value changed
    if (selectionMode == 1 and selectedModuleLast != selectedModule) {
        send_deselect_module = true;
        send_tries = 0;

        update_screen_values();
    }
    if (selectionMode == 2 and selectedLedLast != selectedLed) {
        send_selected_led = true;
        send_tries = 0;
        
        selectedLedLast = selectedLed;
        
        update_screen_values();
    }
    if (selectionMode == 3 and selectedQttLast != selectedQuantity) {
        send_selected_quantity = true;
        send_tries = 0;
        
        selectedQttLast = selectedQuantity;
        
        update_screen_values();
    }
}
