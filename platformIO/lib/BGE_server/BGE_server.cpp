#include "BGE_server.h"

#include <map>

#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <stdexcept>
#include <WiFi.h>

#define SERVER_URL "http://192.168.4.50:8080"

String getHealthCheck() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(SERVER_URL);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      Serial.print("HTTP status: ");
      Serial.println(httpCode);
      Serial.println("Response: " + payload);
      http.end();
      return payload;
    } else {
      String errorMessage = http.errorToString(httpCode);
      Serial.print("Request failed, error: ");
      Serial.println(errorMessage);
      http.end();
      return "Request failed: " + errorMessage;
    }
  } else {
    Serial.println("WiFi not connected, reconnection...");
    WiFi.reconnect();
    return "WiFi not connected";
  }
}


std::map<String, String> tagCache;

String getTagContent(const String& tagId) {
  auto cached = tagCache.find(tagId);
  if (cached != tagCache.end()) {
    return cached->second;
  }

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(SERVER_URL + String("/tag?id=") + tagId);
    int httpCode = http.GET();

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.print("Response (" + String(httpCode) + ") " + payload);
      http.end();

      JsonDocument doc;
      DeserializationError err = deserializeJson(doc, payload);
      if (err) {
        Serial.print("JSON parse failed: ");
        Serial.println(err.c_str());
        return String();
      }

      String idValue = doc["Id"].as<String>();
      String contentValue = doc["Content"].as<String>();
      tagCache[tagId] = contentValue;
      return contentValue;
    }
    http.end();
  }
  return String();
}

String saveTag(const String& tagId, const String& tagData) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    http.begin(SERVER_URL + String("/tag?id=") + tagId + String("&content=") + tagData);
    int httpCode = http.POST("");

    if (httpCode == 200) {
      String payload = http.getString();
      Serial.print("HTTP status: ");
      Serial.println(httpCode);
      Serial.println("Response: " + payload);
      http.end();
      return payload;
    } else {
      String errorMessage = http.errorToString(httpCode);
      Serial.print("Request failed, error: ");
      Serial.println(errorMessage);
      http.end();
      throw std::runtime_error(errorMessage.c_str());
    }
  } else {
    Serial.println("WiFi not connected, reconnection...");
    WiFi.reconnect();
    throw std::runtime_error("WiFi not connected");
  }
}
