/*
 * Standby: Twinkling lights of random colours
 */
 
const uint8_t FADE_RATE = 1; // How long should the trails be. Very low value = longer trails.

void twinkle() {
  if (random(5) == 1) {
    uint16_t i = random(NUM_LEDS);
    leds[i] = CRGB(random(0,80), random(256), random(256));
  }
  fadeToBlackBy(leds, NUM_LEDS, FADE_RATE);

  if (random(5) == 1) {
    uint16_t i = random(NUM_LEDS);
    leds[i] = CRGB(random(0,80), random(256), random(256));
  }
  fadeToBlackBy(leds, NUM_LEDS, FADE_RATE);
}
