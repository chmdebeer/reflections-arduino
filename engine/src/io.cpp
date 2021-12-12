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

Bounce doorLock = Bounce();

DallasTemperature sensors(&oneWire);

uint8_t cabinTemperatureSensorID[8] = { 0x28, 0xFF, 0x10, 0x05, 0x72, 0x17, 0x04, 0xF6 };
uint8_t engineTemperatureBottomSensorID[8] = { 0x28, 0xFF, 0xD8, 0x17, 0x71, 0x17, 0x04, 0x31 };
uint8_t engineTemperatureTopSensorID[8] = { 0x28, 0xFF, 0x1F, 0x0B, 0x70, 0x17, 0x04, 0xC3 };

void setupIO() {

  pinMode(O_PORT_NUTRASALT, OUTPUT);
  pinMode(O_STARBOARD_NUTRASALT, OUTPUT);
  pinMode(O_RELAY_3, OUTPUT);
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
  pinMode(I_PORT_DRIVE_TILT, INPUT);
  pinMode(I_PORT_ENGINE_ALARM, INPUT_PULLUP);
  pinMode(I_PORT_ENGINE_CHECK_LIGHT, INPUT_PULLUP);

  pinMode(I_STARBOARD_ENGINE_OIL, INPUT);
  pinMode(I_STARBOARD_ENGINE_TEMP, INPUT);
  pinMode(I_STARBOARD_DRIVE_TILT, INPUT);
  pinMode(I_STARBOARD_ENGINE_ALARM, INPUT_PULLUP);
  pinMode(I_STARBOARD_ENGINE_CHECK_LIGHT, INPUT_PULLUP);

  pinMode(I_FUEL, INPUT);
  pinMode(I_STEERING_ANGLE, INPUT);
  pinMode(I_ENGINE_BILGE_PUMP, INPUT);

  pinMode(I_AUX_BATTERY, INPUT);
  pinMode(I_PORT_BATTERY, INPUT);
  pinMode(I_STARBOARD_BATTERY, INPUT);

  pinMode(I_DOOR_LOCK, INPUT_PULLUP);
  doorLock.attach(I_DOOR_LOCK);
  doorLock.interval(25); // interval in ms

  sensors.begin();

}

bool readIO(BoatData &boatData) {
  bool newIO = false;
  double value;

  doorLock.update();
  if (doorLock.fell()) {
    newIO |= true;
    if (boatData.utilities.doorLock == N2kOnOff_Off) {
      boatData.utilities.doorLock = N2kOnOff_On;
    } else {
      boatData.utilities.doorLock = N2kOnOff_Off;
    }
  }

  value = analogRead(I_ENGINE_BILGE_PUMP);
  if ((value > 600) && (boatData.bilgePumps.engineRoom.floatSwitch != N2kOnOff_On)) {
    boatData.bilgePumps.engineRoom.floatSwitch = N2kOnOff_On;
    newIO |= true;
    Serial.println("pump on");
  }
  if ((value < 100) && (boatData.bilgePumps.engineRoom.floatSwitch != N2kOnOff_Off)) {
    boatData.bilgePumps.engineRoom.floatSwitch = N2kOnOff_Off;
    newIO |= true;
    Serial.println("pump off");
  }

  return newIO;
}

