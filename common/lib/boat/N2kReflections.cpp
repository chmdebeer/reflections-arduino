/*
N2kReflections.cpp
*/

#include "N2kTypes.h"
#include "N2kReflections.h"
#include <string.h>

void SetN2kReflectionsPGN130980(tN2kMsg &N2kMsg, unsigned char Instance, uint16_t ResetCount) {
    N2kMsg.SetPGN(130980L);
    N2kMsg.Priority=7;
    N2kMsg.AddByte(Instance);
    N2kMsg.Add2ByteUInt(ResetCount);
    N2kMsg.AddByte(0xff);
    N2kMsg.AddByte(0xff);
    N2kMsg.AddByte(0xff);
}

bool ParseN2kReflectionsPGN130980(const tN2kMsg &N2kMsg, unsigned char &Instance, uint16_t& ResetCount) {
  if (N2kMsg.PGN!=130980L) return false;
  int Index=0;
  Instance=N2kMsg.GetByte(Index);
  ResetCount=N2kMsg.Get2ByteUInt(Index);
  return true;
}

void SetN2kReflectionsPGN130981(tN2kMsg &N2kMsg, unsigned char Instance, uint16_t NutraSaltCountdown) {
    N2kMsg.SetPGN(130981L);
    N2kMsg.Priority=7;
    N2kMsg.AddByte(Instance);
    N2kMsg.Add2ByteUInt(NutraSaltCountdown);
    N2kMsg.AddByte(0xff);
    N2kMsg.AddByte(0xff);
    N2kMsg.AddByte(0xff);
}

bool ParseN2kReflectionsPGN130981(const tN2kMsg &N2kMsg, unsigned char &Instance, uint16_t& NutraSaltCountdown) {
  if (N2kMsg.PGN!=130981L) return false;
  int Index=0;
  Instance=N2kMsg.GetByte(Index);
  NutraSaltCountdown=N2kMsg.Get2ByteUInt(Index);
  return true;
}
