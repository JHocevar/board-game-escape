#ifndef BGE_rfid_H
#define BGE_rfid_H

#include <Arduino.h>

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

struct RfidScanResult {
  bool newCard;
  String uid;
  String content;
};

void initRfid();
RfidScanResult checkForRfid();

#endif
