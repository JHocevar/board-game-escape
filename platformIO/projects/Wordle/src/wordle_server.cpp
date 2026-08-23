#include "wordle_server.h"

#include <ArduinoJson.h>
#include <WebServer.h>

#include "rfid.h"
#include "game.h"

WebServer server(80);

void initServer() {
  server.on("/status", HTTP_GET, handleStatus);
  server.on("/reboot", HTTP_POST, handleReboot);
  server.on("/restart", HTTP_POST, handleRestart);
  server.on("/setLetter", HTTP_POST, handleSetLetter);
  server.begin();
  Serial.println("HTTP server started");
}

void handleReboot() {
  server.send(200, "text/plain", "Rebooting...");
  delay(100);
  ESP.restart();
}

void handleRestart() {
  if (server.hasArg("plain") && server.arg("plain").length() > 0) {
    String body = server.arg("plain");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("Error parsing JSON");
      server.send(400, "application/json", "{\"message\":\"invalid JSON\"}");
      return;
    }

    if (doc["newWord"].is<String>()) {
      CORRECT_WORD = doc["newWord"].as<String>();
    }
  }

  wordleGuessCooldown = 0;
  currentGameState = GUESSING;
  previousWordIndex = 0;
  for (int i = 0; i < NUM_READERS; i++) {
    overrideCards[i] = "";
    slotOverridden[i] = false;
    currentWord[i] = "";
  }
  for (int i = 0; i < 6; i++) {
    previousWords[i] = "";
  }

  server.send(200, "application/json", "{\"message\": \"success\"}");
}

void handleStatus() {
  JsonDocument doc;

  doc["correctWord"] = CORRECT_WORD;
  doc["currentWord"] =
      String(currentWord[0]) +
      String(currentWord[1]) +
      String(currentWord[2]) +
      String(currentWord[3]) +
      String(currentWord[4]);

  JsonArray previousWordsJson = doc["previousWords"].to<JsonArray>();
  for (int i = 0; i < previousWordIndex; i++) {
    previousWordsJson.add(previousWords[i]);
  }

  JsonArray readersJson = doc["readers"].to<JsonArray>();

  for (int i = 0; i < NUM_READERS; i++) {
    JsonObject reader = readersJson.add<JsonObject>();

    reader["index"] = i;
    reader["pin"] = SS_PINS[i];

    byte version = readers[i].PCD_GetVersion();
    reader["healthy"] = version != 0x00 && version != 0xFF;
    reader["version"] = "0x" + String(version, HEX);
    reader["card"] = confirmedCards[i];
  }

  String json;
  serializeJson(doc, json);
  server.send(200, "application/json", json);
}

void handleSetLetter() {
  if (server.hasArg("plain")) {
    String body = server.arg("plain");
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      Serial.println("Error parsing JSON");
      return;
    }

    int readerIndex = doc["readerIndex"];
    String letter = doc["letter"];

    if (readerIndex >= 0 && readerIndex < NUM_READERS) {
      currentWord[readerIndex] = letter;
      overrideCards[readerIndex] = letter;
      slotOverridden[readerIndex] = letter != "";
      Serial.printf("Set letter for reader %d to %s\n", readerIndex, letter.c_str());
    } else {
      Serial.printf("Invalid reader index: %d\n", readerIndex);
    }
  }
  server.send(200, "application/json", "{\"message\": \"success\"}");
}