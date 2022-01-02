#include <Arduino.h>
#include <EEPROM.h>
#include <N2kTypes.h>
#include "boatdata.h"
#include "io.h"

#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>
#include <N2kReflections.h>
#include <N2kMsg.h>
#include <NMEA2000.h>

#include <main.h>

#include <Bounce2.h>
#include <timer.h>
#include <timerManager.h>

#include <frequency.h>
#include "utils.h"

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {127488L, &handleEngineRPM},
  {127489L, &handleEngineDynamicParameters},
  {127501L, &handleBinaryStatus},
  {59904L, &handleAddressClaim},
  {0,0}
};

enum timers {
  T_NEW_DEVICE,
  T_READ_SENSOR_DATA,
  T_SEND_SENSOR_DATA,
  T_SEND_STEERING_DATA,
  T_ENGINE,
  T_SYSTEM,
  T_ITEMS
};

Timer * timers = new Timer[T_ITEMS];

unsigned long portEngineRpmTime = 0;
unsigned long starboardEngineRpmTime = 0;

void setup() {
  unsigned char instance;

  Serial.begin(115200);

  clearBoatData(boatData);
  for (instance=1; instance < (unsigned char)E_SWITCH_BANK_INSTANCES; instance++) {
    setIO(boatData, (SwitchBankInstance)instance);
  }
  setupIO();
  setupTimers();
  setupNMEA();

  setupFrequency();

  readRestartCount();
}

void loop() {
  NMEA2000.ParseMessages();

  loopFrequency(boatData);

  if (readIO(boatData, E_ENGINE_STATUS)) {
    n2kBinaryStatus(E_ENGINE_STATUS);
  }

  if (readIO(boatData, E_UTILITIES_ENGINE_ROOM)) {
    n2kBinaryStatus(E_UTILITIES_ENGINE_ROOM);
  }

  TimerManager::instance().update();

  if ( Serial.available() ) { Serial.read(); }
}

void setupNMEA() {

  NMEA2000.SetProductInformation("10000101", 101, "Engine Room", "1.0.1 (2019-05-26)", "1.0.1 (2019-05-26)");

  NMEA2000.SetDeviceInformation(100101, 132, 25, 2041);

  NMEA2000.SetForwardStream(&Serial);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);
  NMEA2000.EnableForward(false);

  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 11);

  NMEA2000.SetMsgHandler(handleNMEA2000Msg);

  NMEA2000.SetHeartbeatInterval(59300);

  NMEA2000.Open();
}

void setupTimers() {
  timers[T_NEW_DEVICE].setInterval(11000, 1);
  timers[T_NEW_DEVICE].setCallback(newDevice);

  timers[T_READ_SENSOR_DATA].setInterval(5007);
  timers[T_READ_SENSOR_DATA].setCallback(readSensors);

  timers[T_SEND_SENSOR_DATA].setInterval(1003);
  timers[T_SEND_SENSOR_DATA].setCallback(sendN2kSensorData);

  timers[T_SEND_STEERING_DATA].setInterval(1119);
  timers[T_SEND_STEERING_DATA].setCallback(sendN2kSteeringData);

  timers[T_ENGINE].setInterval(2033);
  timers[T_ENGINE].setCallback(sendN2kEngineRPM);

  timers[T_SYSTEM].setInterval(20011);
  timers[T_SYSTEM].setCallback(sendN2kSystemStatus);

  TimerManager::instance().start();
}

void handleNMEA2000Msg(const tN2kMsg &N2kMsg) {
  int iHandler;

  for (iHandler=0; NMEA2000Handlers[iHandler].PGN!=0 && !(N2kMsg.PGN==NMEA2000Handlers[iHandler].PGN); iHandler++);
  if (NMEA2000Handlers[iHandler].PGN!=0) {
    NMEA2000Handlers[iHandler].Handler(N2kMsg);
  }
}

void handleBinaryStatus(const tN2kMsg &N2kMsg) {
  unsigned char instance;
  tN2kBinaryStatus binaryStatus;

  if (ParseN2kBinaryStatus(N2kMsg, instance, binaryStatus) ) {
    Serial.println(instance);
    boatDataFromBinaryStatus(instance, binaryStatus, boatData);
    setIO(boatData, (SwitchBankInstance)instance);
  }
}

