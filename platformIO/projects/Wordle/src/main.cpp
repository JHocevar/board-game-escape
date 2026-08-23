// Core
#include <HTTPClient.h>
#include <Arduino.h>

// LED Strop
#include <FastLED.h>

// Shared Libraries
#include "BGE_led.h"
#include "BGE_wifi.h"
#include "BGE_server.h"
#include "BGE_screen.h"

#include "game.h"
#include "wordle_server.h"
#include "leds.h"
#include "words.h"
#include "rfid.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);
  Serial.println("Serial Ready");

  initRfid();
  initWifi();
  initServer();
  initScreen();
  initLEDs();
  initButton();
  
  if (!loadWords()) {
    Serial.println("Failed to load words, halting");
    while (true) delay(1000);
  }

  Serial.println("\nSetup complete\n");
}

void loop() {
  display.clearDisplay();
  server.handleClient();

  // Rest to guessing status if timeout has passed
  checkStatus();

  // Update OLED screen
  printCurrentWord();
  printStatusMessage();
  display.display();

  // Update the LEDs with current status
  displayLEDs(leds);

  // Check if new letter is present on the readeer
  readLetters();

  // Check for button press to submit the word
  checkSubmit();
}
