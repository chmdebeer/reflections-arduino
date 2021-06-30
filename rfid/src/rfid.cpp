#include <Arduino.h>
#include <Wire.h>
#include <rfid.h>
#include <PN532_I2C.h>
#include <PN532.h>

byte readCard[4];
byte lastCard[4];
byte storedCard[9][4] = {
  {0x23, 0xB5, 0xC9, 0x3E},
  {0x46, 0xFE, 0x2C, 0x25},
  {0x5C, 0x4F, 0xEE, 0x0D},
  {0x0C, 0xF8, 0xF9, 0x0E},
  {0xEA, 0xAD, 0x15, 0x78},
  {0x6C, 0x1B, 0xF3, 0x0D},
  {0x6A, 0x2B, 0x0D, 0x78},
  {0x7A, 0x47, 0xF4, 0x3F},
  {0x89, 0x1E, 0x68, 0x8E}
};

PN532_I2C pn532I2C(Wire);
PN532 nfc(pn532I2C);

void setupRFID() {
  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }

  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX);
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC);
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);

  // Set the max number of retry attempts to read from a card
  // This prevents us from waiting forever for a card, which is
  // the default behaviour of the PN532.
  nfc.setPassiveActivationRetries(0xFF);

  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A card");
}

void dump_byte_array(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

bool checkTwo ( byte a[], byte b[] ) {
  for ( uint8_t k = 0; k < 4; k++ ) {   // Loop 4 times
    if ( a[k] != b[k] ) {     // IF a != b then false, because: one fails, all fail
       return false;
    }
  }
  return true;
}

bool readRFID () {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  static uint8_t lastUid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;

  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength, 100);

  if (success) {
    // if (checkTwo(uid, lastUid)) {
    //   return false;
    // }
    // for (uint8_t i=0; i < uidLength; i++) {
    //   lastUid[i] = uid[i];
    // }

    Serial.println("Found a card!");
    Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("UID Value: ");
    for (uint8_t i=0; i < uidLength; i++)
    {
      Serial.print(" 0x");Serial.print(uid[i], HEX);
    }
    Serial.println("");

    for ( uint8_t c = 0; c < 9; c++) {  //
      if (checkTwo(storedCard[c], uid)) {
        return true;
      }
    }
  }
  for (uint8_t i=0; i < uidLength; i++) {
    lastUid[i] = 0;
  }
  return false;
}
