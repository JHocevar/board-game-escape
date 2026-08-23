#include "BGE_wifi.h"

#include <WiFi.h>

void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID_NAME, SSID_PASSWORD);
  Serial.print("Connecting to WiFi ..");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}