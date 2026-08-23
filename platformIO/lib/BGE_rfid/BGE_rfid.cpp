#include "BGE_rfid.h"

// RFID Library
#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>

#define SS_PIN 16
#define LED_READY_PIN 14

unsigned long readyCooldownUntil = 0;
const unsigned long READY_COOLDOWN_MS = 2000;

MFRC522DriverPinSimple ssPin(SS_PIN);
MFRC522DriverSPI driver{ssPin};
MFRC522 rfid(driver);

byte nuidPICC[10] = {0};
byte nuidPICCSize = 0;
bool previousCardPresent = false;
unsigned long lastCardRemovedAt = 0;

void setReadyLed(bool ready) {
  digitalWrite(LED_READY_PIN, ready ? HIGH : LOW);
}

void initRfid() {
  SPI.begin();
  rfid.PCD_Init();

  pinMode(LED_READY_PIN, OUTPUT);
  setReadyLed(true);
  
  Serial.println(F("MFRC522v2 ready. Waiting for cards..."));
  Serial.flush();
}

static String uidBytesToHexString(const byte uid[], byte uidSize) {
  String result;
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] < 0x10) {
      result += "0";
    }
    result += String(uid[i], HEX);
  }
  result.toUpperCase();
  return result;
}

RfidScanResult checkForRfid() {
  RfidScanResult result;
  result.newCard = false;
  result.uid = "";
  result.content = "";

  if (!rfid.PICC_IsNewCardPresent()) {
    if (previousCardPresent) {
      previousCardPresent = false;
      lastCardRemovedAt = millis();
    }
    if (millis() >= readyCooldownUntil && readyCooldownUntil != 0) {
      setReadyLed(true);
      readyCooldownUntil = 0;
    }
    return result;
  }

  if (millis() < readyCooldownUntil) {
    return result;
  }

  setReadyLed(true);

  if (!rfid.PICC_ReadCardSerial()) {
    return result;
  }

  readyCooldownUntil = millis() + READY_COOLDOWN_MS;
  setReadyLed(false);

  // Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  // Serial.println(MFRC522Debug::PICC_GetTypeName(piccType));

  bool sameUid = (rfid.uid.size == nuidPICCSize);
  if (sameUid) {
    for (byte i = 0; i < rfid.uid.size; i++) {
      if (rfid.uid.uidByte[i] != nuidPICC[i]) {
        sameUid = false;
        break;
      }
    }
  }

  unsigned long elapsedSinceRemoval = millis() - lastCardRemovedAt;
  bool removedLongEnough =
      !previousCardPresent &&
      lastCardRemovedAt != 0 &&
      elapsedSinceRemoval >= READY_COOLDOWN_MS;
  bool isNewCard = !sameUid || removedLongEnough;

  if (!isNewCard) {
    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
    return result;
  }

  result.newCard = true;
  result.uid = uidBytesToHexString(rfid.uid.uidByte, rfid.uid.size);
  for (byte i = 0; i < rfid.uid.size; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }
  nuidPICCSize = rfid.uid.size;
  previousCardPresent = true;
  lastCardRemovedAt = 0;

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  return result;
}
