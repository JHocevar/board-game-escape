#include "BGE_led.h"

#include <FastLED.h>

uint8_t currentLedIndex = 0;
CRGB leds[NUM_LEDS];

void initLEDs() {
  delay(500);
  
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
  FastLED.setBrightness(BRIGHTNESS);
  
  fill_solid(leds, NUM_LEDS, CRGB::Teal);
  FastLED.show();
}

void resetReadHistory() {
  currentLedIndex = 0;
  fill_solid(leds, NUM_LEDS, CRGB::White);
  FastLED.show();
  Serial.println("Read history reset.");
}
