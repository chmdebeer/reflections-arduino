#include <Arduino.h>
#include <N2kTypes.h>
#include "BoatData.h"
#include <io.h>
#include <frequency.h>

long previousMillis = 0; // will store last time of the cycle end

volatile unsigned long portDuration = 0;
volatile unsigned int portPulseCount = 0;
volatile unsigned long portPreviousMicros = 0;

volatile unsigned long starboardDuration = 0;
volatile unsigned int starboardPulseCount = 0;
volatile unsigned long starboardPreviousMicros = 0;

double portFrequency = 0.0;
double starboardFrequency = 0.0;

void portInthandler();
void starboardInthandler();

void setupFrequency()
{
  pinMode(I_PORT_ENGINE_RPM, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(I_PORT_ENGINE_RPM), portInthandler, FALLING);

  pinMode(I_STARBOARD_ENGINE_RPM, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(I_STARBOARD_ENGINE_RPM), starboardInthandler, FALLING);
}

bool loopFrequency(BoatData &boatData)
{
  unsigned long currentMillis = millis();
  unsigned long _duration;
  unsigned long _pulsecount;
  double _portFrequency = 0.0;
  double _starboardFrequency = 0.0;
  bool result = false;

  if (currentMillis - previousMillis >= MAIN_PERIOD) {
    previousMillis = currentMillis;

    if ((portDuration > 0) && (portPulseCount > 0)) {
      _duration = portDuration;
      _pulsecount = portPulseCount;
      portDuration = 0; // clear counters
      portPulseCount = 0;
      _portFrequency = (1e6 / double(_duration)) * _pulsecount;
    } else {
      _portFrequency = 0.0;
      portDuration = 0;
      portPulseCount = 0;
    }

    if ((starboardDuration > 0) && (starboardPulseCount > 0)) {
      _duration = starboardDuration;
      _pulsecount = starboardPulseCount;
      starboardDuration = 0;
      starboardPulseCount = 0;
      _starboardFrequency = (1e6 / double(_duration)) * _pulsecount;
    } else {
      _starboardFrequency = 0.0;
      starboardDuration = 0;
      starboardPulseCount = 0;
    }

    if ((abs(portFrequency - _portFrequency) > 0.3) || (abs(starboardFrequency - _starboardFrequency) > 0.3)) {
      result = true;
      portFrequency = _portFrequency;
      starboardFrequency = _starboardFrequency;
    }

    boatData.engines.port.rpm = ((portFrequency*60.0) / 4.0);
    boatData.engines.starboard.rpm = ((starboardFrequency*60.0) / 4.0);

  }
  return result;
}

void portInthandler()
{
  unsigned long currentMicros = micros();
  portDuration += currentMicros - portPreviousMicros;
  portPreviousMicros = currentMicros;
  portPulseCount++;
}

void starboardInthandler()
{
  unsigned long currentMicros = micros();
  starboardDuration += currentMicros - starboardPreviousMicros;
  starboardPreviousMicros = currentMicros;
  starboardPulseCount++;
}
