#include <Arduino.h>
#include <Bounce2.h>
#include <N2kTypes.h>
#include <N2kMessages.h>
#include "BoatData.h"
#include <io.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "utils.h"

OneWire oneWire(I_ONE_WIRE_BUS);

#define BOUNCE_INTERVAL 25

enum debounce {
  E_DOOR_LOCK,
  E_PORT_ENGINE_CHECK,
  E_PORT_ENGINE_MALFUNCTION,
  E_STARBOARD_ENGINE_CHECK,
  E_STARBOARD_ENGINE_MALFUNCTION,
  E_ITEMS
};

Bounce * buttons = new Bounce[E_ITEMS];

DallasTemperature sensors(&oneWire);

uint8_t cabinTemperatureSensorID[8] = { 0x28, 0xFF, 0x10, 0x05, 0x72, 0x17, 0x04, 0xF6 };
uint8_t engineTemperatureBottomSensorID[8] = { 0x28, 0xFF, 0x1F, 0x0B, 0x70, 0x17, 0x04, 0xC3 };
uint8_t engineTemperatureTopSensorID[8] = { 0x28, 0xFF, 0xD8, 0x17, 0x71, 0x17, 0x04, 0x31 };

void setupIO() {

  pinMode(O_PORT_NUTRASALT, OUTPUT);
  pinMode(O_STARBOARD_NUTRASALT, OUTPUT);
  pinMode(O_IGNITION, OUTPUT);
  pinMode(O_RELAY_4, OUTPUT);
  pinMode(O_PORT_DRIVE_BOW_UP, OUTPUT);
  pinMode(O_PORT_DRIVE_BOW_DOWN, OUTPUT);
  pinMode(O_STARBOARD_DRIVE_BOW_UP, OUTPUT);
  pinMode(O_STARBOARD_DRIVE_BOW_DOWN, OUTPUT);

  pinMode(O_ENGINE_ROOM_LIGHTS, OUTPUT);
  pinMode(O_AFT_DECK_LIGHTS, OUTPUT);
  pinMode(O_STERN_LIGHT, OUTPUT);
  pinMode(O_DOOR_LOCK, OUTPUT);

  pinMode(O_PORT_IGNITION, OUTPUT);
  pinMode(O_PORT_START, OUTPUT);
  pinMode(O_STARBOARD_IGNITION, OUTPUT);
  pinMode(O_STARBOARD_START, OUTPUT);

  pinMode(O_PORT_TRIM, OUTPUT);
  pinMode(O_STARBOARD_TRIM, OUTPUT);
  pinMode(O_TRIM_BOW_UP, OUTPUT);
  pinMode(O_TRIM_BOW_DOWN, OUTPUT);

  pinMode(O_BILGE_BLOWER_1, OUTPUT);
  pinMode(O_CABIN_HEATER_FAN, OUTPUT);
  pinMode(O_WATER_PUMP, OUTPUT);
  pinMode(O_ENGINE_BILGE_PUMP, OUTPUT);

  pinMode(O_TIP120_13, OUTPUT);

  pinMode(I_PORT_ENGINE_OIL, INPUT);
  pinMode(I_PORT_ENGINE_TEMP, INPUT);
  pinMode(I_PORT_DRIVE_TRIM, INPUT);
  buttons[E_PORT_ENGINE_CHECK].attach(I_PORT_ENGINE_MALFUNCTION, INPUT_PULLUP);
  buttons[E_PORT_ENGINE_CHECK].attach(I_PORT_ENGINE_CHECK, INPUT_PULLUP);

  pinMode(I_STARBOARD_ENGINE_OIL, INPUT);
  pinMode(I_STARBOARD_ENGINE_TEMP, INPUT);
  pinMode(I_STARBOARD_DRIVE_TRIM, INPUT);
  buttons[E_STARBOARD_ENGINE_CHECK].attach(I_STARBOARD_ENGINE_MALFUNCTION, INPUT_PULLUP);
  buttons[E_STARBOARD_ENGINE_CHECK].attach(I_STARBOARD_ENGINE_CHECK, INPUT_PULLUP);

  pinMode(I_FUEL, INPUT);
  pinMode(I_STEERING_ANGLE, INPUT);
  pinMode(I_ENGINE_BILGE_PUMP, INPUT);

  pinMode(I_AUX_BATTERY, INPUT);
  pinMode(I_PORT_BATTERY, INPUT);
  pinMode(I_STARBOARD_BATTERY, INPUT);

  buttons[E_DOOR_LOCK].attach(I_DOOR_LOCK, INPUT_PULLUP);

  for (int e = 0; e < E_ITEMS; e++) {
    buttons[e].interval(BOUNCE_INTERVAL);
  }

  sensors.begin();

}