void handleAddressClaim(const tN2kMsg &N2kMsg) {
  timers[T_NEW_DEVICE].start();
}

void newDevice() {
  n2kBinaryStatus(E_ENGINE_STATUS);
  n2kBinaryStatus(E_UTILITIES_ENGINE_ROOM);
}

void handleEngineRPM(const tN2kMsg &N2kMsg) {
  unsigned char instance;
  // int servoValue;
  double rpm=0.0;
  double boost=0.0;
  int8_t trim=0;

  // N2kMsg.Print(&Serial);

  if (ParseN2kEngineParamRapid(N2kMsg, instance, rpm, boost, trim) ) {
    if (instance == 0) {
      if (N2kMsg.Source == 1) {
        boatData.engines.starboard.rpm = (int)rpm;
        starboardEngineRpmTime = millis();
      } else {
        boatData.engines.port.rpm = (int)rpm;
        portEngineRpmTime = millis();
      }
    } else if (instance == 1) {
      boatData.engines.starboard.rpm = (int)rpm;
      starboardEngineRpmTime = millis();
    }
  }
}

void handleEngineDynamicParameters(const tN2kMsg &N2kMsg) {
  unsigned char EngineInstance;
  double EngineOilPress;
  double EngineOilTemp;
  double EngineCoolantTemp;
  double AltenatorVoltage;
  double FuelRate;
  double EngineHours;
  double EngineCoolantPress;
  double EngineFuelPress;
  int8_t EngineLoad;
  int8_t EngineTorque;

  if (ParseN2kEngineDynamicParam(N2kMsg, EngineInstance, EngineOilPress, EngineOilTemp, EngineCoolantTemp,
      AltenatorVoltage, FuelRate, EngineHours, EngineCoolantPress, EngineFuelPress, EngineLoad, EngineTorque) ) {
    if (EngineInstance == 0) {
      boatData.engines.port.oilPressure = EngineOilPress;
      boatData.engines.port.waterTemperature = KelvinToC(EngineCoolantTemp);
    } else if (EngineInstance == 1) {
      boatData.engines.starboard.oilPressure = EngineOilPress;
      boatData.engines.starboard.waterTemperature = KelvinToC(EngineCoolantTemp);
    }
  }
}

