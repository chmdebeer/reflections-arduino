#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48


#include <Arduino.h>
#include <EEPROM.h>
#include <N2kTypes.h>
#include "boatdata.h"

#include <NMEA2000_CAN.h>  // This will automatically choose right CAN library and create suitable NMEA2000 object
#include <N2kMessages.h>
#include <N2kReflections.h>
#include <N2kMsg.h>
#include <NMEA2000.h>

#include <main.h>

#include <timer.h>
#include <timerManager.h>
#include <Bounce2.h>

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {127501L, &handleBinaryStatus},
  {59904L, &handleAddressClaim},
  {0,0}
};

enum timers {
  T_NEW_DEVICE,
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
  Serial.print("Start-up ");
  clearBoatData(boatData);
  setupTimers();
  setupNMEA();
  readRestartCount();
  // Serial.println(restartCount);
}

void loop() {
  NMEA2000.ParseMessages();

  sendN2kSystemStatus();

    // sendN2kBinaryStatus();
    delay(5000);

  // TimerManager::instance().update();

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
  timers[T_NEW_DEVICE].setInterval(11000, 1);
  timers[T_NEW_DEVICE].setCallback(newDevice);

  timers[T_SEND_SENSOR_DATA].setInterval(1003);
  timers[T_SEND_SENSOR_DATA].setCallback(sendN2kSensorData);

  TimerManager::instance().start();
  timers[T_NEW_DEVICE].stop();
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

void handleAddressClaim(const tN2kMsg &N2kMsg) {
  Serial.println("Address Claim");
  timers[T_NEW_DEVICE].start();
}

void newDevice() {
  Serial.println("New Device send data");
  sendN2kBinaryStatus();
}

void sendN2kBinaryStatus() {
  tN2kMsg N2kMsg;
  tN2kBinaryStatus binaryStatus_1;
  tN2kBinaryStatus binaryStatus_2;
  tN2kBinaryStatus binaryStatus_3;
  tN2kBinaryStatus binaryStatus_4;
  tN2kBinaryStatus binaryStatus_11;

  binaryStatus_1 = binaryStatusFromBoatData(1, boatData);
  SetN2kBinaryStatus(N2kMsg, 1, binaryStatus_1);
  NMEA2000.SendMsg(N2kMsg);

  delay(N2K_DELAY_BETWEEN_SEND);

  binaryStatus_2 = binaryStatusFromBoatData(2, boatData);
  SetN2kBinaryStatus(N2kMsg, 2, binaryStatus_2);
  NMEA2000.SendMsg(N2kMsg);

  delay(N2K_DELAY_BETWEEN_SEND);

  binaryStatus_3 = binaryStatusFromBoatData(3, boatData);
  SetN2kBinaryStatus(N2kMsg, 3, binaryStatus_3);
  NMEA2000.SendMsg(N2kMsg);

  delay(N2K_DELAY_BETWEEN_SEND);

  binaryStatus_4 = binaryStatusFromBoatData(4, boatData);
  SetN2kBinaryStatus(N2kMsg, 4, binaryStatus_4);
  NMEA2000.SendMsg(N2kMsg);

  delay(N2K_DELAY_BETWEEN_SEND);

  boatData.system.egnineRoomRestartCount = 4000;

  binaryStatus_11 = binaryStatusFromBoatData(11, boatData);
  SetN2kBinaryStatus(N2kMsg, 11, binaryStatus_11);
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
  tN2kMsg N2kMsg_port;
  tN2kMsg N2kMsg_starboard;

  switch (sensorIndex) {
    case 0:
      boatData.fuel.level = 25;
      SetN2kFluidLevel(N2kMsg, 1, N2kft_Fuel, boatData.fuel.level, 1200.0);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 1:
      boatData.engines.engineRoomTemperature = 32+273;
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_EngineRoomTemperature, boatData.engines.engineRoomTemperature);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 2:
      boatData.engines.port.oilPressure = 19000;
      boatData.engines.port.waterTemperature = 85+273;
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
      boatData.engines.starboard.oilPressure = 20000;
      boatData.engines.starboard.waterTemperature = 90+273;
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
      boatData.batteries.port = 12;
      SetN2kDCBatStatus(N2kMsg, 0, boatData.batteries.port, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 5:
      boatData.batteries.starboard = 13;
      SetN2kDCBatStatus(N2kMsg, 1, boatData.batteries.starboard, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 6:
      boatData.batteries.auxiliary = 14;
      SetN2kDCBatStatus(N2kMsg, 2, boatData.batteries.auxiliary, 0, 0, 1);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 7:
      boatData.ac.volts = 112;
      boatData.ac.amps = 15;
      SetN2kACStatus(N2kMsg, 1, 1, N2kACL_Line1, N2kACA_Good, boatData.ac.volts, boatData.ac.amps, 60.0, 25.0, 0.0, 0.0, 0.0);
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 8:
      boatData.environment.belowDeckTemperature = 21;
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_InsideTemperature, CToKelvin(boatData.environment.belowDeckTemperature));
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 9:
      boatData.attitude.yaw = 10;
      boatData.attitude.pitch = 15;
      boatData.attitude.roll = 4;
      SetN2kAttitude(N2kMsg, 1, DegToRad(boatData.attitude.yaw), DegToRad(boatData.attitude.pitch), DegToRad(boatData.attitude.roll));
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 10:
      boatData.engines.port.rpm = 3010;
      boatData.engines.starboard.rpm = 3020;
      boatData.tilt.port.angle = 4.0/13.0;
      boatData.tilt.starboard.angle = 12.0/13.0;
      SetN2kEngineParamRapid(N2kMsg_port, 0, boatData.engines.port.rpm, 0.0, boatData.tilt.port.angle);
      NMEA2000.SendMsg(N2kMsg_port);
      // delay(N2K_DELAY_BETWEEN_SEND);
      SetN2kEngineParamRapid(N2kMsg_starboard, 1, boatData.engines.starboard.rpm, 0.0, boatData.tilt.starboard.angle);
      NMEA2000.SendMsg(N2kMsg_starboard);
      break;
    case 11:
      boatData.engines.engineRoomTemperature = 25+273;
      SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, boatData.engines.engineRoomTemperature);
      NMEA2000.SendMsg(N2kMsg);
      break;
    default:
      break;
  }

  if (++sensorIndex > 11) {
    sensorIndex = 0;
  }
}

void readRestartCount() {
  EEPROM.get(0, boatData.system.egnineRoomRestartCount);
  boatData.system.egnineRoomRestartCount++;
  EEPROM.put(0, boatData.system.egnineRoomRestartCount);
}
