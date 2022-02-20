/*
N2kReflections.h
*/

#ifndef _N2kReflections_H_
#define _N2kReflections_H_

#include "N2kMsg.h"
#include <stdint.h>


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
