#include <ESP8266HTTPClient.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <Dictionary.h>
#include <FastLED.h>
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
#define SELF_NAME "RGB Frame 1"
#define SELF_ID "5"
// Identifica os projetos
Dictionary &projectIds = *(new Dictionary(3));
// -------- Definições do MQTT --------
#define MQTT_BROKER "192.168.15.16"
#define MQTT_PORT 1883
class CommunicationTopic {
  public:
    String communication = "Communication";
    String connection = "Connection";
}
COMMUNICATION_TOPIC = CommunicationTopic();
class RGBFrameTopic {
  public: 
    String project     = "RGBFrame";
    String outRGBStrip = "Outputs/RGBStrip";
}
RGB_FRAME_TOPIC = RGBFrameTopic();
String subscriptionList[2] = {RGB_FRAME_TOPIC.project + "/" + SELF_NAME + "/" + RGB_FRAME_TOPIC.outRGBStrip + "/#",
                              COMMUNICATION_TOPIC.communication + "/" + COMMUNICATION_TOPIC.connection + "/#"}; // No project name cause requester identfies the connection
const int numSubscriptions = len(subscriptionList);
// ========================================================================

// Pinagem e configuração
#define LED_PIN     D1
#define NUM_LEDS    134 // 134
#define MAX_PATTERNS 16
#define BRIGHTNESS  255
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
#define UPDATES_PER_SECOND 100
#define LED_PERCENTAGE 0.05 // 0.05
CRGB leds[NUM_LEDS];

// Configurações de rede, do servidor e do MQTT
const char* ssid       = STASSID;
const char* password   = STAPSK;
const char* serverName = SERVER_NAME;
const char* mqttBroker = MQTT_BROKER;
const int   mqttPort   = MQTT_PORT;
WiFiClient wifi;
HTTPClient http;
PubSubClient mqtt(wifi);
String request_topic;
String initial_topic;

// RGB strip variáveis
CRGBPalette256 currentFadePalette; // static initial fading palette
CRGBPalette256 fadingPalette; // dinamic fading palette
CRGBPalette256 toFadePalette; // static fading into palette
CRGBPalette16 initialColorArray;
CRGBPalette16 changingColorArray;
CRGBPalette16 targetColorArray;
CRGB currentColor;
TBlendType currentBlending;

// Pattern -> defined by a initial color, a target color and a time to fade
// Sequence -> defined by total patterns across all the leds (in the same time frame)
// led_1_Array -> array with first led position of all sequences of current set
// led_2_Array -> array with last led position of all sequences of current set
// led_1_m_Array -> array with led_1_Array value mapped to 0-256 (used during color distribution)
// led_2_m_Array -> array with led_2_Array value mapped to 0-256 (used during color distribution)
// currentLed_Array -> array with led position of the "changing color" led, of all sequences of current set
// currentLed_m_Array -> array with currentLed_Array value mapped to 0-256 (used during color distribution)
// redCheck_Array -> array with booleans indicating if the red color of the current led reached the minimal value for changing the current led, of all sequences of current set
// greenCheck_Array -> array with booleans indicating if the green color of the current led reached the minimal value for changing the current led, of all sequences of current set
// blueCheck_Array -> array with booleans indicating if the blue color of the current led reached the minimal value for changing the current led, of all sequences of current set
volatile int led_1_Array[MAX_PATTERNS + 1]; // num of sequences in each set + 1
volatile int led_2_Array[MAX_PATTERNS + 1]; //
volatile int led_1_m_Array[MAX_PATTERNS + 1]; //
volatile int led_2_m_Array[MAX_PATTERNS + 1]; //
volatile int currentLed_Array[MAX_PATTERNS + 1]; //
volatile int currentLed_m_Array[MAX_PATTERNS + 1]; //
volatile int patternTime_Array[MAX_PATTERNS + 1]; //
bool redCheck_Array[MAX_PATTERNS + 1]; //
bool greenCheck_Array[MAX_PATTERNS + 1]; //
bool blueCheck_Array[MAX_PATTERNS + 1]; //
bool patternCheck_Array[MAX_PATTERNS + 1]; // num of sequences in each set + 1
bool patternChanged = false; // To prevent changing sequence when getting a new sequence
bool sameSequence = false; // Check whether the receiving sequence is exactly the same as the previous received sequence
bool patternsChecked = false; // Indicates when all patterns of the current sequence have ended their fading
int sequencesInSet;
unsigned long currentTime=0;
unsigned long previousTime=0;