bool readIO(BoatData &boatData, SwitchBankInstance instance) {
  bool newIO = false;
  double value;

  if (instance == E_ENGINE_STATUS) {
    newIO |= readMomentaryButton(buttons[E_PORT_ENGINE_CHECK], boatData.engines.port.check, true);
    newIO |= readMomentaryButton(buttons[E_PORT_ENGINE_MALFUNCTION], boatData.engines.port.malfunction, true);
    newIO |= readMomentaryButton(buttons[E_STARBOARD_ENGINE_CHECK], boatData.engines.starboard.check, true);
    newIO |= readMomentaryButton(buttons[E_STARBOARD_ENGINE_MALFUNCTION], boatData.engines.starboard.malfunction, true);

  } else if (instance == E_UTILITIES_ENGINE_ROOM) {
    newIO |= readToggleButton(buttons[E_DOOR_LOCK], boatData.utilities.doorLock);
    if (newIO) {
      Serial.print("Lock ");
      Serial.println(boatData.utilities.doorLock);
    }

  } else if (instance == E_BILGE_PUMPS) {
    value = analogRead(I_ENGINE_BILGE_PUMP);
    if ((value > 600) && (boatData.bilgePumps.engineRoom.floatSwitch != N2kOnOff_On)) {
      newIO = true;
      boatData.bilgePumps.engineRoom.floatSwitch = N2kOnOff_On;
    }
    if ((value < 100) && (boatData.bilgePumps.engineRoom.floatSwitch != N2kOnOff_Off)) {
      newIO = true;
      boatData.bilgePumps.engineRoom.floatSwitch = N2kOnOff_Off;
    }

  }

  return newIO;
}

