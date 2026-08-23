#ifndef WORDLE_RFID_H
#define WORDLE_RFID_H

#include <Arduino.h>
#include <WebServer.h>
#include <MFRC522v2.h> 

#define NUM_READERS 5

extern uint8_t SS_PINS[NUM_READERS];
extern MFRC522 readers[NUM_READERS];

void initRfid();
void getRfidCards(String* results);

#endif