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
#include <Time.h>
#include <timer.h>
#include <timerManager.h>
#include <TinyGPS++.h>

#include "utils.h"

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {127488L, &handleEngineRPM},
  {127489L, &handleEngineDynamicParameters},
  {127501L, &handleBinaryStatus},
  {127505L, &handleFluidLevel},
  {59904L, &handleAddressClaim},
  {0,0}
};

enum timers {
  T_NEW_DEVICE,
  T_IGNITION_LED,
  T_GNSS,
  T_AC,
  T_SYSTEM,
  T_ITEMS
};

Timer * timers = new Timer[T_ITEMS];

TinyGPSPlus gps;

bool newN2kBinaryStatus = false;

void setup() {
  unsigned char instance;

  Serial.begin(115200);

  clearBoatData(boatData);
  for (instance=1; instance < (unsigned char)E_SWITCH_BANK_INSTANCES; instance++) {
    setIO(boatData, (SwitchBankInstance)instance);
  }

  setupIO();
  setupAC();
  setupTimers();
  setupNMEA();
  readRestartCount();
}

void loop() {

  NMEA2000.ParseMessages();

  for (int instance=1; instance < (unsigned char)E_SWITCH_BANK_INSTANCES; instance++) {
    if (((SwitchBankInstance)instance == E_UTILITIES_ENGINE_ROOM) || ((SwitchBankInstance)instance == E_UTILITIES_ENGINE_ROOM)) {
      continue;
    }
    if (readIO(boatData, (SwitchBankInstance)instance)) {
      n2kBinaryStatus((SwitchBankInstance)instance);
    }
  }

  if (readIO(boatData, E_UTILITIES_ENGINE_ROOM)) {
    n2kBinaryStatus(E_UTILITIES_BILGE);
    n2kBinaryStatus(E_UTILITIES_ENGINE_ROOM);
  }

  TimerManager::instance().update();

  if ( Serial.available() ) { Serial.read(); }
}

void setupNMEA() {

  NMEA2000.SetProductInformation("10000201", 201, "Bridge", "2.0.1 (2019-05-26)", "2.0.1 (2019-05-26)");

  NMEA2000.SetDeviceInformation(100201, 132, 25, 2041);

  NMEA2000.SetForwardStream(&Serial);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);
  // NMEA2000.EnableForward(false);

  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 21);

  NMEA2000.SetMsgHandler(handleNMEA2000Msg);

  NMEA2000.SetHeartbeatInterval(59300);

  NMEA2000.Open();
}

void setupTimers() {
  timers[T_NEW_DEVICE].setInterval(12000, 1);
  timers[T_NEW_DEVICE].setCallback(newDevice);

  timers[T_IGNITION_LED].setInterval(500);
  timers[T_IGNITION_LED].setCallback(blinkBridgeStartLed);

  timers[T_AC].setInterval(10007);
  timers[T_AC].setCallback(sendN2kACStatus);

  timers[T_SYSTEM].setInterval(20012);
  timers[T_SYSTEM].setCallback(sendN2kSystemStatus);

  TimerManager::instance().start();
}

void blinkBridgeStartLed() {
  blinkStartLed(boatData, O_PORT_START, O_STARBOARD_START);
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
    newN2kBinaryStatus = true;
  }
}

void handleEngineRPM(const tN2kMsg &N2kMsg) {
  static int portEngineRpm = -1;
  static int starboardEngineRpm = -1;
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
      } else {
        boatData.engines.port.rpm = (int)rpm;
      }
    } else if (instance == 1) {
      boatData.engines.starboard.rpm = (int)rpm;
    }
    if ((abs(portEngineRpm - boatData.engines.port.rpm) > 5) || (abs(starboardEngineRpm - boatData.engines.starboard.rpm) > 5)) {
      portEngineRpm = boatData.engines.port.rpm;
      starboardEngineRpm = boatData.engines.starboard.rpm;
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

void handleFluidLevel(const tN2kMsg &N2kMsg) {
    unsigned char instance;
    tN2kFluidType fluidType;
    double level=0;
    double capacity=0;

    if (ParseN2kFluidLevel(N2kMsg, instance, fluidType, level, capacity) ) {
      if (fluidType == N2kft_Fuel) {
        boatData.fuel.level = level;
        boatData.fuel.capacity = capacity;
    }
  }
}

void handleAddressClaim(const tN2kMsg &N2kMsg) {
  timers[T_NEW_DEVICE].start();
}

void newDevice() {
  n2kBinaryStatus(E_IGNITION_START);
  n2kBinaryStatus(E_POWER_TRIM);
  n2kBinaryStatus(E_TRIM);
  n2kBinaryStatus(E_LIGHTS);
  n2kBinaryStatus(E_SPOTLIGHT);
  n2kBinaryStatus(E_UTILITIES_CABIN);
  n2kBinaryStatus(E_UTILITIES_BILGE);
  n2kBinaryStatus(E_UTILITIES_ENGINE_ROOM);
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
  Serial.println(boatData.system.bridgeRestartCount);

  SetN2kReflectionsResetCount(N2kMsg, 21, boatData.system.bridgeRestartCount);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kACStatus() {
  tN2kMsg N2kMsg;

  readAC(boatData);
  SetN2kACStatus(N2kMsg, 1, 1, N2kACL_Line1, N2kACA_Good, boatData.ac.volts, boatData.ac.amps, 60.0, 25.0, 0.0, 0.0, 0.0);
  NMEA2000.SendMsg(N2kMsg);

}

void readRestartCount() {
  EEPROM.get(0, boatData.system.bridgeRestartCount);
  boatData.system.bridgeRestartCount++;

 Serial.println(boatData.system.bridgeRestartCount);
  EEPROM.put(0, boatData.system.bridgeRestartCount);
}
