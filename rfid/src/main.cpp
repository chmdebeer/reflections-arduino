#include <Arduino.h>
#include <timer.h>
#include <rfid.h>

#define O_DOOR_LOCK_SIGNAL 14
#define O_LED 15

Timer timer;

void resetRfid() {
  Serial.println("LED off");
  digitalWrite(O_LED, 0);
  digitalWrite(O_DOOR_LOCK_SIGNAL, 1);
}

void setup() {
  Serial.begin(115200);   // Initiate a serial communication
  pinMode(O_LED, OUTPUT);
  pinMode(O_DOOR_LOCK_SIGNAL, OUTPUT);
  digitalWrite(O_LED, 0);
  digitalWrite(O_DOOR_LOCK_SIGNAL, 1);

  timer.setInterval(1000, 1);
  timer.setCallback(resetRfid);

  setupRFID();
  Serial.println("Reader ready...");
}

void loop()
{
  if (readRFID()) {
    digitalWrite(O_LED, 1);
    digitalWrite(O_DOOR_LOCK_SIGNAL, 0);
    Serial.println("Pulling door lock signal down");
    timer.start();
  }
  delay(200);
  timer.update();

}
