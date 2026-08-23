#ifndef BGE_led_H
#define BGE_led_H

#include <Arduino.h>
#include <FastLED.h>

#define LED_PIN             33
#define BRIGHTNESS          255
#define LED_TYPE            WS2811
#define COLOR_ORDER         GRB
#define UPDATES_PER_SECOND  100

#ifndef NUM_LEDS
#define NUM_LEDS 9 // Default
#endif

extern CRGB leds[];

void initLEDs();
void resetReadHistory();

#endif