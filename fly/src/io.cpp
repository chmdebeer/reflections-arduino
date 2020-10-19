#include <Arduino.h>
#include <Bounce2.h>
#include <N2kTypes.h>
#include "BoatData.h"
#include "io.h"
#include "servo.h"
#include <Wire.h>
#include "utils.h"

#define BOUNCE_INTERVAL 25
enum debounce {
  E_PORT_START,
  E_STARBOARD_START,
  E_PORT_DRIVE_BOW_UP,
  E_PORT_DRIVE_BOW_DOWN,
  E_STARBOARD_DRIVE_BOW_UP,
  E_STARBOARD_DRIVE_BOW_DOWN,
  E_PORT_TRIM_BOW_UP,
  E_PORT_TRIM_BOW_DOWN,
  E_STARBOARD_TRIM_BOW_UP,
  E_STARBOARD_TRIM_BOW_DOWN,
  E_BILGE_PUMP,
  E_BILGE_BLOWER,
  E_NAVIGATION_LIGHTS,
  E_WINDOW_WIPERS,
  E_HORN,
  E_SPOTLIGHT_FOG,
  E_SPOTLIGHT_SPOT,
  E_SPOTLIGHT_UP,
  E_SPOTLIGHT_DOWN,
  E_SPOTLIGHT_LEFT,
  E_SPOTLIGHT_RIGHT,
  E_ITEMS
};

Bounce * buttons = new Bounce[E_ITEMS];

enum servo {
  S_PORT_ENGINE_RPM,
  S_PORT_ENGINE_OIL,
  S_PORT_ENGINE_TEMPERATURE,
  S_STARBOARD_ENGINE_RPM,
  S_STARBOARD_ENGINE_OIL,
  S_STARBOARD_ENGINE_TEMPERATURE,
  S_ITEMS
};

Servo * servos = new Servo[S_ITEMS];

void setupIO() {
  buttons[E_PORT_START].attach(I_PORT_START, INPUT_PULLUP);
  buttons[E_STARBOARD_START].attach(I_STARBOARD_START, INPUT_PULLUP);

  buttons[E_PORT_DRIVE_BOW_UP].attach(I_PORT_DRIVE_BOW_UP, INPUT_PULLUP);
  buttons[E_PORT_DRIVE_BOW_DOWN].attach(I_PORT_DRIVE_BOW_DOWN, INPUT_PULLUP);
  buttons[E_STARBOARD_DRIVE_BOW_UP].attach(I_STARBOARD_DRIVE_BOW_UP, INPUT_PULLUP);
  buttons[E_STARBOARD_DRIVE_BOW_DOWN].attach(I_STARBOARD_DRIVE_BOW_DOWN, INPUT_PULLUP);

  buttons[E_PORT_TRIM_BOW_UP].attach(I_PORT_TRIM_BOW_UP, INPUT_PULLUP);
  buttons[E_PORT_TRIM_BOW_DOWN].attach(I_PORT_TRIM_BOW_DOWN, INPUT_PULLUP);
  buttons[E_STARBOARD_TRIM_BOW_UP].attach(I_STARBOARD_TRIM_BOW_UP, INPUT_PULLUP);
  buttons[E_STARBOARD_TRIM_BOW_DOWN].attach(I_STARBOARD_TRIM_BOW_DOWN, INPUT_PULLUP);

  buttons[E_BILGE_PUMP].attach(I_BILGE_PUMP, INPUT_PULLUP);
  buttons[E_BILGE_BLOWER].attach(I_BILGE_BLOWER, INPUT_PULLUP);
  buttons[E_NAVIGATION_LIGHTS].attach(I_NAVIGATION_LIGHTS, INPUT_PULLUP);

  buttons[E_HORN].attach(I_HORN, INPUT_PULLUP);

  buttons[E_WINDOW_WIPERS].attach(I_WINDOW_WIPERS, INPUT_PULLUP);

  buttons[E_SPOTLIGHT_FOG].attach(I_SPOTLIGHT_FOG, INPUT);
  buttons[E_SPOTLIGHT_SPOT].attach(I_SPOTLIGHT_SPOT, INPUT);
  buttons[E_SPOTLIGHT_UP].attach(I_SPOTLIGHT_UP, INPUT_PULLUP);
  buttons[E_SPOTLIGHT_DOWN].attach(I_SPOTLIGHT_DOWN, INPUT_PULLUP);
  buttons[E_SPOTLIGHT_LEFT].attach(I_SPOTLIGHT_LEFT, INPUT_PULLUP);
  buttons[E_SPOTLIGHT_RIGHT].attach(I_SPOTLIGHT_RIGHT, INPUT_PULLUP);

  for (int e = 0; e < E_ITEMS; e++) {
    buttons[e].interval(BOUNCE_INTERVAL);
  }

  pinMode(O_BILGE_BLOWER, OUTPUT);
  pinMode(O_NAVIGATION_LIGHTS, OUTPUT);
  pinMode(O_ANCHOR_LIGHTS, OUTPUT);
  pinMode(O_HORN_ONE, OUTPUT);
  pinMode(O_HORN_TWO, OUTPUT);

  pinMode(O_RELAY_4, OUTPUT);
  pinMode(O_RELAY_3, OUTPUT);
  pinMode(O_RELAY_2, OUTPUT);
  pinMode(O_INSTRUMENT_SERVOS, OUTPUT);

  pinMode(O_MOSFET_1, OUTPUT);
  pinMode(O_MOSFET_2, OUTPUT);
  pinMode(O_MOSFET_3, OUTPUT);
  pinMode(O_INSTRUMENT_LIGHTS, OUTPUT);

  pinMode(O_PORT_START, OUTPUT);
  pinMode(O_STARBOARD_START, OUTPUT);

  servos[S_PORT_ENGINE_RPM].attach(O_PORT_ENGINE_RPM);
  servos[S_PORT_ENGINE_OIL].attach(O_PORT_ENGINE_OIL);
  servos[S_PORT_ENGINE_TEMPERATURE].attach(O_PORT_ENGINE_TEMPERATUR);
  servos[S_STARBOARD_ENGINE_RPM].attach(O_STARBOARD_ENGINE_RPM);
  servos[S_STARBOARD_ENGINE_OIL].attach(O_STARBOARD_ENGINE_OIL);
  servos[S_STARBOARD_ENGINE_TEMPERATURE].attach(O_STARBOARD_ENGINE_TEMPERATUR);

  for (int s = 0; s < S_ITEMS; s++) {
    servos[s].writeMicroseconds(1500);
  }
}

