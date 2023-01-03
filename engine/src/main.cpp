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
  T_SYSTEM,
  T_SEND_STEERING_DATA,
  T_SEND_BATTERY_DATA,
  T_SEND_TEMPERATURE_DATA,
  T_SEND_FUEL_DATA,
  T_SEND_ENGINE_DATA,
  T_SEND_ENGINE_RPM,
  T_NUTRASALT,
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

  readRestartCount();

  setupFrequency();

  boatData.trim.port.angle = 1000.0;
  boatData.trim.starboard.angle = 1000.0;
}

void loop() {
  NMEA2000.ParseMessages();

  loopFrequency(boatData);

  processTrim();

  if (readIO(boatData, E_ENGINE_STATUS)) {
    n2kBinaryStatus(E_ENGINE_STATUS);
  }

  if (readIO(boatData, E_UTILITIES_ENGINE_ROOM)) {
    setIO(boatData, E_UTILITIES_ENGINE_ROOM);
    n2kBinaryStatus(E_UTILITIES_ENGINE_ROOM);
  }

  if (readIO(boatData, E_BILGE_PUMPS)) {
    setIO(boatData, E_BILGE_PUMPS);
    n2kBinaryStatus(E_BILGE_PUMPS);
  }

  TimerManager::instance().update();

  if ( Serial.available() ) { Serial.read(); }
}

void setupNMEA() {

  NMEA2000.SetProductInformation("10000101", 101, "Engine Room", "1.0.1 (2019-05-26)", "1.0.1 (2019-05-26)");

  NMEA2000.SetDeviceInformation(100101, 132, 25, 2041);

  NMEA2000.SetForwardStream(&Serial);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);
  // NMEA2000.EnableForward(false);

  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 11);

  NMEA2000.SetMsgHandler(handleNMEA2000Msg);

  NMEA2000.SetHeartbeatIntervalAndOffset(55000, 110);

  NMEA2000.Open();
}

void setupTimers() {
  timers[T_NEW_DEVICE].setInterval(11000, 1);
  timers[T_NEW_DEVICE].setCallback(newDevice);

  timers[T_SYSTEM].setInterval(20011);
  timers[T_SYSTEM].setCallback(sendN2kSystemStatus);

  timers[T_SEND_STEERING_DATA].setInterval(533);
  timers[T_SEND_STEERING_DATA].setCallback(sendSteeringData);

  timers[T_SEND_BATTERY_DATA].setInterval(2055);
  timers[T_SEND_BATTERY_DATA].setCallback(sendBatteryData);

  timers[T_SEND_TEMPERATURE_DATA].setInterval(5055);
  timers[T_SEND_TEMPERATURE_DATA].setCallback(sendTemperatureData);

  timers[T_SEND_FUEL_DATA].setInterval(3077);
  timers[T_SEND_FUEL_DATA].setCallback(sendFuelData);

  timers[T_SEND_ENGINE_DATA].setInterval(15099);
  timers[T_SEND_ENGINE_DATA].setCallback(sendEngineData);

  timers[T_SEND_ENGINE_RPM].setInterval(2033);
  timers[T_SEND_ENGINE_RPM].setCallback(sendN2kEngineRPM);

  timers[T_NUTRASALT].setInterval(1000);
  timers[T_NUTRASALT].setCallback(processNutraSalt);

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
    boatDataFromBinaryStatus(instance, binaryStatus, boatData);
    setIO(boatData, (SwitchBankInstance)instance);
    processNutraSalt();
  }
}

void handleAddressClaim(const tN2kMsg &N2kMsg) {
  timers[T_NEW_DEVICE].start();
}

void newDevice() {
  n2kBinaryStatus(E_ENGINE_STATUS);
  n2kBinaryStatus(E_UTILITIES_ENGINE_ROOM);
  n2kBinaryStatus(E_BILGE_PUMPS);
}

void readRestartCount() {
  EEPROM.get(0, boatData.system.egnineRoomRestartCount);
  boatData.system.egnineRoomRestartCount++;
  EEPROM.put(0, boatData.system.egnineRoomRestartCount);
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

  SetN2kReflectionsResetCount(N2kMsg, 11, boatData.system.egnineRoomRestartCount);
  NMEA2000.SendMsg(N2kMsg);
}

void sendSteeringData() {
  tN2kMsg N2kMsg_Port;

  readSteering(boatData);

  SetN2kPGN127245(N2kMsg_Port, boatData.engines.port.steering, 0, N2kRDO_NoDirectionOrder, 0.0);
  NMEA2000.SendMsg(N2kMsg_Port);
}

void sendBatteryData() {
  static int index = 0;
  tN2kMsg N2kMsg;

  readBatteries(boatData);

  switch (index) {
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
    default:
      index = 0;
      break;
  }

  ++index;
}

void sendTemperatureData() {
  static int index = 0;
  tN2kMsg N2kMsg;

  readTemperatures(boatData);

  switch (index) {
    case 1:
      SetN2kTemperature(N2kMsg, 1, 0, N2kts_EngineRoomTemperature, boatData.engines.engineRoomTemperatureBottom);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 2:
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_EngineRoomTemperature, boatData.engines.engineRoomTemperatureTop);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 3:
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_InsideTemperature, boatData.environment.insideTemperature);
      NMEA2000.SendMsg(N2kMsg);
      break;
    default:
      index = 0;
      break;
  }

  ++index;
}

void sendFuelData() {
  tN2kMsg N2kMsg;

  readFuel(boatData);

  SetN2kFluidLevel(N2kMsg, 1, N2kft_Fuel, boatData.fuel.level, 1200.0);
  NMEA2000.SendMsg(N2kMsg);
}