void setIO(BoatData &boatData) {
  digitalWrite(O_PORT_NUTRASALT, !(boatData.engines.port.nutraSalt == N2kOnOff_On));
  digitalWrite(O_STARBOARD_NUTRASALT, !(boatData.engines.starboard.nutraSalt == N2kOnOff_On));
  digitalWrite(O_RELAY_3, HIGH);
  digitalWrite(O_RELAY_4, HIGH);

  digitalWrite(O_TIP120_13, LOW);

  digitalWrite(O_PORT_DRIVE_BOW_UP, !(boatData.engines.port.tilt.bowUp == N2kOnOff_On));
  digitalWrite(O_PORT_DRIVE_BOW_DOWN, !(boatData.engines.port.tilt.bowDown == N2kOnOff_On));
  digitalWrite(O_STARBOARD_DRIVE_BOW_UP, !(boatData.engines.starboard.tilt.bowUp == N2kOnOff_On));
  digitalWrite(O_STARBOARD_DRIVE_BOW_DOWN, !(boatData.engines.starboard.tilt.bowDown == N2kOnOff_On));

  digitalWrite(O_ENGINE_ROOM_LIGHTS, boatData.lights.engineRoom == N2kOnOff_On);
  digitalWrite(O_AFT_DECK_LIGHTS, boatData.lights.aftDeck == N2kOnOff_On);
  digitalWrite(O_STERN_LIGHT, boatData.lights.stern == N2kOnOff_On);
  digitalWrite(O_DOOR_LOCK, boatData.utilities.doorLock == N2kOnOff_On);

  digitalWrite(O_PORT_IGNITION, boatData.engines.port.ignition == N2kOnOff_On);
  digitalWrite(O_PORT_START, boatData.engines.port.start == N2kOnOff_On);
  digitalWrite(O_STARBOARD_IGNITION, boatData.engines.starboard.ignition == N2kOnOff_On);
  digitalWrite(O_STARBOARD_START, boatData.engines.starboard.start == N2kOnOff_On);

  if (boatData.trim.port.bowUp == N2kOnOff_On) {
    digitalWrite(O_PORT_TRIM, HIGH);
    digitalWrite(O_STARBOARD_TRIM, LOW);
    digitalWrite(O_TRIM_BOW_UP, HIGH);
    digitalWrite(O_TRIM_BOW_DOWN, LOW);
  } else if (boatData.trim.port.bowDown == N2kOnOff_On) {
    digitalWrite(O_PORT_TRIM, HIGH);
    digitalWrite(O_STARBOARD_TRIM, LOW);
    digitalWrite(O_TRIM_BOW_UP, LOW);
    digitalWrite(O_TRIM_BOW_DOWN, HIGH);
  } else if (boatData.trim.starboard.bowUp == N2kOnOff_On) {
    digitalWrite(O_PORT_TRIM, LOW);
    digitalWrite(O_STARBOARD_TRIM, HIGH);
    digitalWrite(O_TRIM_BOW_UP, HIGH);
    digitalWrite(O_TRIM_BOW_DOWN, LOW);
  } else if (boatData.trim.starboard.bowDown == N2kOnOff_On) {
    digitalWrite(O_PORT_TRIM, LOW);
    digitalWrite(O_STARBOARD_TRIM, HIGH);
    digitalWrite(O_TRIM_BOW_UP, LOW);
    digitalWrite(O_TRIM_BOW_DOWN, HIGH);
  } else {
    digitalWrite(O_PORT_TRIM, LOW);
    digitalWrite(O_STARBOARD_TRIM, LOW);
    digitalWrite(O_TRIM_BOW_UP, LOW);
    digitalWrite(O_TRIM_BOW_DOWN, LOW);
  }

  digitalWrite(O_BILGE_BLOWER_1, boatData.blowers.one == N2kOnOff_On);
  digitalWrite(O_CABIN_HEATER_FAN, boatData.utilities.cabinHeaterFan == N2kOnOff_On);
  digitalWrite(O_WATER_PUMP, boatData.utilities.waterPump == N2kOnOff_On);
  digitalWrite(O_ENGINE_BILGE_PUMP, boatData.bilgePumps.engineRoom.on == N2kOnOff_On);

}

