#ifndef PRINTERS_H
#define PRINTERS_H

#include <Arduino.h>

void printHex(byte *buffer, byte bufferSize);
void printDec(byte *buffer, byte bufferSize);

#endif