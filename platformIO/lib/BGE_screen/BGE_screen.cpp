#include <Arduino.h>
#include "BGE_screen.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void initScreen() {
  delay(100);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();

  delay(2000); // Pause for 2 seconds
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 30);

  // Display static text
  display.println("initializing..");
  display.display(); 
}

void printScreenText(String text, int x, int y, int textSize) {
  display.setTextSize(textSize);
  display.setCursor(x, y);
  display.println(text);
}

void printRectangle(int x, int y, int width, int height) {
  display.fillRect(x, y, width, height, WHITE);
  display.display();
}