// Json variável
StaticJsonDocument<2048> JsonConfig;

// Pallet changer variables
CRGB initialColor, targetColor, changingColor, previousTargetColor;
float ledPercentage = LED_PERCENTAGE;
int rAdj = 0, gAdj = 0, bAdj = 0;
String msg_mqtt = "";
volatile int led_1 = 0, led_2 = 0, led_1_m = 0, led_2_m = 0;
volatile int currentLed = 0, currentLed_m = 0;
int seq = 1;
int i_m = 0;

// ================================================
// ============ Wifi Initializations ==============
// ================================================
void init_wifi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  byte wifi_tries = 0;

  while (WiFi.status() != WL_CONNECTED and wifi_tries < 42) {
    delay(500);
    wifi_tries++;
  }

  if (WiFi.status() != WL_CONNECTED) for (;;); // Trava a execução
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

unsigned long intStrToInt(String str) {
   char buffer [256];
   unsigned long ul;
   str.toCharArray(buffer, str.length() + 1);
   ul = strtoul(buffer, 0, 10);
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
    if (index == -1) { // No / found
      topicSplitString[StringCount++] = topicStringToSplit;
      lastTopicSplitString = topicStringToSplit;
      break;
    }
    else {
      topicSplitString[StringCount++] = topicStringToSplit.substring(0, index);
      topicStringToSplit = topicStringToSplit.substring(index+1);
    }
  }

  // If message received was not published by itself
  if (lastTopicSplitString != SELF_ID) {
    // RGB Frame
    if (topicString.indexOf(RGB_FRAME_TOPIC.project) >= 0) {
      // Output / RGB Strip
      if (topicString.indexOf(RGB_FRAME_TOPIC.outRGBStrip) >= 0) {
        // RGBFrame/RGBFrame1/Outputs/RGBStrip/id
        String requested_project_name = topicSplitString[1];
        String requester_project_number = topicSplitString[4];
        String requested_id_str = projectIds[requested_project_name];
        int requested_id = intStrToInt(requested_id_str);
        
        deserializeJson(JsonConfig, payload, length); // Modifica a string original
        changeSequencePattern(); // change toFade palette with setToFadePaletteForward or setToFadePaletteBackward
      }
    }
  }
}

