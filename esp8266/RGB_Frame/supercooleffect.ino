#define TRAIL_SIZE  1
#define DECELERATION  -1
#define INITIAL_VELOCITY_HOR  0.983192
#define INITIAL_VELOCITY_VER  1.125463
#define NUM_OF_TRAILS  4
#define NUM_OF_PATHS   4
#define STRIP_LENGTH  2.233333 // Length of led strip in meters
bool sce_first = true;

float tCycle[NUM_OF_TRAILS]; // Time (t variable)
long tLast[NUM_OF_TRAILS]; // Time (millis reference time)
float pReal[NUM_OF_TRAILS]; // Position (p variable)
float pRealInitial[NUM_OF_TRAILS]; // Initial position (p0 variable)
float vReal[NUM_OF_TRAILS]; // Velocity (v variable)
float vRealInitial[NUM_OF_TRAILS]; // Initial velocity (v0 variable)
float vRealInitialRef[NUM_OF_PATHS] = {INITIAL_VELOCITY_HOR, INITIAL_VELOCITY_VER, INITIAL_VELOCITY_HOR, INITIAL_VELOCITY_VER}; // 4 "paths"
int TRAIL_POSITIONS_START[NUM_OF_TRAILS] = {0, 29, 67, 96}; // Starting led index positions
int TRAIL_POSITIONS[NUM_OF_TRAILS]; // Position (led index)
uint8_t color_changing_counter = 0;
uint8_t vref_counter[NUM_OF_TRAILS] = {0, 0, 0, 0};
uint8_t vref = 0;

void sce_initialization() {
  for (int i=0; i<NUM_OF_TRAILS ;i++) {
    tLast[i] = millis();
    tCycle[i] = 0;
    TRAIL_POSITIONS[i] = TRAIL_POSITIONS_START[i];
    pReal[i] = (TRAIL_POSITIONS[i] * STRIP_LENGTH) / (NUM_LEDS - 1);
    pRealInitial[i] = pReal[i];
    vReal[i] = vRealInitialRef[i];
    vRealInitial[i] = vReal[i];
    vref_counter[i] = 0;
  }
}

void supercooleffect() {
  if (sce_first) {
    sce_first = false;
    sce_initialization();
  }
  if (TRAIL_POSITIONS[0] == 133) sce_initialization(); // Reset cycle for discosidering decimal errors
  
  for(int i=0; i<NUM_OF_TRAILS; i++) {
    tCycle[i] = millis() - tLast[i];    
    pReal[i] = 0.5*DECELERATION*pow(tCycle[i]/1000, 2.0) + vRealInitial[i]*tCycle[i]/1000 + pRealInitial[i];
    vReal[i] = DECELERATION*tCycle[i]/1000 + vRealInitial[i];

    if (pReal[i] <= 0) pReal[i] = STRIP_LENGTH + pReal[i];
    if (pReal[i] >= STRIP_LENGTH) pReal[i] = pReal[i] - STRIP_LENGTH;

    TRAIL_POSITIONS[i] = round(pReal[i]*(NUM_LEDS - 1) / STRIP_LENGTH);
    
    if (vReal[i] <= 0) {
      //pReal[i] = 0; pReal[i] = 0.483; pReal[i] = 1.115; pReal[i] = 1.598;
      vref_counter[i] = vref_counter[i] + 1;
      //if (vref_counter[i] >= NUM_OF_PATHS) vref_counter[i] = 0;
      vref = vref_counter[i] + i;
      if (vref >= NUM_OF_PATHS) vref = vref - NUM_OF_PATHS;
      vReal[i] = vRealInitialRef[vref];
      pRealInitial[i] = pReal[i];
      vRealInitial[i] = vReal[i];
      color_changing_counter = color_changing_counter + 2;
      if (color_changing_counter >= 255) color_changing_counter = 0;
      tLast[i] = millis();
    }
  }
  
  for(int i=0; i<NUM_OF_TRAILS; i++) leds[TRAIL_POSITIONS[i]] = CHSV(color_changing_counter , 255, 255);  
  if (color_changing_counter >= 255) color_changing_counter = 0;
  fadeToBlackBy(leds, NUM_LEDS, TRAIL_SIZE);
}
