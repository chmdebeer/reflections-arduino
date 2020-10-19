#include <Arduino.h>
#include "EmonLib.h"
#include <Wire.h>

void i2cReceive (int howMany);
void i2cRequest();

EnergyMonitor emon1;

float volts[10] = {};
float current[10] = {};

void setup()
{
  Serial.begin(115200);

  Wire.begin(8);
  Wire.onReceive(i2cReceive);
  Wire.onRequest(i2cRequest);

  emon1.voltage(7, 429.4, 1.7);
  emon1.current(0, 44.2);
}

void loop()
{
  static int count = 0;

  emon1.calcVI(60,1100);

  volts[count] = emon1.Vrms;
  current[count] = emon1.Irms;
  count += 1;
  if (count > 9) {
    Serial.print("Volts ");
    Serial.println(emon1.Vrms);
    Serial.print("Amps ");
    Serial.println(emon1.Irms);

    count = 0;
  }
}

void i2cReceive (int howMany)
{
  int SlaveReceived;
  SlaveReceived = Wire.read();
  Serial.print("Slave Received ");
  Serial.println(SlaveReceived);
}

void i2cRequest()
{
  struct AC {
    float volts;
    float amps;
  } ac;

  Serial.println("Master Request");

  ac.volts = (volts[0] + volts[1] + volts[2] + volts[3] + volts[4] + volts[5] + volts[6] + volts[7] + volts[8] + volts[9]) / 10.0;
  ac.amps = (current[0] + current[1] + current[2] + current[3] + current[4] + current[5] + current[6] + current[7] + current[8] + current[9]) / 10.0;

  Serial.print("volt = ");
  Serial.println(ac.volts);
  Serial.print("amp = ");
  Serial.println(ac.amps);

  Serial.println(sizeof(AC));
  Wire.write((byte *)&ac, sizeof(AC));

}
