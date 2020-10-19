#ifndef IO_H
#define IO_H

#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48

#define O_INSTRUMENT_LIGHTS 3
#define O_MOSFET_3 4
#define O_MOSFET_2 5
#define O_MOSFET_1 6

#define O_PORT_ENGINE_RPM 7
#define O_PORT_ENGINE_OIL 8
#define O_PORT_ENGINE_TEMPERATUR 9
#define O_STARBOARD_ENGINE_RPM 10
#define O_STARBOARD_ENGINE_OIL 11
#define O_STARBOARD_ENGINE_TEMPERATUR 12
#define O_13 13
#define O_18 18
#define O_19 19

#define I_NA_SDA 20
#define I_NA_SCL 21

#define O_NAVIGATION_LIGHTS 22
#define O_ANCHOR_LIGHTS 23
#define O_HORN_ONE 24
#define O_HORN_TWO 25
#define O_RELAY_4 26
#define O_RELAY_3 27
#define O_RELAY_2 28
#define O_INSTRUMENT_SERVOS 29

#define I_HORN 30

#define I_WINDOW_WIPERS 31
#define I_NAVIGATION_LIGHTS 32
#define I_BILGE_PUMP 33

#define I_PORT_DRIVE_BOW_UP 34
#define I_PORT_DRIVE_BOW_DOWN 35
#define I_STARBOARD_DRIVE_BOW_UP 36
#define I_STARBOARD_DRIVE_BOW_DOWN 37

#define I_PORT_TRIM_BOW_UP 38
#define I_PORT_TRIM_BOW_DOWN 39
#define I_STARBOARD_TRIM_BOW_UP 40
#define I_STARBOARD_TRIM_BOW_DOWN 41

#define I_PORT_START 42
#define O_PORT_START 43
#define I_STARBOARD_START 44
#define O_STARBOARD_START 45

#define O_BILGE_BLOWER 46
#define I_BILGE_BLOWER 47

#define I_49 49

#define I_NA_50 50
#define I_NA_51 51
#define I_NA_52 52
#define I_NA_53 53

#define I_A0 A0
#define I_A1 A1
#define I_A2 A2
#define I_A3 A3
#define I_A4 A4
#define I_A5 A5
#define I_A6 A6
#define I_A7 A7
#define I_A8 A8
#define I_A9 A9

#define I_SPOTLIGHT_RIGHT A10
#define I_SPOTLIGHT_LEFT A11
#define I_SPOTLIGHT_DOWN A12
#define I_SPOTLIGHT_UP A13
#define I_SPOTLIGHT_SPOT A14
#define I_SPOTLIGHT_FOG A15

void setupIO();
bool readIO(BoatData &boatData);
void setIO(BoatData &boatData);
void setServos(BoatData &boatData);

#endif
