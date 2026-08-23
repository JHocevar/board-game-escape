#ifndef BGE_mqtt_H
#define BGE_mqtt_H

#include <Arduino.h>
#include "EspMQTTClient.h"

extern EspMQTTClient client;

void onConnectionEstablished();

#endif
