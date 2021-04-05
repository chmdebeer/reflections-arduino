#ifndef IO_H
#define IO_H

#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48

#define O_FUEL 3
#define O_PORT_BATTERY 4
#define O_PORT_ENGINE_RPM 5
#define O_PORT_ENGINE_OIL 6
#define O_PORT_ENGINE_TEMP 7

#define O_BILGE_BLOWER 8
#define O_BILGE_PUMP 9
#define O_NAVIGATION_LIGHTS 10
#define O_ANCHOR_LIGHTS 11
#define O_ENGINE_ROOM_LIGHTS 12
#define O_ACCESSORIES_RADIO 13

#define NA_14 14
#define NA_15 15

#define O_STARBOARD_BATTERY 16
#define O_STARBOARD_ENGINE_RPM 17
#define O_STARBOARD_ENGINE_OIL 18
#define O_STARBOARD_ENGINE_TEMP 19

#define NA_20 20
#define NA_21 21

#define I_PORT_DRIVE_BOW_DOWN 22
#define I_PORT_DRIVE_BOW_UP 23
#define I_STARBOARD_DRIVE_BOW_DOWN 24
#define I_STARBOARD_DRIVE_BOW_UP 25

#define O_SERVOS 26
#define O_PORT_ENGINE_HOURS 27
#define O_STARBOARD_ENGINE_HOURS 28
#define O_RELAY_4 29
#define O_RELAY_5 30
#define O_RELAY_6 31
#define O_RELAY_7 32
#define O_RELAY_8 33

#define I_BILGE_PUMP 34
#define I_BILGE_BLOWER 35
#define I_NAVIGATION_LIGHTS 36
#define I_ENGINE_ROOM_LIGHTS 37

#define I_PORT_START 38
#define O_PORT_START 39
#define I_STARBOARD_START 40
#define O_STARBOARD_START 41

#define O_INSTRUMENT_LIGHTS 44
#define O_BATHROOM_LIGHTS 45
#define O_BERTH_LIGHTS 46
#define O_MOSFET_4 47

#define I_ONE_WIRE_BUS 49

#define I_HORN A15
#define I_ROCM_RADIO A14
#define I_WINDOW_WIPERS A13
#define I_CABIN_LIGHTS A12
#define I_FM_RADIO A11
#define I_WATER_PUMP A10
#define I_REFRIGERATOR A9
#define I_SPOT_LIGHT A8

#define I_PORT_TRIM_BOW_UP A5
#define I_PORT_TRIM_BOW_DOWN A4
#define I_STARBOARD_TRIM_BOW_UP A3
#define I_STARBOARD_TRIM_BOW_DOWN A2

void setupIO();
bool readIO(BoatData &boatData);
void setIO(BoatData &boatData);
bool readTilt(BoatData &boatData);
void setupAC();
void readAC(BoatData &boatData);
void setServos(BoatData &boatData);

#endif
