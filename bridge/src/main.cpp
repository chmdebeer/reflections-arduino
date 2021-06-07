#include <Arduino.h>
#include <N2kTypes.h>
#include "boatdata.h"
#include "io.h"

#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>
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
  T_ITEMS
};

Timer * timers = new Timer[T_ITEMS];

TinyGPSPlus gps;

bool newN2kBinaryStatus = false;
bool newN2kServoStatus = false;

void setup() {
  Serial.begin(115200);
  clearBoatData(boatData);
  setIO(boatData);
  setupIO();
  setupAC();
  setupTimers();
  setupNMEA();
}

void loop() {
  bool newIoBinaryStatus = false;

  NMEA2000.ParseMessages();

  newIoBinaryStatus = readIO(boatData);

  if (newIoBinaryStatus) {
    sendN2kBinaryStatus();
  }

  if (newIoBinaryStatus || newN2kBinaryStatus) {
    setIO(boatData);
    printBoatData(boatData);
    newN2kBinaryStatus = false;
    newIoBinaryStatus = false;
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

  if (ParseN2kEngineParamRapid(N2kMsg, instance, rpm, boost, trim) ) {
    if (instance == 0) {
      boatData.engines.port.rpm = (int)rpm;
    } else if (instance == 1) {
      boatData.engines.starboard.rpm = (int)rpm;
    }
    if ((abs(portEngineRpm - boatData.engines.port.rpm) > 5) || (abs(starboardEngineRpm - boatData.engines.starboard.rpm) > 5)) {
      portEngineRpm = boatData.engines.port.rpm;
      starboardEngineRpm = boatData.engines.starboard.rpm;
      newN2kServoStatus = true;
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
    newN2kServoStatus = true;
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
        newN2kServoStatus = true;
    }
  }
}

void handleAddressClaim(const tN2kMsg &N2kMsg) {
  timers[T_NEW_DEVICE].start();
}

void newDevice() {
  sendN2kBinaryStatus();
}

void sendN2kBinaryStatus() {
  tN2kMsg N2kMsg;
  tN2kBinaryStatus binaryStatus_1;
  tN2kBinaryStatus binaryStatus_2;

  binaryStatus_1 = binaryStatusFromBoatData(1, boatData);
  SetN2kBinaryStatus(N2kMsg, 1, binaryStatus_1);
  NMEA2000.SendMsg(N2kMsg);

  delay(N2K_DELAY_BETWEEN_SEND);

  binaryStatus_2 = binaryStatusFromBoatData(2, boatData);
  SetN2kBinaryStatus(N2kMsg, 2, binaryStatus_2);
  NMEA2000.SendMsg(N2kMsg);

}

void sendN2kACStatus() {
  tN2kMsg N2kMsg;

  readAC(boatData);
  SetN2kACStatus(N2kMsg, 1, 1, N2kACL_Line1, N2kACA_Good, boatData.ac.volts, boatData.ac.amps, 60.0, 25.0, 0.0, 0.0, 0.0);
  NMEA2000.SendMsg(N2kMsg);

}
