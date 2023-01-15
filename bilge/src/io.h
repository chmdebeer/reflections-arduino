#ifndef IO_H
#define IO_H

#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 25

#define O_SHOWER_PUMP 26
#define O_REFRIGERATOR 27
#define O_MACERATOR 28
#define O_MIDSHIP_BILDGE_LIGHTS 29

#define O_REALY_2_1 32 //relay 2 - 1
#define O_REALY_2_2 33
#define O_REALY_2_3 34
#define O_REALY_2_4 35
#define O_REALY_2_5 36
#define O_BOW_THRUSTER_POWER 37
#define O_BOW_THRUSTER_PORT 38
#define O_BOW_THRUSTER_STARBOARD 39 // relay 2 - 8

#define O_SPOT_LIGHT_RIGHT 40 //realy 1 - 1
#define O_SPOT_LIGHT_LEFT 41
#define O_SPOT_LIGHT_UP 42
#define O_SPOT_LIGHT_DOWN 43
#define O_SPOT_LIGHT_SPOT 44
#define O_SPOT_LIGHT_FOG 45
#define O_MIDSHIP_BILGE 46
#define O_BLACKWATER_AIR_PUMP 47 //relay 1 - 8

#define I_ONE_WIRE_BUS 49
#define I_MIDSHIP_BILGE A15

void setupIO();
bool readIO(BoatData &boatData, SwitchBankInstance instance);
void setIO(BoatData &boatData, SwitchBankInstance instance);
void setThruster(BoatData &boatData);
void readSensors(BoatData &boatData);

#endif
