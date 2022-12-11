#ifndef IO_H
#define IO_H

#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48

#define I_DOOR_LOCK 18

#define I_PORT_ENGINE_RPM 20
#define I_STARBOARD_ENGINE_RPM 21

// Nutra-salt port
// Nutra-salt starboard

#define O_PORT_NUTRASALT 22
#define O_STARBOARD_NUTRASALT 23
#define O_IGNITION 24
#define O_RELAY_4 25
#define O_PORT_DRIVE_BOW_UP 26
#define O_PORT_DRIVE_BOW_DOWN 27
#define O_STARBOARD_DRIVE_BOW_UP 28
#define O_STARBOARD_DRIVE_BOW_DOWN 29

#define O_ENGINE_ROOM_LIGHTS 30
#define O_AFT_DECK_LIGHTS 31
#define O_STERN_LIGHT 32
#define O_DOOR_LOCK 33

#define O_PORT_IGNITION 34
#define O_STARBOARD_IGNITION 35
#define O_PORT_START 36
#define O_STARBOARD_START 37

#define O_PORT_TRIM 38
#define O_STARBOARD_TRIM 39
#define O_TRIM_BOW_DOWN 40
#define O_TRIM_BOW_UP 41

#define O_CABIN_HEATER_FAN 42
#define O_BILGE_BLOWER_1 43
#define O_WATER_PUMP 44
#define O_ENGINE_BILGE_PUMP 45

#define O_TIP120_13 46

#define I_ONE_WIRE_BUS 47

//20
    // Port 12v
//19
#define I_PORT_ENGINE_OIL A14
//18
#define I_PORT_ENGINE_TEMP A12
//17
#define I_PORT_DRIVE_TRIM A15
//16
  // Vin
//15
#define I_STARBOARD_ENGINE_OIL A13
//14
#define I_STARBOARD_ENGINE_TEMP A9
//13
#define I_STARBOARD_DRIVE_TRIM A7
//12
   // Starboard 12v
//11
   //GND
//10
#define I_FUEL A0
//09
#define I_ENGINE_BILGE_PUMP A2
//08
#define I_AUX_BATTERY A1
//07
#define I_PORT_BATTERY A8
//06
#define I_STARBOARD_BATTERY A10
//05
#define I_STEERING_ANGLE A11
//04 yellow
#define I_PORT_ENGINE_CHECK A5
//03
#define I_PORT_ENGINE_MALFUNCTION A6
//02 yellow
#define I_STARBOARD_ENGINE_CHECK A4
//01
#define I_STARBOARD_ENGINE_MALFUNCTION A3

void setupIO();
bool readIO(BoatData &boatData, SwitchBankInstance instance);
void setIO(BoatData &boatData, SwitchBankInstance instance);
void readSteering(BoatData &boatData);
void readBatteries(BoatData &boatData);
void readTemperatures(BoatData &boatData);
void readFuel(BoatData &boatData);
void readEngines(BoatData &boatData);

#endif
