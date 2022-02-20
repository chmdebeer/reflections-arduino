#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <rfid.h>

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

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.

void setupRFID() {
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
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
  uint8_t uid[] = { 0, 0, 0, 0 };
  uint8_t uidLength = 4;

  // Look for new cards
  if (mfrc522.PICC_IsNewCardPresent()) {
    if (mfrc522.PICC_ReadCardSerial()) {
      uidLength = mfrc522.uid.size;

      Serial.print("UID tag :");
      for (byte i = 0; i < uidLength; i++)
      {
        uid[i] = mfrc522.uid.uidByte[i];
        Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
        Serial.print(mfrc522.uid.uidByte[i], HEX);
      }
      Serial.println();
      mfrc522.PICC_HaltA();
      mfrc522.PCD_StopCrypto1();

      for ( uint8_t c = 0; c < 9; c++) {  //
        if (checkTwo(storedCard[c], uid)) {
          return true;
        }
      }

    }
  }

  return false;
}
