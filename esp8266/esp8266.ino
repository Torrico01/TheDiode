/* Observações:
 *  1) Funcional para quantidades de componentes entre 0 e 99, caso a quantidade supere esse número, serão necessárias mudanças no código (quantity_arr, limit_arr)
 *  
 *  
 */  

#include <ESP8266HTTPClient.h>

#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Wire.h>
#include <SPI.h>

#include <PubSubClient.h>

#include "Button2.h";

#include <ErriezRotaryFullStep.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>

// Definições do Wifi
#ifndef STASSID
#define STASSID "RIBEIRAO 2G"
#define STAPSK  "r24e56g18"
#endif

// Definições de identificadores na comunicação I2C
#define id_Hora "H" // Horário atual
#define id_Slct "S" // Modo de seleção
#define id_Comp "C" // Compartimento de componentes
#define id_AQtd "A" // Todas as quantidades dos componentes
#define id_Qtd  "Q" // Quantidade de componentes no compartimento
#define id_Lim  "L" // Limite da quantidade de componentes

// Definições de pinagem
/* I2C */
//#define SDA_I2C_PIN D1
//#define SCL_I2C_PIN D2
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
 
// Configurações de rede e do servidor
const char* ssid     = STASSID;
const char* password = STAPSK;
IPAddress server(192,168,15,8);
const uint16_t port = 8080;
const char* serverName = "http://192.168.15.8:8080";
WiFiClient client;
HTTPClient http;

// Configurações dos módulos I2C
typedef struct
{
  String type;
  int addr;
}  moduleProprieties;
// Ordem: esquerda para direita, de cima para baixo
moduleProprieties modules[2] = { {"Capacitor eletrolitico 1", 0x22},   // 0x44 (Pic) >>> 0x22 // Placa 1 (mais antiga)
                                 {"Capacitor eletrolitico 2", 0x59} }; // 0xB2 (Pic) >>> 0x59 // Placa 2 (mais recente)
const int numModules = sizeof(modules) / sizeof(modules[0]);

// Variáveis específicas e genéricas
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

// Forward declaration
void rotaryInterrupt();

// Inicia variáveis de tempo
unsigned long millisTarefa1 = millis();
unsigned long update_time = 0; // Inicia em 0 para atualizar imediatamente, depois muda para o intervalo

