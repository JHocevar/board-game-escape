#include "words.h"
#include <vector>
#include <LittleFS.h>

bool loadWords() {
  if (!LittleFS.begin()) {
    Serial.println("Error mounting LittleFS");
    return false;
  }

  File file = LittleFS.open("/words.txt");
  if (!file) {
    Serial.println("Error opening words file");
    return false;
  }

  int count = 0;
  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() > 0) count++;
  }
  file.close();

  Serial.printf("Words file OK, %d words found\n", count);
  return true;
}

bool isValidWord(const String& word) {
  File file = LittleFS.open("/words.txt");
  if (!file) return false;

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.equalsIgnoreCase(word)) {
      file.close();
      return true;
    }
  }
  file.close();
  return false;
}
