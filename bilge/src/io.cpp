#include <Arduino.h>
#include <Bounce2.h>
#include <N2kTypes.h>
#include "BoatData.h"
#include <io.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "utils.h"

OneWire oneWire(I_ONE_WIRE_BUS);

DallasTemperature sensors(&oneWire);

uint8_t belowDeckTemperatureSensorID[8] = { 0x28, 0xFF, 0x8A, 0x51, 0x71, 0x17, 0x04, 0xCE };

void setupIO() {

  pinMode(O_MIDSHIP_BILDGE_LIGHTS, OUTPUT);
  pinMode(O_SHOWER_PUMP, OUTPUT);
  pinMode(O_MACERATOR, OUTPUT);
  pinMode(O_MOSFET_4, OUTPUT);

  pinMode(O_SPOT_LIGHT_SPOT, OUTPUT);
  pinMode(O_SPOT_LIGHT_FOG, OUTPUT);
  pinMode(O_SPOT_LIGHT_UP, OUTPUT);
  pinMode(O_SPOT_LIGHT_DOWN, OUTPUT);
  pinMode(O_SPOT_LIGHT_LEFT, OUTPUT);
  pinMode(O_SPOT_LIGHT_RIGHT, OUTPUT);
  pinMode(O_MIDSHIP_BILGE, OUTPUT);
  pinMode(O_BLACKWATER_AIR_PUMP, OUTPUT);

  pinMode(I_MIDSHIP_BILGE, INPUT);

  sensors.begin();
}

bool readIO(BoatData &boatData) {
  static bool bilgePumpOn = false;
  bool newIO = false;
  double value;

  value = analogRead(I_MIDSHIP_BILGE);
  if (value > 200) {
    boatData.bilgePumps.midship.floatSwitch = N2kOnOff_On;
    if (!bilgePumpOn) {
      bilgePumpOn = true;
      newIO = true;
      Serial.println("On");
    }
  } else {
    boatData.bilgePumps.midship.floatSwitch = N2kOnOff_Off;
    if (bilgePumpOn) {
      bilgePumpOn = false;
      newIO = true;
      Serial.println("Off");
    }
  }

  return newIO;
}

void setIO(BoatData &boatData) {
  digitalWrite(O_MOSFET_4, LOW);

  digitalWrite(O_MIDSHIP_BILDGE_LIGHTS, boatData.lights.belowDeck == N2kOnOff_On);
  digitalWrite(O_SHOWER_PUMP, boatData.utilities.showerDrainPump == N2kOnOff_On);
  digitalWrite(O_MACERATOR, boatData.utilities.macerator == N2kOnOff_On);

  digitalWrite(O_BLACKWATER_AIR_PUMP, !(boatData.utilities.blackwaterAirPump == N2kOnOff_On));
  digitalWrite(O_MIDSHIP_BILGE, !(boatData.bilgePumps.midship.on == N2kOnOff_On));

  if (boatData.lights.spotlight.fog == N2kOnOff_On) {
    digitalWrite(O_SPOT_LIGHT_SPOT, HIGH);
    digitalWrite(O_SPOT_LIGHT_FOG, LOW);
  } else if (boatData.lights.spotlight.spot == N2kOnOff_On) {
    digitalWrite(O_SPOT_LIGHT_FOG, HIGH);
    digitalWrite(O_SPOT_LIGHT_SPOT, LOW);
  } else {
    digitalWrite(O_SPOT_LIGHT_FOG, HIGH);
    digitalWrite(O_SPOT_LIGHT_SPOT, HIGH);
  }

  if (boatData.lights.spotlight.up == N2kOnOff_On) {
    digitalWrite(O_SPOT_LIGHT_DOWN, HIGH);
    digitalWrite(O_SPOT_LIGHT_UP, LOW);
  } else if (boatData.lights.spotlight.down == N2kOnOff_On) {
    digitalWrite(O_SPOT_LIGHT_UP, HIGH);
    digitalWrite(O_SPOT_LIGHT_DOWN, LOW);
  } else {
    digitalWrite(O_SPOT_LIGHT_UP, HIGH);
    digitalWrite(O_SPOT_LIGHT_DOWN, HIGH);
  }

  if (boatData.lights.spotlight.left == N2kOnOff_On) {
    digitalWrite(O_SPOT_LIGHT_RIGHT, HIGH);
    digitalWrite(O_SPOT_LIGHT_LEFT, LOW);
  } else if (boatData.lights.spotlight.right == N2kOnOff_On) {
    digitalWrite(O_SPOT_LIGHT_LEFT, HIGH);
    digitalWrite(O_SPOT_LIGHT_RIGHT, LOW);
  } else {
    digitalWrite(O_SPOT_LIGHT_LEFT, HIGH);
    digitalWrite(O_SPOT_LIGHT_RIGHT, HIGH);
  }

}

void readSensors(BoatData &boatData) {
  double value;

  sensors.requestTemperatures();

  value = (double)sensors.getTempC(belowDeckTemperatureSensorID);

  boatData.environment.belowDeckTemperature = value;

}