// ================================================
// ======== Wifi and I2C Initializations ==========
// ================================================
void init_wifi(){
  // We start by connecting to a WiFi network
  wifi_connection_screen(0);
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

void establish_i2c() {
  JSONVar jsonConfig = get_server_update();
  int module;

  for (module = 0; module < numModules; module++) {
    String limit_arr = get_limit_array(jsonConfig, module);
    String return_msg = post_all_limits(limit_arr, module);
  }

}

// ================================================
// =========== Get updates from server ============
// ================================================
JSONVar get_server_update() {
  JSONVar jsonConfig = JSON.parse(httpGETRequest(serverName));
  if (JSON.typeof(jsonConfig) == "undefined") {
    //Serial.println("Parsing input failed!"); 
  }
  return jsonConfig;
}

String httpGETRequest(const char* serverName) {  
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
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

// ================================================
// ============ Post updates to server ============
// ================================================
void post_server_update(JSONVar jsonConfig) {
  httpPOSTRequest(serverName, JSON.stringify(jsonConfig));
}

void httpPOSTRequest(const char* serverName, String jsonString) {   
  // Your IP address with path or Domain name with URL path 
  http.begin(client, serverName);
  
  // Send HTTP POST request
  http.addHeader("Content-Type", "application/json");
  
  int httpResponseCode = http.POST(jsonString);

  http.end();
}

// =========================================================
// ====== Generic post and get slave update functions ======
// =========================================================
void post_slave_update_from_server(JSONVar jsonSlaveUpdate, String word1, String word2, String word3, char* msg_id, const int addr) {
  String msg;

  // Transforma a String da informação a ser enviada em Char
  msg = JSON.stringify(jsonSlaveUpdate[word1][word2][word3]);
  msg = msg_id + msg + msg_id;
  char buff[22];
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

String post_current_time(JSONVar jsonConfig, int module) { 
  // Envia hora para o escravo e retorna
  post_slave_update_from_server(jsonConfig, "Funcionalidades", "Horario", "Atual", id_Hora, modules[module].addr);
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
// =============== Other functions ================
// ================================================

String get_quantity_array(JSONVar jsonConfig, int module) {
  String quantidade_str = "000000000000000000";
  String categoria, componente, quantidade;
  int j, compartimento;

  for (j=0; j<9; j++) {
    componente = JSON.stringify(jsonConfig[modules[module].type].keys()[j]);
    componente = componente.substring(1,componente.length()-1);

    compartimento = componente.substring(0,2).toInt();
    quantidade = JSON.stringify(jsonConfig[modules[module].type][componente]["Quantidade"]);
    if (quantidade.toInt() < 10) {
      quantidade_str[2*compartimento-2] = '0';
      quantidade_str[2*compartimento-1] = quantidade[0];
    }
    else {
      quantidade_str[2*compartimento-2] = quantidade[0];
      quantidade_str[2*compartimento-1] = quantidade[1];
    }
  }

  return quantidade_str;
}

String get_limit_array(JSONVar jsonConfig, int module) {
  String limite_str = "000000000000000000";
  String categoria, componente, limite;
  int j, compartimento;

  for (j=0; j<9; j++) {
    componente = JSON.stringify(jsonConfig[modules[module].type].keys()[j]);
    componente = componente.substring(1,componente.length()-1);

    compartimento = componente.substring(0,2).toInt();
    limite = JSON.stringify(jsonConfig[modules[module].type][componente]["Limite"]);
    if (limite.toInt() < 10) {
      limite_str[2*compartimento-2] = '0';
      limite_str[2*compartimento-1] = limite[0];
    }
    else {
      limite_str[2*compartimento-2] = limite[0];
      limite_str[2*compartimento-1] = limite[1];
    }
  }

  return limite_str;
}

void generic_config_scan(JSONVar jsonConfig) {
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
  //while (!//Serial) ;

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    for(;;); // Don't proceed, loop forever
  }
  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
  // Clear the buffer
  display.clearDisplay();  

  // Initialize pin change interrupt on both rotary encoder pins
  //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN1), rotaryInterrupt, CHANGE);
  //attachInterrupt(digitalPinToInterrupt(ROTARY_PIN2), rotaryInterrupt, CHANGE);
  // Botão
  btnRotary.begin(ROTARY_BUTTON_PIN);
  btnRotary.setTapHandler(click);
  btnRotary.setLongClickHandler(resetPosition);
  
  // Connect to Wifi
  init_wifi();
  delay(500);

  // Connect to Server
  server_connection_screen(0);// Conectando no Servidor
  String payload = httpGETRequest(serverName);
  if (payload == "{}") {
    server_connection_screen(2); // Erro de conexão
    for(;;); // Trava a execução
  }
  else server_connection_screen(1); // Server conectado
  delay(500);

  // Update screen
  update_screen_values();

  // Módulos
  Wire.begin(SDA, SCL); /* join i2c bus with SDA=D1 and SCL=D2 of NodeMCU */
  //Wire.setClock(100000);

  // Establish I2C connection with every slave and send quantities limits
  establish_i2c();
  
  delay(500);
}

void loop() {
  static String quantity_arr;
  
  btnRotary.loop(); // Checa estado do botão
  if (loop_counter > 10) { // Checa 10 vezes antes de decidir
    button_changes(); // Atualiza variáveis do botão
    loop_counter = 0;
    btn_click = false;
    btn_reset = false;
  }
  loop_counter++;

  rotaryInterrupt();

  if((millis() - millisTarefa1) > update_time){
     JSONVar jsonConfig = get_server_update();
     String return_msg;
     int module;    
     for (module = 0; module < numModules; module++) {
      quantity_arr = get_quantity_array(jsonConfig, module);
      return_msg = post_all_quantities(quantity_arr, module);
      return_msg = post_current_time(jsonConfig, module);
     }  
     update_time = 6000;
     millisTarefa1 = millis();
  }


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
    JSONVar jsonConfig = get_server_update();
    quantity_arr = get_quantity_array(jsonConfig, selectedModule-1); // Get quantity array from current selected slave
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
    millisTarefa1 = millis();
    String componente;
    int j;
    JSONVar jsonConfig = get_server_update();
    
    for (j = 0; j < 9; j++) {
      componente = JSON.stringify(jsonConfig[modules[selectedModule-1].type].keys()[j]);
      componente = componente.substring(1,componente.length()-1);

      if (selectedLed == componente.substring(0,2).toInt())
        jsonConfig[modules[selectedModule-1].type][componente]["Quantidade"] = selectedQuantity;
    }

    post_server_update(jsonConfig);
    selectionMode = 2;
    send_selection_mode = true;
    quantity_arr = get_quantity_array(jsonConfig, selectedModule-1);
    String return_msg = post_all_quantities(quantity_arr, selectedModule-1);
    
    send_server_update = false;
    update_screen_values();
  }

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
