#include <Arduino.h>
#include "NMEA2000_CAN.h"       // This will automatically choose right CAN library and create suitable NMEA2000 object
#include "N2kMessages.h"

/*

Engine room
  -> Oil pressure x 2
  -> water temperature x 2
  -> fuel
  -> rmp x 2
  -> battery x 3
  -> ingition x 2
  -> start x 2

  <- throttle x 2
  <- shift x 2
  <- steering x 2
  <- bildge blower x 4 (R)
  <- deck lights (M)
  <- water header (R)
  <- water pump (R)
  <- cabin heater fan (R)
  <- trim tabs ??
  <- bildge pump (R)


Bridge
  -> master switch
  -> ignition x 2
  -> start x 2
  -> horn
  -> water heater
  -> wiper
  -> 110v sense
  ->
  -> Nav lights
  -> Cabin lights (M)

Overhead
  <- wipers
  <- Cabin lights (M)
  <- Oil pressure x 2 (M)
  -> water temperature x 2
  -> fuel
  -> rmp x 2
  -> battery x 3
  -> ingition x 2
  -> start x 2

Flydeck
  <- horn x 2 (M)
  <- Nav lights x 3 (M)
  -> horn

Bildge
  <- shower pump (R)
  <- macerator (R)
  <- bildge pump (R)
  <- fridge (R)
  <- lights (M)
  <- black water air (M)

*/


#define MainPeriod 100

long previousMillis = 0; // will store last time of the cycle end
volatile unsigned long duration=0; // accumulates pulse width
volatile unsigned int pulsecount=0;
volatile unsigned long previousMicros=0;
int interruptPin = 21;
int ledPin = 44;      // LED connected to digital pin 9

void myinthandler(); // interrupt handler

void setup()
{
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(interruptPin), myinthandler, RISING);
}

void loop()
{
  unsigned long currentMillis = millis();
  tone(ledPin, 200);
//  analogWrite(ledPin, 1);

  if (currentMillis - previousMillis >= MainPeriod)
  {
    previousMillis = currentMillis;
    // need to bufferize to avoid glitches
    unsigned long _duration = duration;
    unsigned long _pulsecount = pulsecount;
    duration = 0; // clear counters
    pulsecount = 0;
    float Freq = 1e6 / float(_duration);    //Duration is in uSecond so it is 1e6 / T

    Freq *= _pulsecount; // calculate F
    // output time and frequency data to RS232
    Serial.print("Frequency: ");
    Serial.print(Freq);
    Serial.println("Hz");
  }
}

void myinthandler() // interrupt handler
{
  unsigned long currentMicros = micros();
  duration += currentMicros - previousMicros;
  previousMicros = currentMicros;
  pulsecount++;
}