void setIO(BoatData &boatData, SwitchBankInstance instance) {
  digitalWrite(O_RELAY_4, HIGH);
  digitalWrite(O_IGNITION, HIGH);
  digitalWrite(O_TIP120_13, LOW);

  if (instance == E_IGNITION_START) {
    digitalWrite(O_PORT_IGNITION, boatData.engines.port.ignition == N2kOnOff_On);
    digitalWrite(O_PORT_START, boatData.engines.port.start == N2kOnOff_On);
    digitalWrite(O_STARBOARD_IGNITION, boatData.engines.starboard.ignition == N2kOnOff_On);
    digitalWrite(O_STARBOARD_START, boatData.engines.starboard.start == N2kOnOff_On);

  } else if (instance == E_ENGINE_STATUS) {

  } else if (instance == E_POWER_TRIM) {
    digitalWrite(O_PORT_DRIVE_BOW_UP, !(boatData.engines.port.trim.bowUp == N2kOnOff_On));
    digitalWrite(O_PORT_DRIVE_BOW_DOWN, !(boatData.engines.port.trim.bowDown == N2kOnOff_On));
    digitalWrite(O_STARBOARD_DRIVE_BOW_UP, !(boatData.engines.starboard.trim.bowUp == N2kOnOff_On));
    digitalWrite(O_STARBOARD_DRIVE_BOW_DOWN, !(boatData.engines.starboard.trim.bowDown == N2kOnOff_On));

  } else if (instance == E_TRIM) {
    if ((boatData.trim.port.bowUp == N2kOnOff_On) || (boatData.trim.starboard.bowUp == N2kOnOff_On)) {
      digitalWrite(O_TRIM_BOW_UP, HIGH);
      digitalWrite(O_TRIM_BOW_DOWN, LOW);
    } else if ((boatData.trim.port.bowDown == N2kOnOff_On) || (boatData.trim.starboard.bowDown == N2kOnOff_On)) {
      digitalWrite(O_TRIM_BOW_UP, LOW);
      digitalWrite(O_TRIM_BOW_DOWN, HIGH);
    } else {
      digitalWrite(O_TRIM_BOW_UP, LOW);
      digitalWrite(O_TRIM_BOW_DOWN, LOW);
    }

    if ((boatData.trim.port.bowUp == N2kOnOff_On) || (boatData.trim.port.bowDown == N2kOnOff_On)) {
      digitalWrite(O_PORT_TRIM, HIGH);
    } else {
      digitalWrite(O_PORT_TRIM, LOW);
    }

    if ((boatData.trim.starboard.bowUp == N2kOnOff_On) || (boatData.trim.starboard.bowDown == N2kOnOff_On)) {
      digitalWrite(O_STARBOARD_TRIM, HIGH);
    } else {
      digitalWrite(O_STARBOARD_TRIM, LOW);
    }

  } else if (instance == E_NUTRASALT) {
    digitalWrite(O_PORT_NUTRASALT, !(boatData.engines.port.nutraSalt == N2kOnOff_On));
    digitalWrite(O_STARBOARD_NUTRASALT, !(boatData.engines.starboard.nutraSalt == N2kOnOff_On));

  } else if (instance == E_LIGHTS) {
    digitalWrite(O_ENGINE_ROOM_LIGHTS, boatData.lights.engineRoom == N2kOnOff_On);
    digitalWrite(O_AFT_DECK_LIGHTS, boatData.lights.aftDeck == N2kOnOff_On);
    digitalWrite(O_STERN_LIGHT, boatData.lights.stern == N2kOnOff_On);

  } else if (instance == E_SPOTLIGHT) {

  } else if (instance == E_UTILITIES_CABIN) {
    digitalWrite(O_CABIN_HEATER_FAN, boatData.utilities.cabinHeaterFan == N2kOnOff_On);

  } else if (instance == E_UTILITIES_BILGE) {

  } else if (instance == E_UTILITIES_ENGINE_ROOM) {
    digitalWrite(O_WATER_PUMP, boatData.utilities.waterPump == N2kOnOff_On);
    digitalWrite(O_BILGE_BLOWER_1, boatData.blower == N2kOnOff_On);
    digitalWrite(O_DOOR_LOCK, boatData.utilities.doorLock == N2kOnOff_On);

  } else if (instance == E_BILGE_PUMPS) {
    digitalWrite(O_ENGINE_BILGE_PUMP, boatData.bilgePumps.engineRoom.on == N2kOnOff_On);

  }
}

