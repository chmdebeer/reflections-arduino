#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48


#include <Arduino.h>
#include <N2kTypes.h>
#include "boatdata.h"

#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>
#include <N2kMsg.h>
#include <NMEA2000.h>

#include <main.h>

#include <timer.h>
#include <timerManager.h>
#include <Bounce2.h>

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {127501L, &handleBinaryStatus},
  {0,0}
};

enum timers {
  T_READ_SENSOR_DATA,
  T_SEND_SENSOR_DATA,
  T_ENGINE,
  T_LOCK,
  T_ITEMS
};

Timer * timers = new Timer[T_ITEMS];

bool newN2kBinaryStatus = false;
bool newBinaryStatus = false;

void setup() {
  Serial.begin(115200);
  clearBoatData(boatData);
  setupTimers();
  setupNMEA();

}

void loop() {
  NMEA2000.ParseMessages();

    // sendN2kBinaryStatus();

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

  NMEA2000.SetHeartbeatInterval(59300);

  NMEA2000.Open();
}

void setupTimers() {
  timers[T_READ_SENSOR_DATA].setInterval(1207);
  timers[T_READ_SENSOR_DATA].setCallback(readSensors);
  //
  timers[T_SEND_SENSOR_DATA].setInterval(1003);
  timers[T_SEND_SENSOR_DATA].setCallback(sendN2kSensorData);
  //
  // timers[T_ENGINE].setInterval(2033);
  // timers[T_ENGINE].setCallback(SendN2kEngineRPM);
  //
  // timers[T_LOCK].setInterval(517);
  // timers[T_LOCK].setCallback(readLock);
  //
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
    newN2kBinaryStatus = true;
  }
}

void sendN2kBinaryStatus() {
  tN2kMsg N2kMsg;
  tN2kBinaryStatus binaryStatus;

  binaryStatus = binaryStatusFromBoatData(4, boatData);
  SetN2kBinaryStatus(N2kMsg, 4, binaryStatus);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kSensorData() {
  static int sensorIndex = 0;
  tN2kMsg N2kMsg;

  Serial.println("sensors");

  switch (sensorIndex) {
    case 0:
      SetN2kFluidLevel(N2kMsg, 1, N2kft_Fuel, boatData.fuel.level, 1200.0);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 1:
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_EngineRoomTemperature, boatData.engines.engineRoomTemperature);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 2:
      SetN2kEngineDynamicParam(N2kMsg, 0,
        boatData.engines.port.oilPressure,
        N2kDoubleNA, // EngineOilTemp
        boatData.engines.port.waterTemperature,
        N2kDoubleNA, // AltenatorVoltage,
        N2kDoubleNA, // FuelRate,
        N2kDoubleNA, // EngineHours
        N2kDoubleNA, // EngineCoolantPress
        N2kDoubleNA // EngineFuelPress
      );
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 3:
      SetN2kEngineDynamicParam(N2kMsg, 1,
        boatData.engines.starboard.oilPressure,
        N2kDoubleNA, // EngineOilTemp
        boatData.engines.starboard.waterTemperature,
        N2kDoubleNA, // AltenatorVoltage,
        N2kDoubleNA, // FuelRate,
        N2kDoubleNA, // EngineHours
        N2kDoubleNA, // EngineCoolantPress
        N2kDoubleNA // EngineFuelPress
      );
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 4:
      SetN2kDCBatStatus(N2kMsg, 0, boatData.batteries.port, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 5:
      SetN2kDCBatStatus(N2kMsg, 1, boatData.batteries.starboard, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 6:
      SetN2kDCBatStatus(N2kMsg, 2, boatData.batteries.auxiliary, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    default:
      break;
  }

  if (++sensorIndex > 6) {
    sensorIndex = 0;
  }
}

void readSensors() {
  boatData.engines.port.rpm = boatData.engines.port.rpm + 10;
  boatData.engines.starboard.rpm = boatData.engines.starboard.rpm + 15;
  SendN2kEngineRPM();
  // readSensors(boatData);
}

void SendN2kEngineRPM() {
  tN2kMsg N2kMsg_port;
  tN2kMsg N2kMsg_starboard;

  SetN2kEngineParamRapid(N2kMsg_port, 0, boatData.engines.port.rpm, 0.0, boatData.tilt.port.angle);
  NMEA2000.SendMsg(N2kMsg_port);
  // delay(N2K_DELAY_BETWEEN_SEND);
  SetN2kEngineParamRapid(N2kMsg_starboard, 1, boatData.engines.starboard.rpm, 0.0, boatData.tilt.starboard.angle);
  NMEA2000.SendMsg(N2kMsg_starboard);
}