bool readIO(BoatData &boatData) {
  bool newIO = false;

  newIO |= readStartButton(buttons[E_PORT_START], boatData.engines.port);
  newIO |= readStartButton(buttons[E_STARBOARD_START], boatData.engines.starboard);

  newIO |= readMomentaryButton(buttons[E_PORT_DRIVE_BOW_UP], boatData.tilt.port.bowUp, true);
  newIO |= readMomentaryButton(buttons[E_PORT_DRIVE_BOW_DOWN], boatData.tilt.port.bowDown, true);
  newIO |= readMomentaryButton(buttons[E_STARBOARD_DRIVE_BOW_UP], boatData.tilt.starboard.bowUp, true);
  newIO |= readMomentaryButton(buttons[E_STARBOARD_DRIVE_BOW_DOWN], boatData.tilt.starboard.bowDown, true);

  newIO |= readMomentaryButton(buttons[E_PORT_TRIM_BOW_UP], boatData.trim.port.bowUp, true);
  newIO |= readMomentaryButton(buttons[E_PORT_TRIM_BOW_DOWN], boatData.trim.port.bowDown, true);
  newIO |= readMomentaryButton(buttons[E_STARBOARD_TRIM_BOW_UP], boatData.trim.starboard.bowUp, true);
  newIO |= readMomentaryButton(buttons[E_STARBOARD_TRIM_BOW_DOWN], boatData.trim.starboard.bowDown, true);

  buttons[E_BILGE_PUMP].update();
  if (buttons[E_BILGE_PUMP].fell()) {
    newIO |= true;
    if ((boatData.bilgePumps.engineRoom.on == N2kOnOff_Off) && (boatData.bilgePumps.midship.on == N2kOnOff_Off)) {
      boatData.bilgePumps.engineRoom.on = N2kOnOff_On;
    } else if ((boatData.bilgePumps.engineRoom.on == N2kOnOff_On) && (boatData.bilgePumps.midship.on == N2kOnOff_Off)) {
      boatData.bilgePumps.midship.on = N2kOnOff_On;
    } else {
      boatData.bilgePumps.engineRoom.on = N2kOnOff_Off;
      boatData.bilgePumps.midship.on = N2kOnOff_Off;
    }
  }

  buttons[E_BILGE_BLOWER].update();
  if (buttons[E_BILGE_BLOWER].fell()) {
    newIO |= true;
    if ((boatData.blowers.one == N2kOnOff_Off) && (boatData.blowers.two == N2kOnOff_Off)) {
      boatData.blowers.one = N2kOnOff_On;
    } else if ((boatData.blowers.one == N2kOnOff_On) && (boatData.blowers.two == N2kOnOff_Off)) {
      boatData.blowers.two = N2kOnOff_On;
    } else {
      boatData.blowers.one = N2kOnOff_Off;
      boatData.blowers.two = N2kOnOff_Off;
    }
  }

  buttons[E_NAVIGATION_LIGHTS].update();
  if (buttons[E_NAVIGATION_LIGHTS].fell()) {
    newIO |= true;
    if ((boatData.lights.navigation == N2kOnOff_Off) && (boatData.lights.anchor == N2kOnOff_Off)) {
      boatData.lights.navigation = N2kOnOff_On;
    } else if ((boatData.lights.navigation == N2kOnOff_On) && (boatData.lights.anchor == N2kOnOff_Off)) {
      boatData.lights.anchor = N2kOnOff_On;
    } else if ((boatData.lights.navigation == N2kOnOff_On) && (boatData.lights.anchor == N2kOnOff_On)) {
      boatData.lights.navigation = N2kOnOff_Off;
    } else {
      boatData.lights.navigation = N2kOnOff_Off;
      boatData.lights.anchor = N2kOnOff_Off;
    }
  }

  buttons[E_HORN].update();
  if (buttons[E_HORN].fell()) {
    newIO |= true;
    boatData.utilities.horn.one = N2kOnOff_On;
    boatData.utilities.horn.two = N2kOnOff_On;
  }
  if (buttons[E_HORN].rose()) {
    newIO |= true;
    boatData.utilities.horn.one = N2kOnOff_Off;
    boatData.utilities.horn.two = N2kOnOff_Off;
  }

  buttons[E_WINDOW_WIPERS].update();
  if (buttons[E_WINDOW_WIPERS].fell()) {
    newIO |= true;
    if ((boatData.utilities.wipers.port != N2kOnOff_On) || (boatData.utilities.wipers.starboard != N2kOnOff_On)) {
      boatData.utilities.wipers.port = N2kOnOff_On;
      boatData.utilities.wipers.starboard = N2kOnOff_On;
    } else {
      boatData.utilities.wipers.port = N2kOnOff_Off;
      boatData.utilities.wipers.starboard = N2kOnOff_Off;
    }
  }

  newIO |= readMomentaryButton(buttons[E_SPOTLIGHT_FOG], boatData.lights.spotlight.fog, false);
  newIO |= readMomentaryButton(buttons[E_SPOTLIGHT_SPOT], boatData.lights.spotlight.spot, false);
  newIO |= readMomentaryButton(buttons[E_SPOTLIGHT_UP], boatData.lights.spotlight.up, false);
  newIO |= readMomentaryButton(buttons[E_SPOTLIGHT_DOWN], boatData.lights.spotlight.down, false);
  newIO |= readMomentaryButton(buttons[E_SPOTLIGHT_LEFT], boatData.lights.spotlight.left, false);
  newIO |= readMomentaryButton(buttons[E_SPOTLIGHT_RIGHT], boatData.lights.spotlight.right, false);

  return newIO;
}

