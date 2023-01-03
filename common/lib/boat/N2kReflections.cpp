/*
N2kReflections.cpp
*/

#include "N2kTypes.h"
#include "N2kReflections.h"
#include <string.h>

//*****************************************************************************
// AC Detailed Status
//
void SetN2kPGN127503(tN2kMsg &N2kMsg, unsigned char Instance, unsigned char NumberOfLines,
                     tN2kACLine ACLine, tN2kACAcceptability ACAcceptability,
                     double Voltage, double Current, double Frequency,
                     double BreakerSize, double RealPower, double ReactivePower, double PowerFactor) {
    N2kMsg.SetPGN(127503L);
    N2kMsg.Priority=6;
    N2kMsg.AddByte(Instance);
    N2kMsg.AddByte(NumberOfLines);
    N2kMsg.AddByte((ACLine&0x03) | ((ACAcceptability&0x03) << 2));
    N2kMsg.Add2ByteUDouble(Voltage,0.01);
    N2kMsg.Add2ByteUDouble(Current,0.1);
    N2kMsg.Add2ByteUDouble(Frequency,0.01);
    N2kMsg.Add2ByteUDouble(BreakerSize,0.1);
    N2kMsg.Add4ByteUDouble(RealPower,0.1); // 1
    N2kMsg.Add4ByteUDouble(ReactivePower,0.1); // 1
    N2kMsg.Add1ByteUDouble(PowerFactor,0.01);
}

//*****************************************************************************
bool ParseN2kPGN127503(const tN2kMsg &N2kMsg, unsigned char &Instance, unsigned char &NumberOfLines,
                     tN2kACLine &ACLine, tN2kACAcceptability &ACAcceptability,
                     double &Voltage, double &Current, double &Frequency,
                     double &BreakerSize, double &RealPower, double &ReactivePower, double &PowerFactor){
  if (N2kMsg.PGN!=127503L) return false;
  int Index=0;
  Instance=N2kMsg.GetByte(Index);
  NumberOfLines=N2kMsg.GetByte(Index);
  unsigned char Line=N2kMsg.GetByte(Index);
  ACLine=(tN2kACLine)(Line&0x03);
  ACAcceptability=(tN2kACAcceptability)((Line>>2)&0x03);
  Voltage=N2kMsg.Get2ByteUDouble(0.01,Index);
  Current=N2kMsg.Get2ByteUDouble(0.1,Index);
  Frequency=N2kMsg.Get2ByteUDouble(0.01,Index);
  BreakerSize=N2kMsg.Get2ByteUDouble(0.1,Index);
  RealPower=N2kMsg.Get4ByteUDouble(0.1,Index);
  ReactivePower=N2kMsg.Get4ByteUDouble(0.1,Index);
  PowerFactor=N2kMsg.Get1ByteUDouble(0.01,Index);

  return true;
}

void SetN2kPGN128006(tN2kMsg &N2kMsg, unsigned char SID, unsigned char Identifier, 
  tN2kThrusterControlDirection Direction, tN2kThrusterControlPower Power, tN2kThrusterControlRetract Retract,
  double Speed, tN2kThrusterControlEvent Event, unsigned char Timeout, double Azimuth) {
    N2kMsg.SetPGN(128006L);
    N2kMsg.Priority=3;
    N2kMsg.AddByte(SID);
    N2kMsg.AddByte(Identifier);
    N2kMsg.AddByte((unsigned char) (((Retract & 0x03) << 6) | ((Power & 0x03) << 4) | (Direction & 0x0F)));
    N2kMsg.Add1ByteUDouble(Speed, 0.004);
    N2kMsg.AddByte((Event & 0x03));
    N2kMsg.AddByte(Timeout);
    N2kMsg.Add2ByteUDouble(Azimuth, 0.0001);

}

bool ParseN2kPGN128006(const tN2kMsg &N2kMsg, unsigned char &SID, unsigned char &Identifier, 
  tN2kThrusterControlDirection &Direction,
  tN2kThrusterControlPower &Power,
  tN2kThrusterControlRetract &Retract,
  double &Speed,
  tN2kThrusterControlEvent &Event,
  unsigned char  &Timeout,
  double &Azimuth) {

  int Index = 0;
  unsigned char field;

  if (N2kMsg.PGN!=128006L) return false;

  SID = N2kMsg.GetByte(Index);
  Identifier = N2kMsg.GetByte(Index);

  field = N2kMsg.GetByte(Index);
  Direction = (tN2kThrusterControlDirection) (field & 0x0f);
  Power = (tN2kThrusterControlPower) ((field >> 4) & 0x03);
  Retract = (tN2kThrusterControlRetract) ((field >> 6) & 0x03);

  Speed = N2kMsg.Get1ByteUDouble(0.004, Index);

  field = N2kMsg.GetByte(Index);
  Event = (tN2kThrusterControlEvent) (field & 0x03);

  Timeout = N2kMsg.GetByte(Index);
  Azimuth = N2kMsg.Get2ByteUDouble(0.0001, Index);

  return true;
}

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
