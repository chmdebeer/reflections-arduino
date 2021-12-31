#include <Arduino.h>
#include "EmonLib.h"
#include <Wire.h>
#include <rfid.h>

#define O_DOOR_LOCK_SIGNAL 15
#define O_LED 17

void readLock();


void setup()
{
  Serial.begin(115200);
  Serial.println("Starting");

  pinMode(O_LED, OUTPUT);
  pinMode(O_DOOR_LOCK_SIGNAL, OUTPUT);
  digitalWrite(O_LED, 0);
  digitalWrite(O_DOOR_LOCK_SIGNAL, 0);

  setupRFID();
}

void loop()
{
  if (readRFID()) {
    digitalWrite(O_LED, 1);
    digitalWrite(O_DOOR_LOCK_SIGNAL, 0);
    Serial.println("Pulling door lock signal down");
  } else {
    digitalWrite(O_LED, 0);
    digitalWrite(O_DOOR_LOCK_SIGNAL, 1);
  }
  delay(200);
}
