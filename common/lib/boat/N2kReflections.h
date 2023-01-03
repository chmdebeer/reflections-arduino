/*
N2kReflections.h
*/

#ifndef _N2kReflections_H_
#define _N2kReflections_H_

#include "N2kMsg.h"
#include <stdint.h>

enum tN2kACLine {
  N2kACL_Line1=0,
  N2kACL_Line2=1,
  N2kACL_Line3=2,
  N2kACL_Reserved=3
};

enum tN2kACAcceptability {
  N2kACA_BadLevel=0,
  N2kACA_BadFrequency=1,
  N2kACA_BeingQualified=2,
  N2kACA_Good=3
};

//*****************************************************************************
// AC Detailed Status
// Input:
//  - Instance            AC instance.
//  - Number of Lines
//  - Line
//  - Acceptability
//  - Reserved
//  - Voltage
//  - Current
//  - Frequency
//  - Breakersize
//  - Real Power
//  - Reactive Power
//  - Power Factor
void SetN2kPGN127503(tN2kMsg &N2kMsg, unsigned char Instance, unsigned char NumberOfLines,
                     tN2kACLine ACLine, tN2kACAcceptability ACAcceptability,
                     double Voltage, double Current, double Frequency,
                     double BreakerSize, double RealPower, double ReactivePower, double PowerFactor);

inline void SetN2kACStatus(tN2kMsg &N2kMsg, unsigned char Instance, unsigned char NumberOfLines,
                     tN2kACLine ACLine, tN2kACAcceptability ACAcceptability,
                     double Voltage, double Current, double Frequency,
                     double BreakerSize, double RealPower, double ReactivePower, double PowerFactor) {
  SetN2kPGN127503(N2kMsg,Instance,NumberOfLines,ACLine,ACAcceptability,Voltage,Current,Frequency,BreakerSize,RealPower,ReactivePower,PowerFactor);
}

bool ParseN2kPGN127503(const tN2kMsg &N2kMsg, unsigned char &Instance, unsigned char &NumberOfLines,
                     tN2kACLine &ACLine, tN2kACAcceptability &ACAcceptability,
                     double &Voltage, double &Current, double &Frequency,
                     double &BreakerSize, double &RealPower, double &ReactivePower, double &PowerFactor);

inline bool ParseN2kACStatus(const tN2kMsg &N2kMsg, unsigned char &Instance, unsigned char &NumberOfLines,
                     tN2kACLine &ACLine, tN2kACAcceptability &ACAcceptability,
                     double &Voltage, double &Current, double &Frequency,
                     double &BreakerSize, double &RealPower, double &ReactivePower, double &PowerFactor) {
  return ParseN2kPGN127503(N2kMsg,Instance,NumberOfLines,ACLine,ACAcceptability,Voltage,Current,Frequency,BreakerSize,RealPower,ReactivePower,PowerFactor);
}

enum tN2kThrusterControlDirection {
  N2kThrusterControlDirection_Off=0,
  N2kThrusterControlDirection_Ready=1,
  N2kThrusterControlDirection_ToPort=2,
  N2kThrusterControlDirection_ToStarboard=3
};

enum tN2kThrusterControlPower {
  N2kThrusterControlPower_Off=0,
  N2kThrusterControlPower_On=1
};

enum tN2kThrusterControlRetract {
  N2kThrusterControlRetract_Off=0,
  N2kThrusterControlRetract_Extend=1,
  N2kThrusterControlRetract_Retract=2,
  N2kThrusterControlRetract_Reserved=3
};

enum tN2kThrusterControlEvent {
  N2kThrusterControlEventOtherDevice=0,
  N2kThrusterControlEventTooFast=1
};


void SetN2kPGN128006(tN2kMsg &N2kMsg, unsigned char SID, unsigned char Identifier, 
  tN2kThrusterControlDirection Direction,
  tN2kThrusterControlPower Power,
  tN2kThrusterControlRetract Retract,
  double Speed,
  tN2kThrusterControlEvent Event,
  unsigned char Timeout,
  double Azimuth
);

inline void SetN2kThrusterControl(tN2kMsg &N2kMsg, unsigned char SID, unsigned char Identifier, 
  tN2kThrusterControlDirection Direction,
  tN2kThrusterControlPower Power,
  tN2kThrusterControlRetract Retract,
  double Speed,
  tN2kThrusterControlEvent Event,
  unsigned char Timeout,
  double Azimuth) {
  SetN2kPGN128006(N2kMsg, SID, Identifier, Direction, Power, Retract, Speed, Event, Timeout, Azimuth);
}

bool ParseN2kPGN128006(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &Identifier, 
  tN2kThrusterControlDirection &Direction,
  tN2kThrusterControlPower &Power,
  tN2kThrusterControlRetract &Retract,
  double &Speed,
  tN2kThrusterControlEvent &Event,
  unsigned char &Timeout,
  double &Azimuth);
inline bool ParseN2kThrusterControl(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &Identifier, 
  tN2kThrusterControlDirection &Direction,
  tN2kThrusterControlPower &Power,
  tN2kThrusterControlRetract &Retract,
  double &Speed,
  tN2kThrusterControlEvent &Event,
  unsigned char &Timeout,
  double &Azimuth) {
  return ParseN2kPGN128006(N2kMsg, SID, Identifier, Direction, Power, Retract, Speed, Event, Timeout, Azimuth);
}

void SetN2kReflectionsPGN130980(tN2kMsg &N2kMsg, unsigned char Instance, uint16_t ResetCount);

inline void SetN2kReflectionsResetCount(tN2kMsg &N2kMsg, unsigned char Instance, uint16_t ResetCount) {
  SetN2kReflectionsPGN130980(N2kMsg,Instance,ResetCount);
}

bool ParseN2kReflectionsPGN130980(const tN2kMsg &N2kMsg, unsigned char &Instance, uint16_t& ResetCount);
inline bool ParseN2kReflectionsResetCount(const tN2kMsg &N2kMsg, unsigned char &Instance, uint16_t& ResetCount) {
  return ParseN2kReflectionsPGN130980(N2kMsg, Instance, ResetCount);
}

void SetN2kReflectionsPGN130981(tN2kMsg &N2kMsg, unsigned char Instance, uint16_t NutraSaltCount);

inline void SetN2kReflectionsNutraSaltCountdown(tN2kMsg &N2kMsg, unsigned char Instance, uint16_t NutraSaltCountdown) {
  SetN2kReflectionsPGN130981(N2kMsg,Instance,NutraSaltCountdown);
}

bool ParseN2kReflectionsPGN130981(const tN2kMsg &N2kMsg, unsigned char &Instance, uint16_t& NutraSaltCountdown);
inline bool ParseN2kReflectionsNutraSaltCountdown(const tN2kMsg &N2kMsg, unsigned char &Instance, uint16_t& NutraSaltCountdown) {
  return ParseN2kReflectionsPGN130981(N2kMsg, Instance, NutraSaltCountdown);
}

#endif
