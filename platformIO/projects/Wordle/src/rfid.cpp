#include "rfid.h"

#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <SPI.h>
#include <WebServer.h>

#include "BGE_server.h"
#include "game.h"

// const uint8_t SS_PINS[NUM_READERS] = {5, 4, 16, 17, 32};
// const uint8_t SS_PINS[NUM_READERS] = {32, 16, 4, 5, 17};
uint8_t SS_PINS[NUM_READERS] = {17, 16, 4, 32, 5};

MFRC522DriverPinSimple ssPins[NUM_READERS] = {
  MFRC522DriverPinSimple(SS_PINS[0]),
  MFRC522DriverPinSimple(SS_PINS[1]),
  MFRC522DriverPinSimple(SS_PINS[2]),
  MFRC522DriverPinSimple(SS_PINS[3]),
  MFRC522DriverPinSimple(SS_PINS[4])
};

MFRC522DriverSPI drivers[NUM_READERS] = {
  MFRC522DriverSPI{ssPins[0]},
  MFRC522DriverSPI{ssPins[1]},
  MFRC522DriverSPI{ssPins[2]},
  MFRC522DriverSPI{ssPins[3]},
  MFRC522DriverSPI{ssPins[4]}
};

MFRC522 readers[NUM_READERS] = {
  MFRC522(drivers[0]),
  MFRC522(drivers[1]),
  MFRC522(drivers[2]),
  MFRC522(drivers[3]),
  MFRC522(drivers[4])
};

static String uidBytesToHexString(const byte uid[], byte uidSize) {
  String result;
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] < 0x10) result += "0";
    result += String(uid[i], HEX);
  }
  result.toUpperCase();
  return result;
}

bool readerFailed[NUM_READERS] = {false};
uint8_t readerFailCount[NUM_READERS] = {0};
#define FAIL_THRESHOLD 5

void initRfid() {
  delay(500);
  SPI.begin();
  for (uint8_t i = 0; i < NUM_READERS; i++) {
    readers[i].PCD_Init();
    readers[i].PCD_SetAntennaGain(MFRC522::PCD_RxGain::RxGain_min);
  }
  Serial.println(F("All MFRC522 readers ready."));
  Serial.flush();
}

void getRfidCards(String* results) {
  for (uint8_t i = 0; i < NUM_READERS; i++) {
    results[i] = "";

    for (uint8_t j = 0; j < NUM_READERS; j++) {
      if (j != i) readers[j].PCD_AntennaOff();
    }
    readers[i].PCD_AntennaOn();
    delay(10);

    byte bufferATQA[2];
    byte bufferSize = sizeof(bufferATQA);
    readers[i].PCD_StopCrypto1();

    MFRC522::StatusCode status = readers[i].PICC_WakeupA(bufferATQA, &bufferSize);
    if (status != MFRC522::StatusCode::STATUS_OK) {
      readerFailCount[i]++;
      if (readerFailCount[i] >= FAIL_THRESHOLD) {
        readerFailed[i] = true;
      }
      continue;
    } else {
      readerFailCount[i] = 0;
      readerFailed[i] = false;
    }

    if (!readers[i].PICC_ReadCardSerial()) {
      continue;
    }

    String tag = uidBytesToHexString(readers[i].uid.uidByte, readers[i].uid.size);
    results[i] = getTagContent(tag);
    readers[i].PICC_HaltA();
  }

  for (uint8_t i = 0; i < NUM_READERS; i++) {
    readers[i].PCD_AntennaOn();
  }
}
