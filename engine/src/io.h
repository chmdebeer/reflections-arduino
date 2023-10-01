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

#define I_ONE_WIRE_BUS 49

//20
    // Port 12v
//19
#define I_PORT_ENGINE_OIL A15
//18
#define I_PORT_ENGINE_TEMP A14
//17
#define I_PORT_DRIVE_TRIM A13
//16
  // Vin
//15
#define I_STARBOARD_ENGINE_OIL A12
//14
#define I_STARBOARD_ENGINE_TEMP A11
//13
#define I_STARBOARD_DRIVE_TRIM A10
//12
   // Starboard 12v
//11
   //GND
//10
#define I_FUEL A9
//09
#define I_ENGINE_BILGE_PUMP A7
//08
#define I_AUX_BATTERY A6
//07
#define I_PORT_BATTERY A5
//06
#define I_STARBOARD_BATTERY A4
//05
#define I_STEERING_ANGLE A8
//04 yellow
#define I_PORT_ENGINE_CHECK 10
//03
#define I_PORT_ENGINE_MALFUNCTION 11
//02 yellow
#define I_STARBOARD_ENGINE_CHECK 12
//01
#define I_STARBOARD_ENGINE_MALFUNCTION 13

void setupIO();
bool readIO(BoatData &boatData, SwitchBankInstance instance);
void setIO(BoatData &boatData, SwitchBankInstance instance);
void readSteering(BoatData &boatData);
void readBatteries(BoatData &boatData);
void readTemperatures(BoatData &boatData);
void readFuel(BoatData &boatData);
void readEngines(BoatData &boatData);

#endif
