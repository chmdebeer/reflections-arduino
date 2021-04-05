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
#include <Servo.h>
#include <stdio.h>
#include <timer.h>
#include <timerManager.h>

#include "utils.h"

#include "attitude.h"

#define DEV_BINARY  0
#define DEV_TEMP  1
#define DEV_ATTITUDE  2

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {127501L, &handleBinaryStatus},
  {0,0}
};

const unsigned long binaryStatusTransmitMessages[] PROGMEM={127501L,0};
const unsigned long attitudeTransmitMessages[] PROGMEM={127257L,0};
const unsigned long temperatureMonitorTransmitMessages[] PROGMEM={130310L,130311L,130312L,0};

enum timers {
  T_TEMPERATURE,
  T_ATTITUDE,
  T_ITEMS
};

Timer * timers = new Timer[T_ITEMS];

void setup() {
  Serial.begin(115200);
  clearBoatData(boatData);
  setIO(boatData);
  setupIO();
  setupTimers();
  setupNMEA();

  setupAttitude();
}

void loop() {
  NMEA2000.ParseMessages();

  if (readIO(boatData)) {
    sendN2kBinaryStatus();
  }

  TimerManager::instance().update();

  loopAttitude(boatData);

  if ( Serial.available() ) { Serial.read(); }
}

void setupNMEA() {
  NMEA2000.SetDeviceCount(2);

  NMEA2000.SetProductInformation(
    "10000401", 401, "Bilge", "4.0.1 (2019-05-26)", "4.0.1 (2019-05-26)", 0xff, 0xffff, 0xff, DEV_BINARY
  );
  NMEA2000.SetProductInformation(
    "10000402", 401, "Bilge", "4.0.1 (2019-05-26)", "4.0.1 (2019-05-26)", 0xff, 0xffff, 0xff, DEV_TEMP
  );
  NMEA2000.SetProductInformation(
    "10000403", 401, "Bilge", "4.0.1 (2019-05-26)", "4.0.1 (2019-05-26)", 0xff, 0xffff, 0xff, DEV_ATTITUDE
  );

  NMEA2000.SetDeviceInformation(100401, 130, 30, 2041, 4, DEV_BINARY);
  NMEA2000.SetDeviceInformation(100402, 130, 75, 2041, 4, DEV_TEMP);
  NMEA2000.SetDeviceInformation(100403, 140, 60, 2041, 4, DEV_ATTITUDE);

  NMEA2000.SetForwardStream(&Serial);
  NMEA2000.SetForwardType(tNMEA2000::fwdt_Text);
  // NMEA2000.EnableForward(false);

  NMEA2000.SetMode(tNMEA2000::N2km_ListenAndNode, 41);

  NMEA2000.ExtendTransmitMessages(binaryStatusTransmitMessages, DEV_BINARY);
  NMEA2000.ExtendTransmitMessages(temperatureMonitorTransmitMessages, DEV_TEMP);
  NMEA2000.ExtendTransmitMessages(attitudeTransmitMessages, DEV_ATTITUDE);

  NMEA2000.SetMsgHandler(handleNMEA2000Msg);

  NMEA2000.SetHeartbeatInterval(59300);

  NMEA2000.Open();
}

void setupTimers() {
  timers[T_TEMPERATURE].setInterval(5007);
  timers[T_TEMPERATURE].setCallback(sendN2kTemperature);

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
    setIO(boatData);
    printBoatData(boatData);
  }
}

void sendN2kBinaryStatus() {
  tN2kMsg N2kMsg;
  tN2kBinaryStatus binaryStatus;

  binaryStatus = binaryStatusFromBoatData(3, boatData);
  SetN2kBinaryStatus(N2kMsg, 3, binaryStatus);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kTemperature() {
  tN2kMsg N2kMsg;

  readSensors(boatData);
  SetN2kTemperature(N2kMsg, 1, 1, N2kts_InsideTemperature, CToKelvin(boatData.environment.belowDeckTemperature));
  NMEA2000.SendMsg(N2kMsg, DEV_TEMP);
}

void sendN2kAttitude() {
  tN2kMsg N2kMsg;

  SetN2kAttitude(N2kMsg, 1, DegToRad(boatData.attitude.yaw), DegToRad(boatData.attitude.pitch), DegToRad(boatData.attitude.roll));
  NMEA2000.SendMsg(N2kMsg, DEV_ATTITUDE);
  // Serial.print("R=");
  // Serial.print(boatData.attitude.roll, 4);
  // Serial.print("\tP=");
  // Serial.print(boatData.attitude.pitch, 4);
  // Serial.print("\tY=");
  // Serial.print(boatData.attitude.yaw, 4);
  // Serial.println(" <-");
}