void mqtt_reconnect() {
  String clientId = SELF_NAME; 
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

//
////
//

void setup() {
    delay(3000); // power-up safety delay
    Serial.begin(9600);
    
    // ============= RGB Strip Setup =============
    // Reset all leds
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
    for(int i = 0; i < NUM_LEDS; i++) leds[i] = CRGB::Black;
    FastLED.show();
    delay(1000);

    // Start leds
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
    FastLED.setBrightness( BRIGHTNESS );
    delay(1000);
    
    //currentPalette = RainbowColors_p;
    fill_solid(currentFadePalette, 256, CRGB::Black);
    fill_solid(fadingPalette, 256, CRGB::Black);
    fill_solid(toFadePalette, 256, CRGB::Black);
    fill_solid(changingColorArray, MAX_PATTERNS, CRGB::Black);
    for (int seqIt = 0; seqIt < MAX_PATTERNS; seqIt++) { // ITERATE OVER SEQUENCES -> TO CHANGE LATER
      redCheck_Array[seqIt] = false;
      greenCheck_Array[seqIt] = false;
      blueCheck_Array[seqIt] = false;
      patternCheck_Array[seqIt] = false;
    }
    currentBlending = LINEARBLEND; // NOBLEND  LINEARBLEND

    // ============= Initializations Setup =============
    // Wifi
    init_wifi();
    delay(250);

    // MQTT
    mqtt.setServer(mqttBroker, mqttPort);
    mqtt.setCallback(mqtt_callback);
    delay(250);
    // Defined topics
    initial_topic = COMMUNICATION_TOPIC.communication + "/" + COMMUNICATION_TOPIC.connection + "/" + SELF_ID;
    request_topic = RGB_FRAME_TOPIC.project + "/" + SELF_NAME + "/" + RGB_FRAME_TOPIC.outRGBStrip + "/" + SELF_ID;

    // Projects ids dictionary
    projectIds(SELF_NAME, SELF_ID);

    currentColor = initialColorArray[0]; // Color to start the fade effect
    previousTargetColor = CRGB::Black;
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("Reset");
}


void loop() {
  bool mqttConnected = mqtt.connected();
  
  if (!mqttConnected) {
    mqtt_reconnect();//------------------------------------------------------>
    mqtt.publish(initial_topic.c_str(), "ON");
    mqtt.publish(request_topic.c_str(), "Request");
  }
  mqtt.loop();//------------------------------------------------------>

  static uint8_t startIndex = 0;
  startIndex = startIndex + 0; /* motion speed */ // Ciclico entre 0 e 255
  
  FillLEDsFromPaletteColors(startIndex);
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);

  currentTime=millis();
  if(sameSequence && (currentTime-previousTime) >= 5000){
    previousTime=currentTime;
    mqtt.publish(request_topic.c_str(), "Request");
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex ) {
    for(int i = 0; i < NUM_LEDS; i++) { // Iterate through all leds
        i_m = map(i, 0, NUM_LEDS-1, 0, 255);

        // Change values of led_1, led_2, currentLed, initialColor, targetColor and changingColor
        // Depending on the sequence/pattern of sequenceSet
        for (int seqIt = 1; seqIt <= sequencesInSet; seqIt++) { // Iterate over sequences
          if (i == led_1_Array[seqIt] || i == led_2_Array[seqIt]) {
            if (i == led_1_Array[seqIt]) toFadePalette[i_m] = initialColorArray[seqIt];
            if (i == led_2_Array[seqIt]) toFadePalette[i_m] = targetColorArray[seqIt];
            led_1 = led_1_Array[seqIt];
            led_2 = led_2_Array[seqIt];
            led_1_m = led_1_m_Array[seqIt];
            led_2_m = led_2_m_Array[seqIt];
            currentLed = currentLed_Array[seqIt];
            currentLed_m = currentLed_m_Array[seqIt];

            initialColor = initialColorArray[seqIt];
            targetColor = targetColorArray[seqIt];
            changingColor = changingColorArray[seqIt];

            seq = seqIt;
            break;
          }
        }
        
        if ((led_2 > led_1 && i >= led_1 && i <= led_2) || (led_2 < led_1 && i <= led_1 && i >= led_2)) { // If  between initial and final leds, apply changes
          if ((i < currentLed && led_2 > led_1) || (i > currentLed && led_2 < led_1)) { // If before current led, fade previous leds
            nblendU8TowardU8(fadingPalette[i_m].r, toFadePalette[i_m].r, patternTime_Array[seq]);
            nblendU8TowardU8(fadingPalette[i_m].g, toFadePalette[i_m].g, patternTime_Array[seq]);
            nblendU8TowardU8(fadingPalette[i_m].b, toFadePalette[i_m].b, patternTime_Array[seq]);
  
            currentColor.r = fadingPalette[i_m].r;
            currentColor.g = fadingPalette[i_m].g;
            currentColor.b = fadingPalette[i_m].b;
          }
          else if ((i > currentLed && led_2 > led_1) || (i < currentLed && led_2 < led_1)) { // If after current led, maintain next leds
            if (led_2 == 0 && i == led_2) {
              currentColor.r = previousTargetColor.r;
              currentColor.g = previousTargetColor.g;
              currentColor.b = previousTargetColor.b;
            }
            else {
              currentColor.r = currentFadePalette[i_m].r;
              currentColor.g = currentFadePalette[i_m].g;
              currentColor.b = currentFadePalette[i_m].b;
            }
          }
          else if (i == currentLed) {
            nblendU8TowardU8(changingColor.r, toFadePalette[currentLed_m].r, patternTime_Array[seq]);
            nblendU8TowardU8(changingColor.g, toFadePalette[currentLed_m].g, patternTime_Array[seq]);
            nblendU8TowardU8(changingColor.b, toFadePalette[currentLed_m].b, patternTime_Array[seq]);
            changingColorArray[seq].r = changingColor.r; // Armazena todas as cores atualmente variando dos patterns da sequencia atual
            changingColorArray[seq].g = changingColor.g;
            changingColorArray[seq].b = changingColor.b;
            currentColor.r = changingColor.r;
            currentColor.g = changingColor.g;
            currentColor.b = changingColor.b;
          }
        }
        else { // If not between initial and final leds, leave without changes
            currentColor.r = currentFadePalette[i_m].r; // Armazena os valores de todos os leds variando atualmente
            currentColor.g = currentFadePalette[i_m].g;
            currentColor.b = currentFadePalette[i_m].b;
        }

        rAdj = (toFadePalette[currentLed_m].r - currentFadePalette[currentLed_m].r)*ledPercentage;
        gAdj = (toFadePalette[currentLed_m].g - currentFadePalette[currentLed_m].g)*ledPercentage;
        bAdj = (toFadePalette[currentLed_m].b - currentFadePalette[currentLed_m].b)*ledPercentage;
        
        if ((rAdj > 0 && changingColor.r >= currentFadePalette[currentLed_m].r + rAdj) ||
            (rAdj < 0 && changingColor.r <= currentFadePalette[currentLed_m].r + rAdj) ||
             rAdj == 0) {
          redCheck_Array[seq] = true;
        }
        if ((gAdj > 0 && changingColor.g >= currentFadePalette[currentLed_m].g + gAdj) ||
            (gAdj < 0 && changingColor.g <= currentFadePalette[currentLed_m].g + gAdj) ||
             gAdj == 0) {
          greenCheck_Array[seq] = true;
        }
        if ((bAdj > 0 && changingColor.b >= currentFadePalette[currentLed_m].b + bAdj) ||
            (bAdj < 0 && changingColor.b <= currentFadePalette[currentLed_m].b + bAdj) ||
             bAdj == 0) {
          blueCheck_Array[seq] = true;
        }

        if (redCheck_Array[seq] && greenCheck_Array[seq] && blueCheck_Array[seq] && led_2 > led_1 && currentLed != led_2) {
          redCheck_Array[seq] = false;
          greenCheck_Array[seq] = false;
          blueCheck_Array[seq] = false;
          fadingPalette[currentLed_m] = changingColor;

          currentLed++;
          if (currentLed > NUM_LEDS-1) currentLed = NUM_LEDS-1;
          currentLed_m = map(currentLed, 0, NUM_LEDS-1, 0, 255);
          currentLed_Array[seq] = currentLed;
          currentLed_m_Array[seq] = currentLed_m;

          changingColor = currentFadePalette[currentLed_m];
          changingColorArray[seq] = changingColor;
          patternChanged = false;
        }
        else if (redCheck_Array[seq] && greenCheck_Array[seq] && blueCheck_Array[seq] && led_2 < led_1 && currentLed != led_2) {
          redCheck_Array[seq] = false;
          greenCheck_Array[seq] = false;
          blueCheck_Array[seq] = false;
          fadingPalette[currentLed_m] = changingColor;

          currentLed--;
          if (currentLed < 0) currentLed = 0;
          currentLed_m = map(currentLed, 0, NUM_LEDS-1, 0, 255);
          currentLed_Array[seq] = currentLed;
          currentLed_m_Array[seq] = currentLed_m;

          changingColor = currentFadePalette[currentLed_m];
          if(currentLed == 0) changingColor = previousTargetColor;
          changingColorArray[seq] = changingColor;
          patternChanged = false;
        }
        else if (currentLed == led_2 && !patternChanged && !sameSequence && fadingPalette[led_1_m] == toFadePalette[led_1_m] && changingColor == targetColor) {
          patternCheck_Array[seq] = true;
          patternsChecked = patternCheck_Array[seq];
          if (led_2 == 0) {
            previousTargetColor = targetColor;
            toFadePalette[led_2_m] = targetColor;
          }
        }

        for (int seqIt = 1; seqIt <= sequencesInSet; seqIt++) {
          patternsChecked = patternsChecked & patternCheck_Array[seqIt];
        }
        if (patternsChecked) {
          currentFadePalette = toFadePalette;
          for (int seqIt = 1; seqIt <= sequencesInSet; seqIt++) {
            redCheck_Array[seqIt] = false;
            greenCheck_Array[seqIt] = false;
            blueCheck_Array[seqIt] = false;
            patternCheck_Array[seqIt] = false;
            currentFadePalette[led_2_m_Array[seqIt]] = targetColorArray[seqIt];
          }
          mqtt.publish(request_topic.c_str(), "Request");
          patternChanged = true;
          patternsChecked = false;
          delay(150);
        }

        leds[i] = currentColor;
        colorIndex += 255/NUM_LEDS; // Ciclico entre 0 e 255
    }
}


void changeSequencePattern() {
  // real range 1-134, test range 1-30 // Starts with first led = 0
  sequencesInSet = JsonConfig["Total"];
  fadingPalette = currentFadePalette;

  for (int seq_cnt = 1; seq_cnt <= sequencesInSet; seq_cnt++) {
    String str_seq = String(seq_cnt);
    int new_led1_m;

    sameSequence = true;
    if (!(String(led_1_Array[seq_cnt]) == JsonConfig[str_seq]["L1"] &&
        String(led_2_Array[seq_cnt]) == JsonConfig[str_seq]["L2"] &&
        String(patternTime_Array[seq_cnt]) == JsonConfig[str_seq]["T"] &&
        initialColorArray[seq_cnt].r == JsonConfig[str_seq]["ICr"] &&
        initialColorArray[seq_cnt].g == JsonConfig[str_seq]["ICg"] &&
        initialColorArray[seq_cnt].b == JsonConfig[str_seq]["ICb"] &&
        targetColorArray[seq_cnt].r == JsonConfig[str_seq]["TCr"] &&
        targetColorArray[seq_cnt].g == JsonConfig[str_seq]["TCg"] &&
        targetColorArray[seq_cnt].b == JsonConfig[str_seq]["TCb"])) { // Check whether the receiving sequence is not exactly the same as the previous received sequence
      sameSequence = false;

      new_led1_m = map(JsonConfig[str_seq]["L1"], 0, NUM_LEDS-1, 0, 255);
      changingColorArray[seq_cnt] = toFadePalette[new_led1_m]; // Update changing color leds to initial led position of new pattern

      led_1_Array[seq_cnt] = JsonConfig[str_seq]["L1"];
      led_2_Array[seq_cnt] = JsonConfig[str_seq]["L2"];
      patternTime_Array[seq_cnt] = JsonConfig[str_seq]["T"];
      initialColorArray[seq_cnt].r = JsonConfig[str_seq]["ICr"];
      initialColorArray[seq_cnt].g = JsonConfig[str_seq]["ICg"];
      initialColorArray[seq_cnt].b = JsonConfig[str_seq]["ICb"];
      targetColorArray[seq_cnt].r = JsonConfig[str_seq]["TCr"];
      targetColorArray[seq_cnt].g = JsonConfig[str_seq]["TCg"];
      targetColorArray[seq_cnt].b = JsonConfig[str_seq]["TCb"];
  
      led_1_m_Array[seq_cnt] = map(led_1_Array[seq_cnt], 0, NUM_LEDS-1, 0, 255);
      led_2_m_Array[seq_cnt] = map(led_2_Array[seq_cnt], 0, NUM_LEDS-1, 0, 255);
      currentLed_Array[seq_cnt] = JsonConfig[str_seq]["L1"];
      currentLed_m_Array[seq_cnt] = map(currentLed_Array[seq_cnt], 0, NUM_LEDS-1, 0, 255);
  
      if (led_2_Array[seq_cnt] > led_1_Array[seq_cnt]) setToFadePaletteForward(led_1_m_Array[seq_cnt], led_2_m_Array[seq_cnt], initialColorArray[seq_cnt], targetColorArray[seq_cnt]);
      else setToFadePaletteBackward(led_1_m_Array[seq_cnt], led_2_m_Array[seq_cnt], initialColorArray[seq_cnt], targetColorArray[seq_cnt]);
    }
  }

  for (int seq_cnt = sequencesInSet + 1; seq_cnt <= MAX_PATTERNS; seq_cnt++) { // Set as zero all other patterns in sequence array
      led_1_Array[seq_cnt] = 0;
      led_2_Array[seq_cnt] = 0;
      led_1_m_Array[seq_cnt] = 0;
      led_2_m_Array[seq_cnt] = 0;
      patternTime_Array[seq_cnt] = 0;
      initialColorArray[seq_cnt].r = 0;
      initialColorArray[seq_cnt].g = 0;
      initialColorArray[seq_cnt].b = 0;
      targetColorArray[seq_cnt].r = 0;
      targetColorArray[seq_cnt].g = 0;
      targetColorArray[seq_cnt].b = 0;
      currentLed_Array[seq_cnt] = 0;
      currentLed_m_Array[seq_cnt] = 0;
  }
}

void setToFadePaletteBackward(int led_1_m, int led_2_m, CRGB initialColor, CRGB targetColor) {
  float redDiff, greenDiff, blueDiff;
  redDiff = initialColor.r - targetColor.r;
  greenDiff = initialColor.g - targetColor.g;
  blueDiff = initialColor.b - targetColor.b;
  int redColor, greenColor, blueColor;
  int nLeds = abs(led_2_m - led_1_m);

  for(int j = 0; j < nLeds; j++) {  
    redColor = targetColor.r + (j * redDiff)/nLeds;
    greenColor = targetColor.g + (j * greenDiff)/nLeds;
    blueColor = targetColor.b + (j * blueDiff)/nLeds;
    toFadePalette[led_2_m+j] = CRGB(redColor, greenColor, blueColor);
  }
}

void setToFadePaletteForward(int led_1_m, int led_2_m, CRGB initialColor, CRGB targetColor) {
  float redDiff, greenDiff, blueDiff;
  redDiff = targetColor.r - initialColor.r;
  greenDiff = targetColor.g - initialColor.g;
  blueDiff = targetColor.b - initialColor.b;
  int redColor, greenColor, blueColor;
  int nLeds = abs(led_2_m - led_1_m);

  for(int j = 0; j < nLeds; j++) {  
    redColor = initialColor.r + (j * redDiff)/nLeds;
    greenColor = initialColor.g + (j * greenDiff)/nLeds;
    blueColor = initialColor.b + (j * blueDiff)/nLeds;
    toFadePalette[led_1_m+j] = CRGB(redColor, greenColor, blueColor);
  }
}

void nblendU8TowardU8(uint8_t &current, const uint8_t target, int t) {
  if (current == target) return;

  if (current < target) {
    uint8_t delta = target - current;
    delta = scale8_video(delta, t); // 4, 16
    current += delta;
  }
  else {
    uint8_t delta = current - target;
    delta = scale8_video(delta, t); // 4, 16
    current -= delta;
  }
}
