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

DallasTemperature sensors(&oneWire);

uint8_t engineRoomTemperatureSensorID[8] = { 0x28, 0xFF, 0x10, 0x05, 0x72, 0x17, 0x04, 0xF6 };
uint8_t portEngineTemperatureSensorID[8] = { 0x28, 0xFF, 0xD8, 0x17, 0x71, 0x17, 0x04, 0x31 };
uint8_t starboardEngineTemperatureSensorID[8] = { 0x28, 0xFF, 0x1F, 0x0B, 0x70, 0x17, 0x04, 0xC3 };

int portTilt[10] = {};
int starboardTilt[10] = {};

void setupIO() {

  pinMode(O_RELAY_1, OUTPUT);
  pinMode(O_WATER_PUMP, OUTPUT);
  pinMode(O_CABIN_HEATER_FAN, OUTPUT);
  pinMode(O_ENGINE_BILGE_PUMP, OUTPUT);
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
  pinMode(O_BILGE_BLOWER_2, OUTPUT);

  pinMode(O_TIP120_12, OUTPUT);
  pinMode(O_TIP120_13, OUTPUT);

  pinMode(I_PORT_ENGINE_TEMP, INPUT);
  pinMode(I_STARBOARD_ENGINE_TEMP, INPUT);
  pinMode(I_PORT_ENGINE_OIL, INPUT);
  pinMode(I_STARBOARD_ENGINE_OIL, INPUT);
  pinMode(I_FUEL, INPUT);

  pinMode(I_PORT_BATTERY, INPUT);
  pinMode(I_STARBOARD_BATTERY, INPUT);
  pinMode(I_AUX_BATTERY, INPUT);

  pinMode(I_ENGINE_BILGE_PUMP, INPUT);

  pinMode(I_PORT_DRIVE_TILT, INPUT);
  pinMode(I_STARBOARD_DRIVE_TILT, INPUT);

  sensors.begin();

}

bool readIO(BoatData &boatData) {
  static bool bilgePumpOn = false;
  bool newIO = false;
  double value;

  value = analogRead(I_ENGINE_BILGE_PUMP);
  if (value > 200) {
    boatData.bilgePumps.engineRoom.floatSwitch = N2kOnOff_On;
    if (!bilgePumpOn) {
      bilgePumpOn = true;
      newIO = true;
    }
  } else {
    boatData.bilgePumps.engineRoom.floatSwitch = N2kOnOff_Off;
    if (bilgePumpOn) {
      bilgePumpOn = false;
      newIO = true;
    }
  }

  return newIO;
}

void setIO(BoatData &boatData) {
  digitalWrite(O_RELAY_1, HIGH);
  digitalWrite(O_TIP120_12, LOW);
  digitalWrite(O_TIP120_13, LOW);

  digitalWrite(O_WATER_PUMP, !(boatData.utilities.waterPump == N2kOnOff_On));
  digitalWrite(O_CABIN_HEATER_FAN, !(boatData.utilities.cabinHeaterFan == N2kOnOff_On));
  digitalWrite(O_ENGINE_BILGE_PUMP, !(boatData.bilgePumps.engineRoom.on == N2kOnOff_On));
  digitalWrite(O_PORT_DRIVE_BOW_UP, !(boatData.tilt.port.bowUp == N2kOnOff_On));
  digitalWrite(O_PORT_DRIVE_BOW_DOWN, !(boatData.tilt.port.bowDown == N2kOnOff_On));
  digitalWrite(O_STARBOARD_DRIVE_BOW_UP, !(boatData.tilt.starboard.bowUp == N2kOnOff_On));
  digitalWrite(O_STARBOARD_DRIVE_BOW_DOWN, !(boatData.tilt.starboard.bowDown == N2kOnOff_On));

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
  digitalWrite(O_BILGE_BLOWER_2, boatData.blowers.two == N2kOnOff_On);

}

