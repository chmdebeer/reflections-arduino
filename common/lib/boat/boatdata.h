#ifndef BOAT_DATA_H
#define BOAT_DATA_H

#define N2K_DELAY_BETWEEN_SEND 10

enum OffOn {
	OFF = 0,
	ON = 1
};

enum SwitchBankInstance {
  E_ZERO,
  E_IGNITION_START,
  E_ENGINE_STATUS,
  E_POWER_TRIM,
  E_TRIM,
  E_NUTRASALT,
  E_LIGHTS,
  E_SPOTLIGHT,
  E_UTILITIES_CABIN,
  E_UTILITIES_BILGE,
  E_UTILITIES_ENGINE_ROOM,
  E_SWITCH_BANK_INSTANCES
};

enum TrimDirection {
  E_TRIM_OFF,
  E_TRIM_UP,
  E_TRIM_DOWN
};


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
  tN2kOnOff port;
  tN2kOnOff starboard;
} Wipers;

typedef struct {
  tN2kOnOff one;
  tN2kOnOff two;
} Horn;

typedef struct {
  double angle;
  TrimDirection direction;
  unsigned long start;
  tN2kOnOff bowUp;
  tN2kOnOff bowDown;
} BowUpDown;

typedef struct {
  BowUpDown port;
  BowUpDown starboard;
} Trim;

typedef struct {
  tN2kOnOff ignition;
  tN2kOnOff start;
  bool ready;
  tN2kOnOff nutraSalt;
  int rpm;
  int oilPressure;
  int waterTemperature;
  BowUpDown trim;
  double steering;
  tN2kOnOff check;
  tN2kOnOff malfunction;
} Engine;

typedef struct {
  Engine port;
  Engine starboard;
  double engineRoomTemperatureBottom;
  double engineRoomTemperatureTop;
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
  unsigned char cabinIntensity;
  unsigned char berthIntensity;
  unsigned char headIntensity;
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
  Horn horn;
  Wipers wipers;
  tN2kOnOff waterPump;
  tN2kOnOff waterHeater;
  tN2kOnOff cabinHeaterFan;
  tN2kOnOff blackwaterAirPump;
  tN2kOnOff macerator;
  tN2kOnOff showerDrainPump;
  tN2kOnOff rocmRadio;
  tN2kOnOff fmRadio;
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
  double insideTemperature;
  double cabinTemperature;
  double belowDeckTemperature;
} Environment;

typedef struct {
  unsigned int bridgeRestartCount;
  unsigned int flybridgeRestartCount;
  unsigned int egnineRoomRestartCount;
  unsigned int bildgeRestartCount;
} System;

typedef struct {
  Fuel fuel;
  Engines engines;
  Trim trim;
  Batteries batteries;
  tN2kOnOff blower;
  BilgePumps bilgePumps;
  Lights lights;
  Utilities utilities;
  Attitude attitude;
  AC ac;
  Environment environment;
  System system;
} BoatData;

void clearBoatData(BoatData &boatData);
uint64_t binaryStatusFromBoatData(unsigned char instance, BoatData boatData);
void boatDataFromBinaryStatus(unsigned char instance, uint64_t binaryStatus, BoatData &boatData);


#endif
