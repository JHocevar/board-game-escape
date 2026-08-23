// Core
#include <Arduino.h>
#include <HTTPClient.h>

// Helpers
#include "printers.h"

// Shared Libraries
#include "BGE_led.h"
#include "BGE_wifi.h"
#include "BGE_server.h"
#include "BGE_rfid.h"
#include "BGE_screen.h"
#include "BGE_mqtt.h"

// Ready indicator and button
// #define BUTTON_PIN 26
// bool buttonPressedLast = false;

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait until serial port opens
  Serial.println("Ready");

  initRfid();

  initWifi();

  // pinMode(BUTTON_PIN, INPUT_PULLUP);
  // buttonPressedLast = digitalRead(BUTTON_PIN) == LOW;
  
  initScreen();
  // initLEDs();

  Serial.println("Setup complete.");
}

void loop() {
  client.loop();
  // bool buttonPressed = digitalRead(BUTTON_PIN) == LOW;
  // if (buttonPressed && !buttonPressedLast) {
  //   resetReadHistory();
  // }
  // buttonPressedLast = buttonPressed;

  RfidScanResult scan = checkForRfid();

  if (scan.newCard) {
    String content = getTagContent(scan.uid);
    Serial.println("New card detected with content: " + content);
  }

  // printRectangle(0, 0, 16, 16);
  // printRectangle(16, 16, 16, 16);
  // printRectangle(32, 32, 16, 16);
  // printRectangle(48, 48, 16, 16);

  // RfidScanResult scan = checkForRfid();
  // if (scan.newCard) {
  //   Serial.println(F("A new card has been detected."));
  //   printScreenText("Lauren is the best", 2);
  //   Serial.print("Health check: ");
  //   Serial.println(getHealthCheck());
  //   Serial.println("End of health check.");

  //   leds[currentLedIndex] = tagColors[currentLedIndex];
  //   FastLED.show();
  //   currentLedIndex = (currentLedIndex + 1) % NUM_LEDS;

  //   Serial.println(F("The UID tag is:"));
  //   Serial.print(F("UID: "));
  //   Serial.println(scan.uid);
  // }
}
