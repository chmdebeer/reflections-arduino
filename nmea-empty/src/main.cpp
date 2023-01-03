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
#include "utils.h"

BoatData boatData;

tNMEA2000Handler NMEA2000Handlers[]={
  {127501L, &handleBinaryStatus},
  {128006L, &handleThruster},
  {59904L, &handleAddressClaim},
  {0,0}
};

enum timers {
  T_NEW_DEVICE,
  T_READ_SENSOR_DATA,
  T_SEND_SENSOR_DATA,
  T_SWITCHES,
  T_ENGINE,
  T_LOCK,
  T_SYSTEM,
  T_NUTRASALT,
  T_ITEMS
};

Timer * timers = new Timer[T_ITEMS];

bool newN2kBinaryStatus = false;
bool newBinaryStatus = false;

void setup() {
  Serial.begin(115200);
  Serial.print("Start-up ");
  clearBoatData(boatData);
  boatData.engines.port.waterTemperature = 273;
  boatData.engines.starboard.waterTemperature = 283;
  setupTimers();
  setupNMEA();
  readRestartCount();
  Serial.println(boatData.system.egnineRoomRestartCount);
}


void processTrim() {
  tN2kMsg N2kMsg;

  if (newTrimAngle(boatData.trim.port) || newTrimAngle(boatData.trim.starboard)) {
    Serial.println((int8_t)(boatData.trim.port.angle / 100.0));
    Serial.println((int8_t)(boatData.trim.starboard.angle / 100.0));
    SetN2kTrimTab(N2kMsg, (int8_t)(boatData.trim.port.angle / 100.0), (int8_t)(boatData.trim.starboard.angle / 100.0));
    NMEA2000.SendMsg(N2kMsg);
  }
}

void loop() {

  NMEA2000.ParseMessages();

  // sendN2kSystemStatus();

    // sendN2kBinaryStatus();
    // delay(500);

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

  NMEA2000.SetHeartbeatIntervalAndOffset(55000, 110);

  NMEA2000.Open();

}