void setIO(BoatData &boatData) {
  digitalWrite(O_RELAY_4, HIGH);
  digitalWrite(O_RELAY_3, HIGH);
  digitalWrite(O_RELAY_2, HIGH);
  digitalWrite(O_MOSFET_1, LOW);
  digitalWrite(O_MOSFET_2, LOW);
  digitalWrite(O_MOSFET_3, LOW);

  digitalWrite(O_NAVIGATION_LIGHTS, !(boatData.lights.navigation == N2kOnOff_On));
  digitalWrite(O_ANCHOR_LIGHTS, !(boatData.lights.anchor == N2kOnOff_On));
  digitalWrite(O_INSTRUMENT_SERVOS, !((boatData.engines.port.ignition == N2kOnOff_On) || (boatData.engines.starboard.ignition == N2kOnOff_On)));

  digitalWrite(O_INSTRUMENT_LIGHTS, boatData.lights.instruments == N2kOnOff_On);

  digitalWrite(O_HORN_ONE, !(boatData.utilities.horn.one == N2kOnOff_On));
  digitalWrite(O_HORN_TWO, !(boatData.utilities.horn.two == N2kOnOff_On));

  digitalWrite(O_BILGE_BLOWER, ((boatData.blowers.one == N2kOnOff_On) || (boatData.blowers.two == N2kOnOff_On)));
}

void setServos(BoatData &boatData) {
  servos[S_PORT_ENGINE_RPM].writeMicroseconds(map(boatData.engines.port.rpm, 0, 6000, 1000, 2000));
  servos[S_PORT_ENGINE_OIL].writeMicroseconds(map(boatData.engines.port.oilPressure, 0, 549172, 1000, 2000));
  servos[S_PORT_ENGINE_TEMPERATURE].writeMicroseconds(map(boatData.engines.port.waterTemperature, 40, 120, 1000, 2000));
  servos[S_STARBOARD_ENGINE_RPM].writeMicroseconds(map(boatData.engines.starboard.rpm, 0, 6000, 1000, 2000));
  servos[S_STARBOARD_ENGINE_OIL].writeMicroseconds(map(boatData.engines.starboard.oilPressure, 0, 549172, 1000, 2000));
  servos[S_STARBOARD_ENGINE_TEMPERATURE].writeMicroseconds(map(boatData.engines.starboard.waterTemperature, 40, 120, 1000, 2000));
}
