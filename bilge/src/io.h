#ifndef IO_H
#define IO_H

#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48

#define O_MIDSHIP_BILDGE_LIGHTS 3
#define O_SHOWER_PUMP 4
#define O_MACERATOR 5
#define O_MOSFET_4 6

#define O_BLACKWATER_AIR_PUMP 22
#define O_MIDSHIP_BILGE 23
#define O_SPOT_LIGHT_FOG 24
#define O_SPOT_LIGHT_SPOT 25
#define O_SPOT_LIGHT_DOWN 26
#define O_SPOT_LIGHT_UP 27
#define O_SPOT_LIGHT_LEFT 28
#define O_SPOT_LIGHT_RIGHT 29

#define I_ONE_WIRE_BUS 46
#define I_MIDSHIP_BILGE A15

void setupIO();
bool readIO(BoatData &boatData);
void setIO(BoatData &boatData);
void readSensors(BoatData &boatData);

#endif
