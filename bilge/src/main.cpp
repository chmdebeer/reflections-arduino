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
#include <stdio.h>

#include "utils.h"
#include "attitude.h"

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {127501L, &handleBinaryStatus},
  {0,0}
};

const unsigned long binaryStatusTransmitMessages[] PROGMEM={127501L,0};
const unsigned long attitudeTransmitMessages[] PROGMEM={127257L,0};
const unsigned long temperatureMonitorTransmitMessages[] PROGMEM={130310L,130311L,130312L,0};

enum timers {
  T_NEW_DEVICE,
  T_SYSTEM,
  T_TEMPERATURE,
  T_ATTITUDE,
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

  setupAttitude();
}

void loop() {

  NMEA2000.ParseMessages();

  if (readIO(boatData, E_BILGE_PUMPS)) {
    setIO(boatData, E_BILGE_PUMPS);
    n2kBinaryStatus(E_BILGE_PUMPS);
  }

  TimerManager::instance().update();

  loopAttitude(boatData);

  if ( Serial.available() ) { Serial.read(); }
}

void setupNMEA() {

  NMEA2000.SetProductInformation("10000401", 401, "Bilge", "4.0.1 (2019-05-26)", "4.0.1 (2019-05-26)");

  NMEA2000.SetDeviceInformation(100401, 130, 30, 2041);

  NMEA2000.SetForwardStream(&Serial);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);
  // NMEA2000.EnableForward(false);

  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 41);

  NMEA2000.SetMsgHandler(handleNMEA2000Msg);

  NMEA2000.SetHeartbeatInterval(59300);

  NMEA2000.Open();
}

void setupTimers() {
  timers[T_NEW_DEVICE].setInterval(12000, 1);
  timers[T_NEW_DEVICE].setCallback(newDevice);

  timers[T_TEMPERATURE].setInterval(5007);
  timers[T_TEMPERATURE].setCallback(sendN2kSensorData);

  timers[T_ATTITUDE].setInterval(1002);
  timers[T_ATTITUDE].setCallback(sendN2kAttitude);

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
  n2kBinaryStatus(E_BILGE_PUMPS);
}

void readRestartCount() {
  EEPROM.get(0, boatData.system.bridgeRestartCount);
  boatData.system.bridgeRestartCount++;
  EEPROM.put(0, boatData.system.bridgeRestartCount);
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

  SetN2kReflectionsResetCount(N2kMsg, 21, boatData.system.bridgeRestartCount);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kSensorData() {
  tN2kMsg N2kMsg;

  readSensors(boatData);
  SetN2kTemperature(N2kMsg, 1, 1, N2kts_InsideTemperature, CToKelvin(boatData.environment.belowDeckTemperature));
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kAttitude() {
  tN2kMsg N2kMsg;

  SetN2kAttitude(N2kMsg, 1, DegToRad(boatData.attitude.yaw), DegToRad(boatData.attitude.pitch), DegToRad(boatData.attitude.roll));
  NMEA2000.SendMsg(N2kMsg);
  // Serial.print("R=");
  // Serial.print(boatData.attitude.roll, 4);
  // Serial.print("\tP=");
  // Serial.print(boatData.attitude.pitch, 4);
  // Serial.print("\tY=");
  // Serial.print(boatData.attitude.yaw, 4);
  // Serial.println(" <-");
}