void sendEngineData() {
  tN2kMsg N2kMsg;

  // readEngines(boatData);

}

void handleEngineRPM(const tN2kMsg &N2kMsg) {
  unsigned char instance;
  double rpm=0.0;
  double boost=0.0;
  int8_t trim=0;

  if (ParseN2kEngineParamRapid(N2kMsg, instance, rpm, boost, trim) ) {
    if (instance == 0) {
      if (N2kMsg.Source == 1) {
        boatData.engines.starboard.rpm = (int)rpm;
        boatData.engines.starboard.trim.angle = (int)trim;
        starboardEngineRpmTime = millis();
      } else {
        boatData.engines.port.rpm = (int)rpm;
        boatData.engines.port.trim.angle = (int)trim;
        portEngineRpmTime = millis();
      }
    } else if (instance == 1) {
      boatData.engines.starboard.rpm = (int)rpm;
      boatData.engines.starboard.trim.angle = (int)trim;
      starboardEngineRpmTime = millis();
    }
  }
}

void handleEngineDynamicParameters(const tN2kMsg &N2kMsg) {
  unsigned char instance;
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
  // unsigned int value;

  if (ParseN2kEngineDynamicParam(N2kMsg, instance, EngineOilPress, EngineOilTemp, EngineCoolantTemp,
      AltenatorVoltage, FuelRate, EngineHours, EngineCoolantPress, EngineFuelPress, EngineLoad, EngineTorque) ) {
    if (instance == 0) {
      if (N2kMsg.Source == 1) {
        boatData.engines.starboard.oilPressure = EngineOilPress;
        boatData.engines.starboard.waterTemperature = KelvinToC(EngineCoolantTemp);
      } else {
      boatData.engines.port.oilPressure = EngineOilPress;
      boatData.engines.port.waterTemperature = KelvinToC(EngineCoolantTemp);
      }
    } else if (instance == 1) {
      boatData.engines.starboard.oilPressure = EngineOilPress;
      boatData.engines.starboard.waterTemperature = KelvinToC(EngineCoolantTemp);
    }
  }
}

void processTrim() {
  tN2kMsg N2kMsg;

  if (newTrimAngle(boatData.trim.port) || newTrimAngle(boatData.trim.starboard)) {
    Serial.print("Port: ");
    Serial.print((int8_t)(boatData.trim.port.angle / 100.0));
    Serial.print("  Startboard: ");
    Serial.println((int8_t)(boatData.trim.starboard.angle / 100.0));
    SetN2kTrimTab(N2kMsg, (int8_t)(boatData.trim.port.angle / 100.0), (int8_t)(boatData.trim.starboard.angle / 100.0));
    NMEA2000.SendMsg(N2kMsg);
  }
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
  tN2kEngineDiscreteStatus1 Status1;

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
      N2kDoubleNA, // EngineFuelPress
      N2kInt8NA, // EngineLoad
      N2kInt8NA, // EngineTorque
      Status1 // Alarms
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
      N2kDoubleNA, // EngineFuelPress
      N2kInt8NA,
      N2kInt8NA,
      Status1
    );
    NMEA2000.SendMsg(N2kMsg_dynamicStarboard);

    starboardEngineRpmTime = millis();
  }

}


void sendN2kNutraSaltCountdown(unsigned char instance, Engine &engine, bool reset = false) {
  tN2kMsg N2kMsg;
  unsigned int count;
  unsigned int countdown;

  if ((engine.nutraSaltStart > 0) && ((engine.nutraSalt == N2kOnOff_On) || reset)) {
    count = millis() - engine.nutraSaltStart;

    if (count > 0) {
      countdown = (26000 - count) / 1000;
    }

    if (count > 25000) {
      engine.ignition = N2kOnOff_Off;
      setIO(boatData, E_IGNITION_START);
      n2kBinaryStatus(E_IGNITION_START);
    }

    if (count > 26000) {
      engine.nutraSalt = N2kOnOff_Off;
      countdown = 0;
      engine.nutraSaltStart = 0;
      setIO(boatData, E_NUTRASALT);
      n2kBinaryStatus(E_NUTRASALT);
    }
    
    if (reset) {
      countdown = 0;
      engine.nutraSaltStart = 0;
      Serial.println("resetting");
    }

    Serial.print(instance);
    Serial.print(" - NutraSalt countdown ");
    Serial.println(countdown);
    SetN2kReflectionsPGN130981(N2kMsg, instance, countdown);
    NMEA2000.SendMsg(N2kMsg);
  }
}

void processNutraSalt() {
  bool reset;

  reset = false;
  if ((boatData.engines.port.nutraSalt == N2kOnOff_On) && (boatData.engines.port.nutraSaltStart == 0)) {
    boatData.engines.port.nutraSaltStart = millis();
  } else if ((boatData.engines.port.nutraSalt == N2kOnOff_Off) && (boatData.engines.port.nutraSaltStart > 0)) {
    reset = true;
  }
  sendN2kNutraSaltCountdown(0, boatData.engines.port, reset);

  reset = false;
  if ((boatData.engines.starboard.nutraSalt == N2kOnOff_On) && (boatData.engines.starboard.nutraSaltStart == 0)) {
    boatData.engines.starboard.nutraSaltStart = millis();
  } else if ((boatData.engines.starboard.nutraSalt == N2kOnOff_Off) && (boatData.engines.starboard.nutraSaltStart > 0)) {
    reset = true;
  }
  sendN2kNutraSaltCountdown(1, boatData.engines.starboard, reset);
}
