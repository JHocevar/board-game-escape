#include "BGE_mqtt.h"

EspMQTTClient client(
  SSID_NAME,      // WIFI SSID
  SSID_PASSWORD,  // WIFI Password
  "192.168.4.34", // MQTT Broker IP address
  "mqtt",         // MQTT User
  "mqtt"          // MQTT Password
);

void onConnectionEstablished() {
  client.subscribe("mytopic/test", [] (const String &payload)  {
    Serial.println(payload);
  });

  client.publish("mytopic/test", "This is a message");
}