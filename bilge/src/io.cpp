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
  pinMode(O_REFRIGERATOR, OUTPUT);

  pinMode(O_SPOT_LIGHT_SPOT, OUTPUT);
  pinMode(O_SPOT_LIGHT_FOG, OUTPUT);
  pinMode(O_SPOT_LIGHT_UP, OUTPUT);
  pinMode(O_SPOT_LIGHT_DOWN, OUTPUT);
  pinMode(O_SPOT_LIGHT_LEFT, OUTPUT);
  pinMode(O_SPOT_LIGHT_RIGHT, OUTPUT);
  pinMode(O_MIDSHIP_BILGE, OUTPUT);
  pinMode(O_BLACKWATER_AIR_PUMP, OUTPUT);

  pinMode(O_REALY_2_1, OUTPUT);
  pinMode(O_REALY_2_2, OUTPUT);
  pinMode(O_REALY_2_3, OUTPUT);
  pinMode(O_REALY_2_4, OUTPUT);
  pinMode(O_REALY_2_5, OUTPUT);

  pinMode(O_BOW_THRUSTER_POWER, OUTPUT);
  pinMode(O_BOW_THRUSTER_PORT, OUTPUT);
  pinMode(O_BOW_THRUSTER_STARBOARD, OUTPUT);

  digitalWrite(O_MIDSHIP_BILDGE_LIGHTS, LOW);
  digitalWrite(O_SHOWER_PUMP, LOW);
  digitalWrite(O_MACERATOR, LOW);
  digitalWrite(O_REFRIGERATOR, LOW);

  digitalWrite(O_SPOT_LIGHT_SPOT, HIGH);
  digitalWrite(O_SPOT_LIGHT_FOG, HIGH);
  digitalWrite(O_SPOT_LIGHT_UP, HIGH);
  digitalWrite(O_SPOT_LIGHT_DOWN, HIGH);
  digitalWrite(O_SPOT_LIGHT_LEFT, HIGH);
  digitalWrite(O_SPOT_LIGHT_RIGHT, HIGH);
  digitalWrite(O_MIDSHIP_BILGE, HIGH);
  digitalWrite(O_BLACKWATER_AIR_PUMP, HIGH);

  digitalWrite(O_REALY_2_1, HIGH);
  digitalWrite(O_REALY_2_2, HIGH);
  digitalWrite(O_REALY_2_3, HIGH);
  digitalWrite(O_REALY_2_4, HIGH);
  digitalWrite(O_REALY_2_5, HIGH);

  digitalWrite(O_BOW_THRUSTER_POWER, HIGH);
  digitalWrite(O_BOW_THRUSTER_PORT, HIGH);
  digitalWrite(O_BOW_THRUSTER_STARBOARD, HIGH);

  pinMode(I_MIDSHIP_BILGE, INPUT);

  sensors.begin();
}

bool readIO(BoatData &boatData, SwitchBankInstance instance) {
  bool newIO = false;
  double value;

  if (instance == E_BILGE_PUMPS) {
    value = analogRead(I_MIDSHIP_BILGE);
    if ((value > 250) && (boatData.bilgePumps.midship.floatSwitch != N2kOnOff_On)) {
      newIO = true;
      boatData.bilgePumps.midship.floatSwitch = N2kOnOff_On;
    }
    if ((value < 100) && (boatData.bilgePumps.midship.floatSwitch != N2kOnOff_Off)) {
      newIO = true;
      boatData.bilgePumps.midship.floatSwitch = N2kOnOff_Off;
    }
  }

  return newIO;
}

void setIO(BoatData &boatData, SwitchBankInstance instance) {

  if (instance == E_IGNITION_START) {

  } else if (instance == E_ENGINE_STATUS) {

  } else if (instance == E_POWER_TRIM) {

  } else if (instance == E_TRIM) {

  } else if (instance == E_NUTRASALT) {

  } else if (instance == E_LIGHTS) {
    digitalWrite(O_MIDSHIP_BILDGE_LIGHTS, boatData.lights.belowDeck == N2kOnOff_On);

  } else if (instance == E_SPOTLIGHT) {

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

  } else if (instance == E_UTILITIES_CABIN) {

  } else if (instance == E_UTILITIES_BILGE) {
    digitalWrite(O_SHOWER_PUMP, boatData.utilities.showerDrainPump == N2kOnOff_On);
    digitalWrite(O_MACERATOR, boatData.utilities.macerator == N2kOnOff_On);
    digitalWrite(O_BLACKWATER_AIR_PUMP, !(boatData.utilities.blackwaterAirPump == N2kOnOff_On));
    digitalWrite(O_REFRIGERATOR, boatData.utilities.refrigerator == N2kOnOff_On);

  } else if (instance == E_UTILITIES_ENGINE_ROOM) {

  } else if (instance == E_BILGE_PUMPS) {
    digitalWrite(O_MIDSHIP_BILGE, !(boatData.bilgePumps.midship.on == N2kOnOff_On));

  }

}

void setThruster(BoatData &boatData) {
  if (boatData.engines.bowThruster.power == N2kOnOff_On) {
    digitalWrite(O_BOW_THRUSTER_POWER, LOW);
    if (boatData.engines.bowThruster.toPort == N2kOnOff_On) {
      digitalWrite(O_BOW_THRUSTER_PORT, LOW);
      digitalWrite(O_BOW_THRUSTER_STARBOARD, HIGH);
    } else if (boatData.engines.bowThruster.toStarboard == N2kOnOff_On) {
      digitalWrite(O_BOW_THRUSTER_PORT, HIGH);
      digitalWrite(O_BOW_THRUSTER_STARBOARD, LOW);
    } else {
      digitalWrite(O_BOW_THRUSTER_PORT, HIGH);
      digitalWrite(O_BOW_THRUSTER_STARBOARD, HIGH);
    }
  } else {
    digitalWrite(O_BOW_THRUSTER_POWER, HIGH);
    digitalWrite(O_BOW_THRUSTER_PORT, HIGH);
    digitalWrite(O_BOW_THRUSTER_STARBOARD, HIGH);
  }
}

void readSensors(BoatData &boatData) {
  double value;

  sensors.requestTemperatures();

  value = (double)sensors.getTempC(belowDeckTemperatureSensorID);

  boatData.environment.belowDeckTemperature = value;

}
