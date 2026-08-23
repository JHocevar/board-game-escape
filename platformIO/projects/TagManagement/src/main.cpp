#include <Arduino.h>
#include <HTTPClient.h>

#include "BGE_wifi.h"
#include "BGE_rfid.h"
// #include "BGE_screen.h"
#include "BGE_server.h"

String content = "";

void setup() {
  Serial.begin(115200);
  while (!Serial);  // Wait until serial port opens
  Serial.println("Serial Ready");

  initRfid();
  initWifi();
  // initScreen();
  }

const String letters[] = {"A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z"};
constexpr size_t letterCount = sizeof(letters) / sizeof(letters[0]);
int letterIndex = 0;

bool logged = false;
void assignLetters() {
  if (letterIndex >= letterCount) {
    // printScreenText("All letters assigned!", 13, 36, 2);
    if (!logged) {
      Serial.println("All letters assigned!");
      logged = true;
    }
    return;
  }

  RfidScanResult scan = checkForRfid();

  String letter = letters[letterIndex];
  // printScreenText("Scan tag for " + letter, 13, 36, 2);
  if (!logged) {
    Serial.println("Scan tag for letter: " + letter);
    logged = true;
  }
  String result;
  if (scan.newCard) {
    Serial.println("Got new card with UID: " + scan.uid);
    result = saveTag(scan.uid, letter);
    letterIndex += 1;
    // printScreenText(result, 13, 52, 2);
    Serial.println("Result: " + result);
    delay(3000);
    logged = false;
  }
}

void readLetter() {
  RfidScanResult scan = checkForRfid();

  if (content == "") {
    // printScreenText("Scan tag...", 13, 36, 2);
    Serial.println("Scan tag...");
  }

  if (scan.newCard) {
    content = getTagContent(scan.uid);
    // printScreenText(content, 13, 52, 2);
    Serial.println("Content: " + content);
  }
}

void loop() {
  assignLetters();
  // readLetter();
}