void readSensors(BoatData &boatData) {
  double value;
  // int atdValue;

  sensors.requestTemperatures();

  value = (double)sensors.getTempC(engineTemperatureBottomSensorID);
  boatData.engines.engineRoomTemperatureBottom = CToKelvin(value);

  value = (double)sensors.getTempC(engineTemperatureTopSensorID);
  boatData.engines.engineRoomTemperatureTop = CToKelvin(value);

  value = (double)sensors.getTempC(cabinTemperatureSensorID);
  boatData.environment.insideTemperature = CToKelvin(value);

  value = readAtd(I_PORT_BATTERY, 0, 1023, 0, 1555, 100.0);
  boatData.batteries.port = value;

  value = readAtd(I_STARBOARD_BATTERY, 0, 1023, 0, 1558, 100.0);
  boatData.batteries.starboard = value;

  value = readAtd(I_AUX_BATTERY, 0, 1023, 0, 1555, 100.0);
  boatData.batteries.auxiliary = value;

  // Serial.println("");
  // Serial.println("============");
  // Serial.print("I_PORT_ENGINE_TEMP ");
  // Serial.print(analogRead(I_PORT_ENGINE_TEMP));
  // Serial.print(" ");
  value = readAtd(I_PORT_ENGINE_TEMP, 0, 560, 90, 0, 1.0);
  // Serial.println(value);
  boatData.engines.port.waterTemperature = CToKelvin(value);
  // Serial.print("I_PORT_ENGINE_OIL ");
  // Serial.print(analogRead(I_PORT_ENGINE_OIL));
  // Serial.print(" ");
  value = readAtd(I_PORT_ENGINE_OIL, 0, 122, 822, 0, 1.0);
  // Serial.println(value);
  boatData.engines.port.oilPressure = value;
  //
  // Serial.print("I_PORT_DRIVE_TRIM ");
  // Serial.print(analogRead(I_PORT_DRIVE_TRIM));
  // Serial.print(" ");
  value = readAtd(I_PORT_DRIVE_TRIM, 0, 80, 0, 40, 1.0);
  // Serial.println(value);
  boatData.engines.port.trim.angle = value;

  // Serial.print("I_PORT_ENGINE_ALARM ");
  // Serial.println(digitalRead(I_PORT_ENGINE_MALFUNCTION));
  // Serial.print("I_PORT_ENGINE_CHECK_LIGHT ");
  // Serial.println(digitalRead(I_PORT_ENGINE_CHECK));
  //
  // Serial.println("");
  // Serial.print("I_STARBOARD_ENGINE_TEMP ");
  // Serial.print(" ");
  // Serial.print(analogRead(I_STARBOARD_ENGINE_TEMP));
  // Serial.print(" ");
  value = readAtd(I_STARBOARD_ENGINE_TEMP, 0, 560, 90, 0, 1.0);
  // Serial.println(value);
  boatData.engines.starboard.waterTemperature = CToKelvin(value);
  //
  // I_PORT_ENGINE_TEMP 311 - 346    26 336   36 342   41   343      57  178
  // I_PORT_ENGINE_OIL 203 - 223     652  24     620 30
  // I_PORT_DRIVE_TRIM 832 - 921     54 26     19  44
  // I_PORT_ENGINE_ALARM 1
  // I_PORT_ENGINE_CHECK_LIGHT 1
    //

    // I_STARBOARD_ENGINE_TEMP 340    22 304    37  349    45   331     72  117
    // I_STARBOARD_ENGINE_OIL 218    748 11    668   15
    // I_STARBOARD_DRIVE_TRIM 889    14  6    21  41
    // I_STARBOARD_ENGINE_ALARM 1
    // I_STARBOARD_ENGINE_CHECK_LIGHT 1


  // Serial.print("I_STARBOARD_ENGINE_OIL ");
  // Serial.print(analogRead(I_STARBOARD_ENGINE_OIL));
  // Serial.print(" ");
  value = readAtd(I_STARBOARD_ENGINE_OIL, 0, 122, 822, 0, 1.0);
  // Serial.println(value);
  boatData.engines.starboard.oilPressure = value;
  //

  value = readAtd(I_FUEL, 33, 201, 0, 100, 1.0);
  value = (12.85/boatData.batteries.auxiliary) * value;
  if (value > 100.0) {
    value = 100.0;
  }

  boatData.fuel.level = value;

  // Serial.print("I_STARBOARD_DRIVE_TRIM ");
  // Serial.print(analogRead(I_STARBOARD_DRIVE_TRIM));
  // Serial.print(" ");
  value = readAtd(I_STARBOARD_DRIVE_TRIM, 0, 80, 0, 40, 1.0);
  // Serial.println(value);
  boatData.engines.starboard.trim.angle = value;



  // Serial.print("I_STARBOARD_ENGINE_ALARM ");
  // Serial.println(digitalRead(I_STARBOARD_ENGINE_MALFUNCTION));
  // Serial.print("I_STARBOARD_ENGINE_CHECK_LIGHT ");
  // Serial.println(digitalRead(I_STARBOARD_ENGINE_CHECK));

  value = readAtd(I_STEERING_ANGLE, 8, 198, 7854, -7854, 10000.0);
  boatData.engines.port.steering = value;

}
