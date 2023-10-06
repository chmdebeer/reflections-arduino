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

#include "utils.h"

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {59904L, &handleAddressClaim},
  {127488L, &handleEngineRPM},
  {127501L, &handleBinaryStatus},
  {0,0}
};

const unsigned long binaryStatusTransmitMessages[] PROGMEM={127501L,128006L,0};

enum timers {
  T_NEW_DEVICE,
  T_SYSTEM,
  T_IGNITION_LED,

  T_ITEMS
};

Timer * timers = new Timer[T_ITEMS];

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
}

void loop() {

  NMEA2000.ParseMessages();

  for (int instance=1; instance < (unsigned char)E_SWITCH_BANK_INSTANCES; instance++) {
    if (readIO(boatData, (SwitchBankInstance)instance)) {
      setIO(boatData, (SwitchBankInstance)instance);
      if (instance == (unsigned char)E_BOW_THRUSTER) {
        sendN2kThruster();
      } else {
        n2kBinaryStatus((SwitchBankInstance)instance);
      }
    }
  }

  TimerManager::instance().update();

  if ( Serial.available() ) { Serial.read(); }
}

void setupNMEA() {

  NMEA2000.SetProductInformation("10000301", 301, "Fly-Bridge", "3.0.1 (2019-05-26)", "3.0.1 (2019-05-26)");

  NMEA2000.SetDeviceInformation(100301, 130, 30, 2041);

  NMEA2000.SetForwardStream(&Serial);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);
  // NMEA2000.EnableForward(false);

  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 31);

  NMEA2000.ExtendTransmitMessages(binaryStatusTransmitMessages);

  NMEA2000.SetMsgHandler(handleNMEA2000Msg);

  NMEA2000.SetHeartbeatIntervalAndOffset(55000, 310);

  NMEA2000.Open();
}

void setupTimers() {
  timers[T_NEW_DEVICE].setInterval(12000, 1);
  timers[T_NEW_DEVICE].setCallback(newDevice);

  timers[T_SYSTEM].setInterval(20012);
  timers[T_SYSTEM].setCallback(sendN2kSystemStatus);

  timers[T_IGNITION_LED].setInterval(500);
  timers[T_IGNITION_LED].setCallback(blinkBridgeStartLed);

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

void readRestartCount() {
  EEPROM.get(0, boatData.system.restartCount);
  boatData.system.restartCount++;
  EEPROM.put(0, boatData.system.restartCount);
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

  SetN2kReflectionsResetCount(N2kMsg, 31, boatData.system.restartCount);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kThruster() {
  tN2kMsg N2kMsg;
  tN2kThrusterControlDirection direction;
  tN2kThrusterControlPower power;

  digitalWrite(O_BOW_THRUSTER_ON, (boatData.engines.bowThruster.power == N2kOnOff_On));
  if (boatData.engines.bowThruster.power == N2kOnOff_On) {
    power = N2kThrusterControlPower_On;

    if (boatData.engines.bowThruster.toPort == N2kOnOff_On) {
      direction = N2kThrusterControlDirection_ToPort;
    } else if (boatData.engines.bowThruster.toStarboard == N2kOnOff_On) {
      direction = N2kThrusterControlDirection_ToStarboard;
    } else {
      direction = N2kThrusterControlDirection_Off;
    }
  } else {
    power = N2kThrusterControlPower_Off;
    direction = N2kThrusterControlDirection_Off;
  }

  SetN2kPGN128006(N2kMsg, 11, 11, 
    direction, power,
    N2kThrusterControlRetract_Off, 1.0, N2kThrusterControlEventOtherDevice, 200, 0
  );
  NMEA2000.SendMsg(N2kMsg);

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
      } else {
        boatData.engines.port.rpm = (int)rpm;
      }
    } else if (instance == 1) {
      boatData.engines.starboard.rpm = (int)rpm;
    }
  }
}

void blinkBridgeStartLed() {
  blinkStartLed(boatData, O_PORT_START, O_STARBOARD_START);
}

