#ifndef IO_H
#define IO_H

#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48

#define O_INSTRUMENT_LIGHTS 3
#define O_FLYDECK_LIGHT 4
#define O_MOSFET_2 5
#define O_MOSFET_1 6

#define O_BOW_THRUSTER_ON 16
#define I_BOW_THRUSTER_ON 17
#define I_BOW_THRUSTER_PORT 18
#define I_BOW_THRUSTER_STARBOARD 19

#define O_NAVIGATION_LIGHTS 22
#define O_ANCHOR_LIGHTS 23
#define O_HORN_ONE 24
#define O_HORN_TWO 25
#define O_RELAY_4 26
#define O_RELAY_3 27
#define O_RELAY_2 28
#define O_RELAY_1 29

#define I_DRIVE_BOW_DOWN 32
#define I_DRIVE_BOW_UP 33
#define I_PORT_DRIVE_BOW_UP 34
#define I_PORT_DRIVE_BOW_DOWN 35
#define I_STARBOARD_DRIVE_BOW_UP 36
#define I_STARBOARD_DRIVE_BOW_DOWN 37

#define I_PORT_START 38
#define O_PORT_START 39
#define I_STARBOARD_START 40
#define O_STARBOARD_START 41

#define I_SPOTLIGHT_FOG A15
#define I_SPOTLIGHT_SPOT A14
#define I_SPOTLIGHT_UP A13
#define I_SPOTLIGHT_DOWN A12
#define I_SPOTLIGHT_LEFT A11
#define I_SPOTLIGHT_RIGHT A10

#define I_A9 A9
#define I_A8 A8
#define I_A7 A7
#define I_TRIM_BOW_UP A6
#define I_PORT_TRIM_BOW_UP A5
#define I_PORT_TRIM_BOW_DOWN A4
#define I_STARBOARD_TRIM_BOW_UP A3
#define I_STARBOARD_TRIM_BOW_DOWN A2
#define I_TRIM_BOW_DOWN A1
#define I_A0 A0


void setupIO();
bool readIO(BoatData &boatData, SwitchBankInstance instance);
void setIO(BoatData &boatData, SwitchBankInstance instance);

#endif
