#ifndef BGE_screen_H
#define BGE_screen_H

#include <Arduino.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

extern Adafruit_SSD1306 display;

void printScreenText(String, int, int, int);
void printRectangle(int x, int y, int width, int height);
void initScreen();

#endif