void readSensors(BoatData &boatData) {
  double value;
  int atdValue;

  sensors.requestTemperatures();

  value = (double)sensors.getTempC(engineTemperatureBottomSensorID);
  boatData.engines.engineRoomTemperatureBottom = CToKelvin(value);

  value = (double)sensors.getTempC(engineTemperatureTopSensorID);
  boatData.engines.engineRoomTemperatureTop = CToKelvin(value);

  value = (double)sensors.getTempC(cabinTemperatureSensorID);
  boatData.environment.insideTemperature = CToKelvin(value);

  Serial.println("");
  Serial.println("============");
  Serial.print("I_PORT_ENGINE_TEMP ");
  Serial.print(analogRead(I_PORT_ENGINE_TEMP));
  Serial.print(" ");
  value = readAtd(I_PORT_ENGINE_TEMP, 0, 560, 90, 0, 1.0);
  Serial.println(value);
  boatData.engines.port.waterTemperature = CToKelvin(value);
  Serial.print("I_PORT_ENGINE_OIL ");
  Serial.print(analogRead(I_PORT_ENGINE_OIL));
  Serial.print(" ");
  value = readAtd(I_PORT_ENGINE_OIL, 0, 122, 822, 0, 1.0);
  Serial.println(value);
  boatData.engines.port.oilPressure = value;
  //
  Serial.print("I_PORT_DRIVE_TILT ");
  Serial.print(analogRead(I_PORT_DRIVE_TILT));
  Serial.print(" ");
  value = readAtd(I_PORT_DRIVE_TILT, 0, 80, 0, 40, 1.0);
  Serial.println(value);
  boatData.engines.port.tilt.angle = value;

  Serial.print("I_PORT_ENGINE_ALARM ");
  Serial.println(digitalRead(I_PORT_ENGINE_ALARM));
  Serial.print("I_PORT_ENGINE_CHECK_LIGHT ");
  Serial.println(digitalRead(I_PORT_ENGINE_CHECK_LIGHT));
  //
  Serial.println("");
  Serial.print("I_STARBOARD_ENGINE_TEMP ");
  Serial.print(" ");
  Serial.print(analogRead(I_STARBOARD_ENGINE_TEMP));
  Serial.print(" ");
  value = readAtd(I_STARBOARD_ENGINE_TEMP, 0, 560, 90, 0, 1.0);
  Serial.println(value);
  boatData.engines.starboard.waterTemperature = CToKelvin(value);
  //
  // I_PORT_ENGINE_TEMP 311 - 346    26 336   36 342   41   343      57  178
  // I_PORT_ENGINE_OIL 203 - 223     652  24     620 30
  // I_PORT_DRIVE_TILT 832 - 921     54 26     19  44
  // I_PORT_ENGINE_ALARM 1
  // I_PORT_ENGINE_CHECK_LIGHT 1
    //

    // I_STARBOARD_ENGINE_TEMP 340    22 304    37  349    45   331     72  117
    // I_STARBOARD_ENGINE_OIL 218    748 11    668   15
    // I_STARBOARD_DRIVE_TILT 889    14  6    21  41
    // I_STARBOARD_ENGINE_ALARM 1
    // I_STARBOARD_ENGINE_CHECK_LIGHT 1


  Serial.print("I_STARBOARD_ENGINE_OIL ");
  Serial.print(analogRead(I_STARBOARD_ENGINE_OIL));
  Serial.print(" ");
  value = readAtd(I_STARBOARD_ENGINE_OIL, 0, 122, 822, 0, 1.0);
  Serial.println(value);
  boatData.engines.starboard.oilPressure = value;
  //

  atdValue = analogRead(I_FUEL);
  // value = readAtd(I_FUEL, 38, 222, 100, 0, 1.0);
  value = readAtd(I_FUEL, 45, 900, 100, 0, 1.0);
  boatData.fuel.level = value;

  value = readAtd(I_PORT_BATTERY, 0, 1023, 0, 1555, 100.0);
  boatData.batteries.port = value;

  value = readAtd(I_STARBOARD_BATTERY, 0, 1023, 0, 1558, 100.0);
  boatData.batteries.starboard = value;

  value = readAtd(I_AUX_BATTERY, 0, 1023, 0, 1555, 100.0);
  boatData.batteries.auxiliary = value;


  Serial.print("I_STARBOARD_DRIVE_TILT ");
  Serial.print(analogRead(I_STARBOARD_DRIVE_TILT));
  Serial.print(" ");
  value = readAtd(I_STARBOARD_DRIVE_TILT, 0, 80, 0, 40, 1.0);
  Serial.println(value);
  boatData.engines.starboard.tilt.angle = value;



  Serial.print("I_STARBOARD_ENGINE_ALARM ");
  Serial.println(digitalRead(I_STARBOARD_ENGINE_ALARM));
  Serial.print("I_STARBOARD_ENGINE_CHECK_LIGHT ");
  Serial.println(digitalRead(I_STARBOARD_ENGINE_CHECK_LIGHT));

  value = readAtd(I_STEERING_ANGLE, 8, 198, 7854, -7854, 10000.0);
  boatData.engines.port.steering = value;

}