void setupTimers() {
  timers[T_NEW_DEVICE].setInterval(11000, 1);
  timers[T_NEW_DEVICE].setCallback(newDevice);


  // timers[T_SWITCHES].setInterval(1003);
  // timers[T_SWITCHES].setCallback(sendSwitches);

  timers[T_SEND_SENSOR_DATA].setInterval(1003);
  timers[T_SEND_SENSOR_DATA].setCallback(sendN2kSensorData);

  timers[T_ENGINE].setInterval(20000);
  timers[T_ENGINE].setCallback(sendEngine);

  timers[T_SYSTEM].setInterval(2000);
  timers[T_SYSTEM].setCallback(sendN2kSystemStatus);

  timers[T_NUTRASALT].setInterval(1000);
  timers[T_NUTRASALT].setCallback(sendN2kFuelConsumption);

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

void setIO(BoatData &boatData, SwitchBankInstance instance) {
  if (instance == E_IGNITION_START) {
    // boatData.engines.port.ignition // 1.1
    // boatData.engines.port.start // 1.2
    // boatData.engines.starboard.ignition // 1.3
    // boatData.engines.starboard.start // 1.4
    Serial.print("boatData.engines.port.ignition");
    Serial.println(boatData.engines.port.ignition == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.engines.port.start");
    Serial.println(boatData.engines.port.start == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.engines.starboard.ignition");
    Serial.println(boatData.engines.starboard.ignition == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.engines.starboard.start");
    Serial.println(boatData.engines.starboard.start == N2kOnOff_On ? " On" : " Off");

  } else if (instance == E_ENGINE_STATUS) {
    // boatData.engines.port.check // 2.1
    // boatData.engines.port.malfunction // 2.2
    // boatData.engines.starboard.check // 2.3
    // boatData.engines.starboard.malfunction // 2.$

  } else if (instance == E_POWER_TRIM) {
    // boatData.engines.port.trim.bowUp // 3.1
    // boatData.engines.port.trim.bowDown // 3.2
    // boatData.engines.starboard.trim.bowUp // 3.3
    // boatData.engines.starboard.trim.bowDown // 3.4
    Serial.print("boatData.engines.port.trim.bowUp");
    Serial.println(boatData.engines.port.trim.bowUp == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.engines.port.trim.bowDown");
    Serial.println(boatData.engines.port.trim.bowDown == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.engines.starboard.trim.bowUp");
    Serial.println(boatData.engines.starboard.trim.bowUp == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.engines.starboard.trim.bowDown");
    Serial.println(boatData.engines.starboard.trim.bowDown == N2kOnOff_On ? " On" : " Off");

  } else if (instance == E_TRIM) {
    // boatData.trim.port.bowUp // 4.1
    // boatData.trim.port.bowDown // 4.2
    // boatData.trim.starboard.bowUp // 4.3
    // boatData.trim.starboard.bowDown // 4.4
    // Serial.print("boatData.trim.port.bowUp");
    // Serial.println(boatData.trim.port.bowUp == N2kOnOff_On ? " On" : " Off");
    // Serial.print("boatData.trim.port.bowDown");
    // Serial.println(boatData.trim.port.bowDown == N2kOnOff_On ? " On" : " Off");
    // Serial.print("boatData.trim.starboard.bowUp");
    // Serial.println(boatData.trim.starboard.bowUp == N2kOnOff_On ? " On" : " Off");
    // Serial.print("boatData.trim.starboard.bowDown");
    // Serial.println(boatData.trim.starboard.bowDown == N2kOnOff_On ? " On" : " Off");

  } else if (instance == E_NUTRASALT) {
    // boatData.engines.port.nutraSalt // 5.1
    // boatData.engines.starboard.nutraSalt // 5.2
    Serial.print("boatData.engines.port.nutraSalt");
    Serial.println(boatData.engines.port.nutraSalt == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.engines.starboard.nutraSalt");
    Serial.println(boatData.engines.starboard.nutraSalt == N2kOnOff_On ? " On" : " Off");
    if ((boatData.engines.port.nutraSalt == N2kOnOff_On) && (boatData.engines.port.nutraSaltStart == 0)) {
      boatData.engines.port.nutraSaltStart = millis();
    } else if ((boatData.engines.port.nutraSalt == N2kOnOff_Off) && (boatData.engines.port.nutraSaltStart > 0)) {
      sendN2kNutraSaltCountdown(0, boatData.engines.port, true);
    }
    if ((boatData.engines.starboard.nutraSalt == N2kOnOff_On) && (boatData.engines.starboard.nutraSaltStart == 0)) {
      boatData.engines.starboard.nutraSaltStart = millis();
    } else if ((boatData.engines.starboard.nutraSalt == N2kOnOff_Off) && (boatData.engines.starboard.nutraSaltStart > 0)) {
      sendN2kNutraSaltCountdown(1, boatData.engines.starboard, true);
    }

  } else if (instance == E_LIGHTS) {
    // boatData.lights.instruments // 6.1
    // boatData.lights.cabin // 6.2
    // boatData.lights.berth // 6.3
    // boatData.lights.head // 6.4
    // boatData.lights.aftDeck // 6.5
    // boatData.lights.belowDeck // 6.6
    // boatData.lights.engineRoom // 6.7
    // boatData.lights.anchor // 6.8
    // boatData.lights.navigation // 6.9
    // boatData.lights.stern // 6.10
    Serial.print("boatData.lights.instruments");
    Serial.println(boatData.lights.instruments == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.cabin");
    Serial.println(boatData.lights.cabin == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.berth");
    Serial.println(boatData.lights.berth == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.head");
    Serial.println(boatData.lights.head == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.aftDeck");
    Serial.println(boatData.lights.aftDeck == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.belowDeck");
    Serial.println(boatData.lights.belowDeck == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.engineRoom");
    Serial.println(boatData.lights.engineRoom == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.anchor");
    Serial.println(boatData.lights.anchor == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.navigation");
    Serial.println(boatData.lights.navigation == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.stern");
    Serial.println(boatData.lights.stern == N2kOnOff_On ? " On" : " Off");

  } else if (instance == E_SPOTLIGHT) {
    // boatData.lights.spotlight.fog // 7.1
    // boatData.lights.spotlight.spot // 7.2
    // boatData.lights.spotlight.up // 7.3
    // boatData.lights.spotlight.down // 7.4
    // boatData.lights.spotlight.right // 7.5
    // boatData.lights.spotlight.left // 7.6
    Serial.print("boatData.lights.spotlight.fog");
    Serial.println(boatData.lights.spotlight.fog == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.spotlight.spot");
    Serial.println(boatData.lights.spotlight.spot == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.spotlight.up");
    Serial.println(boatData.lights.spotlight.up == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.spotlight.down");
    Serial.println(boatData.lights.spotlight.down == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.spotlight.right");
    Serial.println(boatData.lights.spotlight.right == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.lights.spotlight.left");
    Serial.println(boatData.lights.spotlight.left == N2kOnOff_On ? " On" : " Off");

  } else if (instance == E_UTILITIES_CABIN) {
    // boatData.utilities.horn.one // 8.1
    // boatData.utilities.horn.two // 8.2
    // boatData.utilities.wipers.port // 8.3
    // boatData.utilities.wipers.starboard // 8.4
    // boatData.utilities.cabinHeaterFan // 8.5
    // boatData.utilities.fmRadio // 8.6
    Serial.print("boatData.utilities.horn.one");
    Serial.println(boatData.utilities.horn.one == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.horn.two");
    Serial.println(boatData.utilities.horn.two == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.wipers.port");
    Serial.println(boatData.utilities.wipers.port == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.wipers.starboard");
    Serial.println(boatData.utilities.wipers.starboard == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.cabinHeaterFan");
    Serial.println(boatData.utilities.cabinHeaterFan == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.fmRadio");
    Serial.println(boatData.utilities.fmRadio == N2kOnOff_On ? " On" : " Off");

  } else if (instance == E_UTILITIES_BILGE) {
    // boatData.utilities.blackwaterAirPump // 9.1
    // boatData.utilities.macerator // 9.2
    // boatData.utilities.showerDrainPump // 9.3
    // boatData.bilgePumps.midship.on // 9.4
    // boatData.bilgePumps.midship.floatSwitch // 9.5
    Serial.print("boatData.utilities.blackwaterAirPump");
    Serial.println(boatData.utilities.blackwaterAirPump == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.macerator");
    Serial.println(boatData.utilities.macerator == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.showerDrainPump");
    Serial.println(boatData.utilities.showerDrainPump == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.bilgePumps.midship.on");
    Serial.println(boatData.bilgePumps.midship.on == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.bilgePumps.midship.floatSwitch");
    Serial.println(boatData.bilgePumps.midship.floatSwitch == N2kOnOff_On ? " On" : " Off");

  } else if (instance == E_UTILITIES_ENGINE_ROOM) {
    // boatData.utilities.waterPump // 10.1
    // boatData.blower // 10.2
    // boatData.bilgePumps.engineRoom.on // 10.3
    // boatData.bilgePumps.engineRoom.floatSwitch // 10.4
    // boatData.utilities.doorLock // 10.5
    Serial.print("boatData.utilities.waterPump");
    Serial.println(boatData.utilities.waterPump == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.blower");
    Serial.println(boatData.blower == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.bilgePumps.engineRoom.on");
    Serial.println(boatData.bilgePumps.engineRoom.on == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.bilgePumps.engineRoom.floatSwitch");
    Serial.println(boatData.bilgePumps.engineRoom.floatSwitch == N2kOnOff_On ? " On" : " Off");
    Serial.print("boatData.utilities.doorLock");
    Serial.println(boatData.utilities.doorLock == N2kOnOff_On ? " On" : " Off");
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

void handleThruster(const tN2kMsg &N2kMsg) {
  unsigned char SID;
  unsigned char Identifier; 
  tN2kThrusterControlDirection Direction;
  tN2kThrusterControlPower Power;
  tN2kThrusterControlRetract Retract;
  double Speed;
  tN2kThrusterControlEvent Event;
  unsigned char Timeout;
  double Azimuth; 

  Serial.println("Thruster");
  if (ParseN2kPGN128006(N2kMsg, SID, Identifier, 
    Direction, Power, Retract, Speed, Event, Timeout, Azimuth) ) {

    if (Power == N2kThrusterControlPower_On) {
      boatData.engines.bowThruster.power = N2kOnOff_On;

      if (Direction == N2kThrusterControlDirection_ToPort) {
        boatData.engines.bowThruster.toPort = N2kOnOff_On;
        boatData.engines.bowThruster.toStarboard = N2kOnOff_Off;
      } else if (Direction == N2kThrusterControlDirection_ToStarboard) {
        boatData.engines.bowThruster.toPort = N2kOnOff_Off;
        boatData.engines.bowThruster.toStarboard = N2kOnOff_On;
      } else {
        boatData.engines.bowThruster.toPort = N2kOnOff_Off;
        boatData.engines.bowThruster.toStarboard = N2kOnOff_Off;
      }

    } else {
      boatData.engines.bowThruster.power = N2kOnOff_Off;
      boatData.engines.bowThruster.toPort = N2kOnOff_Off;
      boatData.engines.bowThruster.toStarboard = N2kOnOff_Off;
    }     

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

void n2kBinaryStatus(SwitchBankInstance instance) {
  tN2kMsg N2kMsg;
  tN2kBinaryStatus binaryStatus;

  binaryStatus = binaryStatusFromBoatData(instance, boatData);
  SetN2kBinaryStatus(N2kMsg, (unsigned char)instance, binaryStatus);
  NMEA2000.SendMsg(N2kMsg);
}

void sendN2kNutraSaltCountdown(unsigned char instance, Engine &engine, bool reset = false) {
  tN2kMsg N2kMsg;
  unsigned int count;
  unsigned int countdown;

  if ((engine.nutraSaltStart > 0) && ((engine.nutraSalt == N2kOnOff_On) || reset)) {
    count = millis() - engine.nutraSaltStart;
    if (count < 45000) {
      countdown = (45000 - count) / 1000;
    } else {
      countdown = 0;
      engine.nutraSalt = N2kOnOff_Off;
      engine.ignition = N2kOnOff_Off;
      engine.nutraSaltStart = 0;
      // setIO(boatData, E_IGNITION_START);
      n2kBinaryStatus(E_IGNITION_START);
      // setIO(boatData, E_NUTRASALT);
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
  sendN2kNutraSaltCountdown(0, boatData.engines.port);
  sendN2kNutraSaltCountdown(1, boatData.engines.starboard);
}


void sendN2kACStatus() {
  tN2kMsg N2kMsg;

  boatData.ac.volts = 116.0;
  boatData.ac.amps = 1.7;
  SetN2kACStatus(N2kMsg, 1, 1, N2kACL_Line1, N2kACA_Good, boatData.ac.volts, boatData.ac.amps, 60.0, 25.0, 0.0, 0.0, 0.0);
  NMEA2000.SendMsg(N2kMsg);

}


void sendEngine() {
  tN2kMsg N2kMsgDynamicPort;
  tN2kMsg N2kMsgDynamicStarboard;
  tN2kMsg N2kMsgRapidPort;
  tN2kMsg N2kMsgRapidStarboard;
/*
  boatData.engines.port.oilPressure += 1;
  if (boatData.engines.port.oilPressure > 800) {
    boatData.engines.port.oilPressure = 0;
  }
  boatData.engines.port.waterTemperature += 0.1;
  if (boatData.engines.port.waterTemperature > 373) {
    boatData.engines.port.waterTemperature = 273;
  }
  SetN2kEngineDynamicParam(N2kMsgDynamicPort, 0,
    boatData.engines.port.oilPressure,
    N2kDoubleNA, // EngineOilTemp
    boatData.engines.port.waterTemperature,
    N2kDoubleNA, // AltenatorVoltage,
    N2kDoubleNA, // FuelRate,
    N2kDoubleNA, // EngineHours
    N2kDoubleNA, // EngineCoolantPress
    N2kDoubleNA // EngineFuelPress
  );
  NMEA2000.SendMsg(N2kMsgDynamicPort);

  boatData.engines.starboard.oilPressure += 1;
  if (boatData.engines.starboard.oilPressure > 800) {
    boatData.engines.starboard.oilPressure = 0;
  }
  boatData.engines.starboard.waterTemperature += 0.1;
  if (boatData.engines.starboard.waterTemperature > 373) {
    boatData.engines.starboard.waterTemperature = 273;
  }
  SetN2kEngineDynamicParam(N2kMsgDynamicStarboard, 1,
    boatData.engines.starboard.oilPressure,
    N2kDoubleNA, // EngineOilTemp
    boatData.engines.starboard.waterTemperature,
    N2kDoubleNA, // AltenatorVoltage,
    N2kDoubleNA, // FuelRate,
    N2kDoubleNA, // EngineHours
    N2kDoubleNA, // EngineCoolantPress
    N2kDoubleNA // EngineFuelPress
  );
  NMEA2000.SendMsg(N2kMsgDynamicStarboard);


  boatData.engines.port.rpm += 10;
  if (boatData.engines.port.rpm > 5000) {
    boatData.engines.port.rpm = 0;
  }

  boatData.engines.starboard.rpm += 10;
  if (boatData.engines.starboard.rpm > 5000) {
    boatData.engines.starboard.rpm = 0;
  }

  boatData.engines.port.trim.angle += 0.1;
  if (boatData.engines.starboard.trim.angle > 21) {
    boatData.engines.starboard.trim.angle = 0;
  }

  boatData.engines.starboard.trim.angle += 0.1;
  if (boatData.engines.starboard.trim.angle > 21) {
    boatData.engines.starboard.trim.angle = 0;
  }
  SetN2kEngineParamRapid(N2kMsgRapidPort, 0, boatData.engines.port.rpm, 0.0, boatData.engines.port.trim.angle);
  NMEA2000.SendMsg(N2kMsgRapidPort);
  // delay(N2K_DELAY_BETWEEN_SEND);
  SetN2kEngineParamRapid(N2kMsgRapidStarboard, 1, boatData.engines.starboard.rpm, 0.0, boatData.engines.starboard.trim.angle);
  NMEA2000.SendMsg(N2kMsgRapidStarboard);
*/
}


void sendN2kFuelConsumption() {
  tN2kMsg N2kMsgPort;
  tN2kMsg N2kMsgStarboard;

  // SetN2kPGN127497(N2kMsgPort, 0, 2, 0.3, 0.4, 0.5);
  // NMEA2000.SendMsg(N2kMsgPort);
  //
  // SetN2kPGN127497(N2kMsgStarboard, 1, 6, 0.7, 0.8, 0.9);
  // NMEA2000.SendMsg(N2kMsgStarboard);

  SetN2kPGN127489(N2kMsgPort, 0, 0, 0, 0, 0, 0.6, 0, 0, 0, 0, 0,
    false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, false
  );
  NMEA2000.SendMsg(N2kMsgPort);
  SetN2kPGN127489(N2kMsgStarboard, 1, 0, 0, 0, 0, 0.7, 0, 0, 0, 0, 0,
    false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, false, false,
    false, false, false, false, false
  );
  NMEA2000.SendMsg(N2kMsgStarboard);

}

void sendN2kSensorData() {
  sendN2kACStatus();
  static int sensorIndex = 0;
  tN2kMsg N2kMsg;
  tN2kMsg N2kMsg_port;
  tN2kMsg N2kMsg_starboard;
  unsigned char test;
  
  switch (sensorIndex) {
    case 0:
      Serial.println("$$$$$$$$$$$$$$$$444");



      SetN2kPGN128006(N2kMsg, 1, 3, 
      N2kThrusterControlDirection_ToStarboard, 
      N2kThrusterControlPower_On,
      N2kThrusterControlRetract_Retract,
      0.98,
      N2kThrusterControlEventOtherDevice,
      225,
      0.34);
      NMEA2000.SendMsg(N2kMsg);
      // boatData.fuel.level = 25;
      // SetN2kFluidLevel(N2kMsg, 1, N2kft_Fuel, boatData.fuel.level, 1200.0);
      // NMEA2000.SendMsg(N2kMsg);
      break;
    case 1:
      // boatData.engines.engineRoomTemperature = 32+273;
      // SetN2kTemperature(N2kMsg, 1, 1, N2kts_EngineRoomTemperature, boatData.engines.engineRoomTemperature);
      // NMEA2000.SendMsg(N2kMsg);
      break;
    case 2:
      // boatData.engines.port.oilPressure = 19000;
      // boatData.engines.port.waterTemperature = 85+273;
      // SetN2kEngineDynamicParam(N2kMsg, 0,
      //   boatData.engines.port.oilPressure,
      //   N2kDoubleNA, // EngineOilTemp
      //   boatData.engines.port.waterTemperature,
      //   N2kDoubleNA, // AltenatorVoltage,
      //   N2kDoubleNA, // FuelRate,
      //   N2kDoubleNA, // EngineHours
      //   N2kDoubleNA, // EngineCoolantPress
      //   N2kDoubleNA // EngineFuelPress
      // );
      // NMEA2000.SendMsg(N2kMsg);
      break;
    case 3:
      // boatData.engines.starboard.oilPressure = 20000;
      // boatData.engines.starboard.waterTemperature = 90+273;
      // SetN2kEngineDynamicParam(N2kMsg, 1,
      //   boatData.engines.starboard.oilPressure,
      //   N2kDoubleNA, // EngineOilTemp
      //   boatData.engines.starboard.waterTemperature,
      //   N2kDoubleNA, // AltenatorVoltage,
      //   N2kDoubleNA, // FuelRate,
      //   N2kDoubleNA, // EngineHours
      //   N2kDoubleNA, // EngineCoolantPress
      //   N2kDoubleNA // EngineFuelPress
      // );
      // NMEA2000.SendMsg(N2kMsg);
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
      Serial.println("attitude");
      boatData.attitude.roll += 1;
      if (boatData.attitude.roll > 20) {
        boatData.attitude.roll = -20;
      }
      // boatData.attitude.pitch = 15;
      // boatData.attitude.roll = 4;
      SetN2kAttitude(N2kMsg, 1, DegToRad(boatData.attitude.yaw), DegToRad(boatData.attitude.pitch), DegToRad(boatData.attitude.roll));
      NMEA2000.SendMsg(N2kMsg);
      break;
    case 10:
      boatData.engines.port.rpm = 3010;
      boatData.engines.starboard.rpm = 3020;
      boatData.engines.port.trim.angle = 4.0/13.0;
      boatData.engines.starboard.trim.angle = 12.0/13.0;
      SetN2kEngineParamRapid(N2kMsg_port, 0, boatData.engines.port.rpm, 0.0, boatData.engines.port.trim.angle);
      NMEA2000.SendMsg(N2kMsg_port);
      // delay(N2K_DELAY_BETWEEN_SEND);
      SetN2kEngineParamRapid(N2kMsg_starboard, 1, boatData.engines.starboard.rpm, 0.0, boatData.engines.starboard.trim.angle);
      NMEA2000.SendMsg(N2kMsg_starboard);
      break;
    case 11:
      // boatData.engines.engineRoomTemperature = 25+273;
      // SetN2kTemperature(N2kMsg, 1, 1, N2kts_MainCabinTemperature, boatData.engines.engineRoomTemperature);
      // NMEA2000.SendMsg(N2kMsg);
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