void n2kBinaryStatus(SwitchBankInstance instance) {
  tN2kMsg N2kMsg;
  tN2kBinaryStatus binaryStatus;

  binaryStatus = binaryStatusFromBoatData(instance, boatData);
  SetN2kBinaryStatus(N2kMsg, (unsigned char)instance, binaryStatus);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kSystemStatus() {
  tN2kMsg N2kMsg;

  Serial.print("Restart count ");
  Serial.println(boatData.system.egnineRoomRestartCount);

  SetN2kReflectionsResetCount(N2kMsg, 11, boatData.system.egnineRoomRestartCount);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kSensorData() {
  static int sensorIndex = 0;
  tN2kMsg N2kMsg;

  switch (sensorIndex) {
    case 1:
      SetN2kDCBatStatus(N2kMsg, 0, boatData.batteries.port, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 2:
      SetN2kDCBatStatus(N2kMsg, 1, boatData.batteries.starboard, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 3:
      SetN2kDCBatStatus(N2kMsg, 2, boatData.batteries.auxiliary, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 4:
      SetN2kTemperature(N2kMsg, 1, 0, N2kts_EngineRoomTemperature, boatData.engines.engineRoomTemperatureBottom);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 5:
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_EngineRoomTemperature, boatData.engines.engineRoomTemperatureTop);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 6:
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_InsideTemperature, boatData.environment.insideTemperature);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 7:
      SetN2kFluidLevel(N2kMsg, 1, N2kft_Fuel, boatData.fuel.level, 1200.0);
      NMEA2000.SendMsg(N2kMsg);
      break;
    default:
      sensorIndex = 0;
      break;
  }

  ++sensorIndex;
}

void readSensors() {
  readSensors(boatData);
}

void sendN2kSteeringData() {
  tN2kMsg N2kMsg_Port;

  SetN2kPGN127245(N2kMsg_Port, boatData.engines.port.steering, 0, N2kRDO_NoDirectionOrder, 0.0);
  NMEA2000.SendMsg(N2kMsg_Port);
}

void sendN2kTrimTab() {
  tN2kMsg N2kMsg;

  if (newTrimAngle(boatData.trim.port) || newTrimAngle(boatData.trim.starboard)) {
    Serial.println((int8_t)(boatData.trim.port.angle / 100.0));
    Serial.println((int8_t)(boatData.trim.starboard.angle / 100.0));
    SetN2kTrimTab(N2kMsg, (int8_t)(boatData.trim.port.angle / 100.0), (int8_t)(boatData.trim.starboard.angle / 100.0));
    NMEA2000.SendMsg(N2kMsg);
  }
}

void sendN2kEngineRPM() {
  tN2kMsg N2kMsg_rapidPort;
  tN2kMsg N2kMsg_dynamicPort;
  tN2kMsg N2kMsg_rapidStarboard;
  tN2kMsg N2kMsg_dynamicStarboard;

  // static int rpm = 0;
  // Serial.print("RPM ");
  // Serial.println(rpm);
  // SetN2kEngineParamRapid(N2kMsg_rapidPort, 0, rpm, 0.0, boatData.engines.port.trim.angle);
  // NMEA2000.SendMsg(N2kMsg_rapidPort);
  // SetN2kEngineParamRapid(N2kMsg_rapidStarboard, 1, boatData.engines.starboard.rpm, 0.0, boatData.engines.starboard.trim.angle);
  // NMEA2000.SendMsg(N2kMsg_rapidStarboard);
  // rpm++;
  // return;

  if ((millis() - portEngineRpmTime) > 2000) {
    // Serial.print("Sending Port RPM ");
    // Serial.println(boatData.engines.port.rpm);
    SetN2kEngineParamRapid(N2kMsg_rapidPort, 0, boatData.engines.port.rpm, 0.0, boatData.engines.port.trim.angle);
    NMEA2000.SendMsg(N2kMsg_rapidPort);

    SetN2kEngineDynamicParam(N2kMsg_dynamicPort, 0,
      boatData.engines.port.oilPressure,
      N2kDoubleNA, // EngineOilTemp
      boatData.engines.port.waterTemperature,
      N2kDoubleNA, // AltenatorVoltage,
      N2kDoubleNA, // FuelRate,
      N2kDoubleNA, // EngineHours
      N2kDoubleNA, // EngineCoolantPress
      N2kDoubleNA // EngineFuelPress
    );
    NMEA2000.SendMsg(N2kMsg_dynamicPort);

    portEngineRpmTime = millis();
  }
  // delay(N2K_DELAY_BETWEEN_SEND);
  if ((millis() - starboardEngineRpmTime) > 2000) {
    // Serial.print("Sending Starboard RPM ");
    // Serial.println(boatData.engines.starboard.rpm);
    SetN2kEngineParamRapid(N2kMsg_rapidStarboard, 1, boatData.engines.starboard.rpm, 0.0, boatData.engines.starboard.trim.angle);
    NMEA2000.SendMsg(N2kMsg_rapidStarboard);

    SetN2kEngineDynamicParam(N2kMsg_dynamicStarboard, 1,
      boatData.engines.starboard.oilPressure,
      N2kDoubleNA, // EngineOilTemp
      boatData.engines.starboard.waterTemperature,
      N2kDoubleNA, // AltenatorVoltage,
      N2kDoubleNA, // FuelRate,
      N2kDoubleNA, // EngineHours
      N2kDoubleNA, // EngineCoolantPress
      N2kDoubleNA // EngineFuelPress
    );
    NMEA2000.SendMsg(N2kMsg_dynamicStarboard);

    starboardEngineRpmTime = millis();
  }

}

void readRestartCount() {
  EEPROM.get(0, boatData.system.egnineRoomRestartCount);
  boatData.system.egnineRoomRestartCount++;
  EEPROM.put(0, boatData.system.egnineRoomRestartCount);
}
