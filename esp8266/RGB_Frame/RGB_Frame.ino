#include <FastLED.h>
#define LED_PIN     D1
#define NUM_LEDS    30
#define BRIGHTNESS  50
#define LED_TYPE    WS2812
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define UPDATES_PER_SECOND 100
#define LED_1_1 0 // starts in 0
#define LED_2_1 6
#define LED_1_2 13 // starts in 0
#define LED_2_2 7
#define LED_1_3 14 // starts in 0
#define LED_2_3 20
#define LED_1_4 27 // starts in 0
#define LED_2_4 21
#define LED_PERCENTAGE 0.01 // 0.05

CRGBPalette256 currentFadePalette;
CRGBPalette256 fadingPalette;
CRGBPalette256 toFadePalette;
CRGBPalette16 initialColorArray;
CRGBPalette16 changingColorArray;
CRGBPalette16 targetColorArray;
CRGB currentColor; // To set current led
TBlendType currentBlending;

int led_1_Array[5]; // num of sequences in each set + 1
int led_2_Array[5]; // num of sequences in each set + 1
int led_1_m_Array[5]; // num of sequences in each set + 1
int led_2_m_Array[5]; // num of sequences in each set + 1
int currentLed_Array[5]; // num of sequences in each set + 1
int currentLed_m_Array[5]; // num of sequences in each set + 1

bool redCheck_Array[5]; // num of sequences in each set + 1
bool greenCheck_Array[5]; // num of sequences in each set + 1
bool blueCheck_Array[5]; // num of sequences in each set + 1

// Sequence -> defined by a initial color, a target color and a time to fade
// Sequence set -> defined by total sequences across all the leds (in the same time frame)
// numOfSequencesInSet -> array of number of sequences in each sequence set
int numOfSequencesInSet[6]; // num of sets
int numOfSequencesInSetLen;
int sequenceSet = 0;

bool patternChanged = false; // Currently only works with same size sequences ending at the same time

void setup() {
    delay(3000); // power-up safety delay
    Serial.begin(9600);
    
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
    fill_solid(changingColorArray, 16, CRGB::Black);
    for (int seqIt = 0; seqIt < 5; seqIt++) { // ITERATE OVER SEQUENCES -> TO CHANGE LATER
      redCheck_Array[seqIt] = false;
      greenCheck_Array[seqIt] = false;
      blueCheck_Array[seqIt] = false;
    }
    currentBlending = LINEARBLEND; // NOBLEND  LINEARBLEND

    // Populate array of numOfSequencesInSet
    numOfSequencesInSet[0] = 5;
    numOfSequencesInSet[1] = 5;
    numOfSequencesInSet[2] = 5;
    numOfSequencesInSet[3] = 5;
    numOfSequencesInSet[4] = 5;
    numOfSequencesInSet[5] = 5;
    numOfSequencesInSetLen = 6;
     // ------- Time Slot Update -------
    changeSequencePattern(); // change toFade palette with setToFadePaletteForward or setToFadePaletteBackward
    // -------------------------------

    currentColor = initialColorArray[0]; // Color to start the fade effect
    Serial.println("Reset");
}


void loop() {
  static uint8_t startIndex = 0;
  startIndex = startIndex + 0; /* motion speed */ // Ciclico entre 0 e 255
  
  FillLEDsFromPaletteColors(startIndex);
  
  FastLED.show();
  FastLED.delay(1000 / UPDATES_PER_SECOND);
}

