#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48


#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  Serial.println("Start-up ");
  // analogReference(INTERNAL2V56);
  pinMode(45, OUTPUT);
}

void loop() {
  int sensorValue;
  int angle;
  Serial.println("on");
  digitalWrite(45, HIGH);
  delay(2000);
  Serial.println("off");
  digitalWrite(45, LOW);
  delay(2000);
  // sensorValue = analogRead(A0);
  // angle = map(sensorValue, 33, 610, -45, 45);
  // Serial.print(sensorValue);
  // Serial.print(", ");
  // Serial.println(angle);
  // // TimerManager::instance().update();

  if ( Serial.available() ) { Serial.read(); }
}
