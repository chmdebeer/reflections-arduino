#ifndef BOAT_DATA_H
#define BOAT_DATA_H

#define N2K_DELAY_BETWEEN_SEND 10

enum OffOn {
	OFF = 0,
	ON = 1
};

typedef struct {
  tN2kOnOff ignition;
  tN2kOnOff start;
  bool ready;
  int rpm;
  int oilPressure;
  int waterTemperature;
  int tiltAngle;
} Engine;

typedef struct {
  double port;
  double starboard;
  double auxiliary;
} Batteries;

typedef struct {
  double level;
  double capacity;
} Fuel;

typedef struct {
  Engine port;
  Engine starboard;
  double engineRoomTemperature;
} Engines;

typedef struct {
  tN2kOnOff fog;
  tN2kOnOff spot;
  tN2kOnOff up;
  tN2kOnOff down;
  tN2kOnOff right;
  tN2kOnOff left;
} Spotlight;

typedef struct {
  tN2kOnOff instruments;
  tN2kOnOff cabin;
  tN2kOnOff berth;
  tN2kOnOff head;
  tN2kOnOff aftDeck;
  tN2kOnOff belowDeck;
  tN2kOnOff engineRoom;
  tN2kOnOff anchor;
  tN2kOnOff navigation;
  Spotlight spotlight;
  tN2kOnOff stern;
} Lights;

typedef struct {
  tN2kOnOff on;
  tN2kOnOff floatSwitch;
} BilgePump;

typedef struct {
  BilgePump engineRoom;
  BilgePump midship;
} BilgePumps;

typedef struct {
  tN2kOnOff one;
  tN2kOnOff two;
} Blowers;

typedef struct {
  int angle;
  tN2kOnOff bowUp;
  tN2kOnOff bowDown;
} BowUpDown;

typedef struct {
  BowUpDown port;
  BowUpDown starboard;
} Trim;

typedef struct {
  tN2kOnOff port;
  tN2kOnOff starboard;
} Wipers;

typedef struct {
  tN2kOnOff one;
  tN2kOnOff two;
} Horn;

typedef struct {
  Horn horn;
  Wipers wipers;
  tN2kOnOff waterPump;
  tN2kOnOff waterHeater;
  tN2kOnOff cabinHeaterFan;
  tN2kOnOff blackwaterAirPump;
  tN2kOnOff macerator;
  tN2kOnOff showerDrainPump;
  tN2kOnOff radio;
  tN2kOnOff stereo;
  tN2kOnOff refrigerator;
  tN2kOnOff doorLock;
} Utilities;

typedef struct {
  float pitch;
  float yaw;
  float roll;
} Attitude;

typedef struct {
  double volts;
  double amps;
} AC;

typedef struct {
  double cabinTemperature;
  double belowDeckTemperature;
} Environment;

typedef struct {
  Fuel fuel;
  Engines engines;
  Trim tilt;
  Trim trim;
  Batteries batteries;
  Blowers blowers;
  BilgePumps bilgePumps;
  Lights lights;
  Utilities utilities;
  Attitude attitude;
  AC ac;
  Environment environment;
} BoatData;

void clearBoatData(BoatData &boatData);
uint64_t binaryStatusFromBoatData(unsigned char instance, BoatData boatData);
void boatDataFromBinaryStatus(unsigned char instance, uint64_t binaryStatus, BoatData &boatData);


#endif