void FillLEDsFromPaletteColors( uint8_t colorIndex )
{
    int i_m;
    CRGB previousInitColor;
    CRGB fadeColorAdjusted;
    int rAdj, gAdj, bAdj;
    float ledPercentage = LED_PERCENTAGE;

    int led_1, led_2, led_1_m, led_2_m;
    int currentLed, currentLed_m;
    CRGB initialColor, targetColor, changingColor;
    int seq = 0;

    for(int i = 0; i < NUM_LEDS; i++) {
        i_m = map(i, 0, NUM_LEDS-1, 0, 255);

        // Change values of led_1, led_2, currentLed, initialColor, targetColor and changingColor
        // Depending on the sequence of sequenceSet
        for (int seqIt = 1; seqIt < 5; seqIt++) { // ITERATE OVER SEQUENCES -> TO CHANGE LATER
          if (seqIt < numOfSequencesInSet[sequenceSet]) {
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
          else break;
        }
        
        if ((led_2 > led_1 && i >= led_1 && i <= led_2) || (led_2 < led_1 && i <= led_1 && i >= led_2)) {
          if ((i < currentLed && led_2 > led_1) || (i > currentLed && led_2 < led_1)) {        
            nblendU8TowardU8(fadingPalette[i_m].r, toFadePalette[i_m].r);
            nblendU8TowardU8(fadingPalette[i_m].g, toFadePalette[i_m].g);
            nblendU8TowardU8(fadingPalette[i_m].b, toFadePalette[i_m].b);
  
            currentColor.r = fadingPalette[i_m].r;
            currentColor.g = fadingPalette[i_m].g;
            currentColor.b = fadingPalette[i_m].b;
          }
          else if ((i > currentLed && led_2 > led_1) || (i < currentLed && led_2 < led_1)) {
            currentColor.r = currentFadePalette[i_m].r;
            currentColor.g = currentFadePalette[i_m].g;
            currentColor.b = currentFadePalette[i_m].b;
          }
          else if (i == currentLed) {
            if (currentLed == led_1) {
              nblendU8TowardU8(changingColor.r, initialColor.r);
              nblendU8TowardU8(changingColor.g, initialColor.g);
              nblendU8TowardU8(changingColor.b, initialColor.b);
            }
            else if (currentLed == led_2) {
              nblendU8TowardU8(changingColor.r, targetColor.r);
              nblendU8TowardU8(changingColor.g, targetColor.g);
              nblendU8TowardU8(changingColor.b, targetColor.b);
            }
            else {
              nblendU8TowardU8(changingColor.r, toFadePalette[currentLed_m].r);
              nblendU8TowardU8(changingColor.g, toFadePalette[currentLed_m].g);
              nblendU8TowardU8(changingColor.b, toFadePalette[currentLed_m].b);
            }
            changingColorArray[seq].r = changingColor.r;
            changingColorArray[seq].g = changingColor.g;
            changingColorArray[seq].b = changingColor.b;
            currentColor.r = changingColor.r;
            currentColor.g = changingColor.g;
            currentColor.b = changingColor.b;
          }
        }
        else {
            currentColor.r = currentFadePalette[i_m].r;
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
          Serial.println("Led ++");
          redCheck_Array[seq] = false;
          greenCheck_Array[seq] = false;
          blueCheck_Array[seq] = false;
          fadingPalette[currentLed_m] = changingColor;

          currentLed++;
          currentLed_m = map(currentLed, 0, NUM_LEDS-1, 0, 255);
          currentLed_Array[seq] = currentLed;
          currentLed_m_Array[seq] = currentLed_m;

          changingColor = currentFadePalette[currentLed_m];
          changingColorArray[seq] = changingColor;
          patternChanged = false;
        }
        else if (redCheck_Array[seq] && greenCheck_Array[seq] && blueCheck_Array[seq] && led_2 < led_1 && currentLed != led_2) {
          Serial.println("Led --");
          redCheck_Array[seq] = false;
          greenCheck_Array[seq] = false;
          blueCheck_Array[seq] = false;
          fadingPalette[currentLed_m] = changingColor;

          currentLed--;
          currentLed_m = map(currentLed, 0, NUM_LEDS-1, 0, 255);
          currentLed_Array[seq] = currentLed;
          currentLed_m_Array[seq] = currentLed_m;

          changingColor = currentFadePalette[currentLed_m];
          changingColorArray[seq] = changingColor; // problem
          patternChanged = false;
        }
        else if (changingColor == targetColor && currentLed == led_2 && !patternChanged) {
          Serial.println("Changing palette pattern");
          redCheck_Array[seq] = false;
          greenCheck_Array[seq] = false;
          blueCheck_Array[seq] = false;

          currentFadePalette = toFadePalette; // No futuro: mudar para atualizar o currentFadePalette apenas quando a última sequencia tiver sido atulizada (que pode demorar mais que outras sequencias)
          changeSequencePattern();
          patternChanged = true;
        }

        leds[i] = currentColor;
        colorIndex += 255/NUM_LEDS; // Ciclico entre 0 e 255
    }
}


void changeSequencePattern() {
  // real range 1-110, test range 1-30 // Starts with first led = 0
  for (int seq = 1; seq < 5; seq++) { // ITERATE OVER SEQUENCES -> TO CHANGE LATER
    if (seq < numOfSequencesInSet[sequenceSet]) {
      if (sequenceSet == 0) {
        if (seq == 1) {
          // *** Substituir para algo do tipo JsonConfig[sequenceSet][seq][Led_1]
          // *** Tirar os ifs anteriores
          led_1_Array[seq] = LED_1_1; // GET FROM JSON 
          led_2_Array[seq] = LED_2_1; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
        if (seq == 2) {
          led_1_Array[seq] = LED_1_2; // GET FROM JSON
          led_2_Array[seq] = LED_2_2; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
        if (seq == 3) {
          led_1_Array[seq] = LED_1_3; // GET FROM JSON
          led_2_Array[seq] = LED_2_3; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
        if (seq == 4) {
          led_1_Array[seq] = LED_1_4; // GET FROM JSON
          led_2_Array[seq] = LED_2_4; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
      }
      if (sequenceSet == 1) {
        if (seq == 1) {
          led_1_Array[seq] = LED_2_1; // GET FROM JSON
          led_2_Array[seq] = LED_1_1; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
        if (seq == 2) {
          led_1_Array[seq] = LED_2_2; // GET FROM JSON
          led_2_Array[seq] = LED_1_2; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
        if (seq == 3) {
          led_1_Array[seq] = LED_2_3; // GET FROM JSON
          led_2_Array[seq] = LED_1_3; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
        if (seq == 4) {
          led_1_Array[seq] = LED_2_4; // GET FROM JSON
          led_2_Array[seq] = LED_1_4; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
      }
      if (sequenceSet == 2) {
        if (seq == 1) {
          led_1_Array[seq] = LED_1_1; // GET FROM JSON
          led_2_Array[seq] = LED_2_1; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
        if (seq == 2) {
          led_1_Array[seq] = LED_1_2; // GET FROM JSON
          led_2_Array[seq] = LED_2_2; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
        if (seq == 3) {
          led_1_Array[seq] = LED_1_3; // GET FROM JSON
          led_2_Array[seq] = LED_2_3; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
        if (seq == 4) {
          led_1_Array[seq] = LED_1_4; // GET FROM JSON
          led_2_Array[seq] = LED_2_4; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
      }
      if (sequenceSet == 3) {
        if (seq == 1) {
          led_1_Array[seq] = LED_2_1; // GET FROM JSON 
          led_2_Array[seq] = LED_1_1; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
        if (seq == 2) {
          led_1_Array[seq] = LED_2_2; // GET FROM JSON
          led_2_Array[seq] = LED_1_2; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
        if (seq == 3) {
          led_1_Array[seq] = LED_2_3; // GET FROM JSON
          led_2_Array[seq] = LED_1_3; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
        if (seq == 4) {
          led_1_Array[seq] = LED_2_4; // GET FROM JSON
          led_2_Array[seq] = LED_1_4; // GET FROM JSON
          initialColorArray[seq] = CRGB::Red; // GET FROM JSON
          targetColorArray[seq] = CRGB::Blue; // GET FROM JSON
        }
      }
      if (sequenceSet == 4) {
        if (seq == 1) {
          led_1_Array[seq] = LED_1_1; // GET FROM JSON
          led_2_Array[seq] = LED_2_1; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
        if (seq == 2) {
          led_1_Array[seq] = LED_1_2; // GET FROM JSON
          led_2_Array[seq] = LED_2_2; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
        if (seq == 3) {
          led_1_Array[seq] = LED_1_3; // GET FROM JSON
          led_2_Array[seq] = LED_2_3; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
        if (seq == 4) {
          led_1_Array[seq] = LED_1_4; // GET FROM JSON
          led_2_Array[seq] = LED_2_4; // GET FROM JSON
          initialColorArray[seq] = CRGB::Blue; // GET FROM JSON
          targetColorArray[seq] = CRGB::Green; // GET FROM JSON
        }
      }
      if (sequenceSet == 5) {
        if (seq == 1) {
          led_1_Array[seq] = LED_2_1; // GET FROM JSON
          led_2_Array[seq] = LED_1_1; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
        if (seq == 2) {
          led_1_Array[seq] = LED_2_2; // GET FROM JSON
          led_2_Array[seq] = LED_1_2; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
        if (seq == 3) {
          led_1_Array[seq] = LED_2_3; // GET FROM JSON
          led_2_Array[seq] = LED_1_3; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
        if (seq == 4) {
          led_1_Array[seq] = LED_2_4; // GET FROM JSON
          led_2_Array[seq] = LED_1_4; // GET FROM JSON
          initialColorArray[seq] = CRGB::Green; // GET FROM JSON
          targetColorArray[seq] = CRGB::Red; // GET FROM JSON
        }
      }
      

      led_1_m_Array[seq] = map(led_1_Array[seq], 0, NUM_LEDS-1, 0, 255);
      led_2_m_Array[seq] = map(led_2_Array[seq], 0, NUM_LEDS-1, 0, 255);
      currentLed_Array[seq] = led_1_Array[seq];
      currentLed_m_Array[seq] = map(currentLed_Array[seq], 0, NUM_LEDS-1, 0, 255);

      if (led_2_Array[seq] > led_1_Array[seq]) setToFadePaletteForward(led_1_m_Array[seq], led_2_m_Array[seq], initialColorArray[seq], targetColorArray[seq]);
      else setToFadePaletteBackward(led_1_m_Array[seq], led_2_m_Array[seq], initialColorArray[seq], targetColorArray[seq]);
    }
    else break;
  }

  sequenceSet++;
  if (sequenceSet == 6) sequenceSet = 0;//numOfSequencesInSetLen) sequenceSet = 0;
}

void setToFadePaletteBackward(int led_1_m, int led_2_m, CRGB initialColor, CRGB targetColor) { // led_2 < led_1
  //fill_solid(toFadePalette, 256, CRGB::Black);
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

void setToFadePaletteForward(int led_1_m, int led_2_m, CRGB initialColor, CRGB targetColor) { // led_2 > led_1
  //fill_solid(toFadePalette, 256, CRGB::Black);
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
  /*
  for (int i=0; i<255; i++) {
    Serial.print("id: ");
    Serial.print(i);
    Serial.print(", red: ");
    Serial.print(finalPalette[i].r);
    Serial.print(", green: ");
    Serial.print(finalPalette[i].g);
    Serial.print(", blue: ");
    Serial.println(finalPalette[i].b);
  }
  */
}

// Helper function that blends one uint8_t toward another by a given amount
void nblendU8TowardU8(uint8_t &current, const uint8_t target)
{
  if (current == target) return;

  if (current < target) {
    uint8_t delta = target - current;
    delta = scale8_video(delta, 16); // 4 (old 120)
    current += delta;
  }
  else {
    uint8_t delta = current - target;
    delta = scale8_video(delta, 16); // 4
    current -= delta;
  }
}