void readSensors(BoatData &boatData) {
  double value;

  sensors.requestTemperatures();

  value = (double)sensors.getTempC(portEngineTemperatureSensorID);

  value = readAtd(I_PORT_ENGINE_TEMP, 0, 538, 130, 0, 1.0);
  if (value < 0) {
    value = 0;
  }
  boatData.engines.port.waterTemperature = CToKelvin(value);

  value = (double)sensors.getTempC(starboardEngineTemperatureSensorID);

  value = readAtd(I_STARBOARD_ENGINE_TEMP, 0, 538, 130, 0, 1.0);
  if (value < 0) {
    value = 0;
  }

  boatData.engines.starboard.waterTemperature = CToKelvin(value);

  value = readAtd(I_PORT_ENGINE_OIL, 96, 330, 662390, 0, 1.0);
  if (value < 0) {
    value = 0;
  }
  boatData.engines.port.oilPressure = value;

  value = readAtd(I_STARBOARD_ENGINE_OIL, 96, 330, 662390, 0, 1.0);
  if (value < 0) {
    value = 0;
  }
  boatData.engines.starboard.oilPressure = value;

  value = (double)sensors.getTempC(engineRoomTemperatureSensorID);
  if (value < 0) {
    value = 0;
  }
  boatData.engines.engineRoomTemperature = CToKelvin(value);

  value = readAtd(I_FUEL, 275, 510, 100, 0, 1.0);
  if (value < 0) {
    value = 0;
  }
  boatData.fuel.level = value;

  value = readAtd(I_PORT_BATTERY, 0, 755, 0, 2000, 100.0);
  if (value < 0) {
    value = 0;
  }
  boatData.batteries.port = value;

  value = readAtd(I_STARBOARD_BATTERY, 0, 755, 0, 2000, 100.0);
  if (value < 0) {
    value = 0;
  }
  boatData.batteries.starboard = value;

  value = readAtd(I_AUX_BATTERY, 0, 750, 0, 2000, 100.0);
  if (value < 0) {
    value = 0;
  }
  boatData.batteries.auxiliary = value;

  value = readAtd(I_ENGINE_BILGE_PUMP, 0, 750, 0, 2000, 100.0);
  if (value> 200) {
    boatData.bilgePumps.engineRoom.on = N2kOnOff_On;
  } else {
    boatData.bilgePumps.engineRoom.on = N2kOnOff_Off;
  }

}

bool readTilt(BoatData &boatData) {
  static int count = 0;
  static double currentPortAngle = -15;
  static double currentStarboardAngle = -15;
  double portAngle;
  double starboardAngle;
  bool changed = false;

  portTilt[count] = analogRead(I_PORT_DRIVE_TILT);
  starboardTilt[count] = analogRead(I_STARBOARD_DRIVE_TILT);
  count+=1;
  if (count >= 10) {
    count = 0;
  }

  portAngle = (portTilt[0] + portTilt[1] + portTilt[2] + portTilt[3] + portTilt[4] + portTilt[5] + portTilt[6] + portTilt[7] + portTilt[8] + portTilt[9]) / 10.0;
  portAngle = (portAngle * 0.07042) - 10.61;
  portAngle = round(portAngle);
  if (portAngle < -9.0) {
    portAngle = -9.0;
  }
  boatData.tilt.port.angle = portAngle;

  starboardAngle = (starboardTilt[0] + starboardTilt[1] + starboardTilt[2] + starboardTilt[3] + starboardTilt[4] + starboardTilt[5] + starboardTilt[6] + starboardTilt[7] + starboardTilt[8] + starboardTilt[9]) / 10.0;
  starboardAngle = (starboardAngle * 0.07042) - 10.61;
  starboardAngle = round(starboardAngle);
  if (starboardAngle < -9.0) {
    starboardAngle = -9.0;
  }
  boatData.tilt.starboard.angle = starboardAngle;

  if ((currentPortAngle != portAngle) || (currentStarboardAngle != starboardAngle)) {
    currentPortAngle = portAngle;
    currentStarboardAngle = starboardAngle;
    changed = true;
  }
  return changed;
}
