#ifndef BGE_server_H
#define BGE_server_H

#include <Arduino.h>

String getHealthCheck();
String getTagContent(const String& tagId);
String saveTag(const String& tagId, const String& tagData);

#endif
