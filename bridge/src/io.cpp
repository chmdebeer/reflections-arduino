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
  E_SPOT_LIGHT,
  E_ENGINE_ROOM_LIGHTS,
  E_CABIN_LIGHTS,
  E_WATER_PUMP,
  E_ROCM_RADIO,
  E_FM_RADIO,
  E_REFRIGERATOR,
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

  buttons[E_ENGINE_ROOM_LIGHTS].attach(I_ENGINE_ROOM_LIGHTS, INPUT_PULLUP);
  buttons[E_CABIN_LIGHTS].attach(I_CABIN_LIGHTS, INPUT_PULLUP);

  buttons[E_WATER_PUMP].attach(I_WATER_PUMP, INPUT_PULLUP);
  buttons[E_ROCM_RADIO].attach(I_ROCM_RADIO, INPUT_PULLUP);
  buttons[E_FM_RADIO].attach(I_FM_RADIO, INPUT_PULLUP);
  buttons[E_REFRIGERATOR].attach(I_REFRIGERATOR, INPUT_PULLUP);
  buttons[E_SPOT_LIGHT].attach(I_SPOT_LIGHT, INPUT_PULLUP);

  for (int e = 0; e < E_ITEMS; e++) {
    buttons[e].interval(BOUNCE_INTERVAL);
  }

  pinMode(O_BILGE_PUMP, OUTPUT);
  pinMode(O_ENGINE_ROOM_LIGHTS, OUTPUT);
  pinMode(O_ACCESSORIES_RADIO, OUTPUT);
  pinMode(O_BILGE_BLOWER, OUTPUT);
  pinMode(O_NAVIGATION_LIGHTS, OUTPUT);
  pinMode(O_ANCHOR_LIGHTS, OUTPUT);

  pinMode(O_PORT_START, OUTPUT);
  pinMode(O_STARBOARD_START, OUTPUT);

  pinMode(O_SERVOS, OUTPUT);
  pinMode(O_PORT_ENGINE_HOURS , OUTPUT);
  pinMode(O_STARBOARD_ENGINE_HOURS , OUTPUT);
  pinMode(O_RELAY_4, OUTPUT);
  pinMode(O_RELAY_5, OUTPUT);
  pinMode(O_RELAY_6, OUTPUT);
  pinMode(O_RELAY_7, OUTPUT);
  pinMode(O_RELAY_8, OUTPUT);

  pinMode(O_INSTRUMENT_LIGHTS, OUTPUT);
  pinMode(O_BATHROOM_LIGHTS, OUTPUT);
  pinMode(O_BATHROOM_LIGHTS, OUTPUT);
  pinMode(O_MOSFET_4, OUTPUT);

  servos[S_PORT_ENGINE_RPM].attach(O_PORT_ENGINE_RPM);
  servos[S_PORT_ENGINE_OIL].attach(O_PORT_ENGINE_OIL);
  // servos[S_PORT_ENGINE_TEMPERATURE].attach(O_PORT_ENGINE_TEMPERATUR);
  servos[S_STARBOARD_ENGINE_RPM].attach(O_STARBOARD_ENGINE_RPM);
  servos[S_STARBOARD_ENGINE_OIL].attach(O_STARBOARD_ENGINE_OIL);
  // servos[S_STARBOARD_ENGINE_TEMPERATURE].attach(O_STARBOARD_ENGINE_TEMPERATUR);

  for (int s = 0; s < S_ITEMS; s++) {
    servos[s].writeMicroseconds(1500);
  }
}

bool readIO(BoatData &boatData) {
  bool newIO = false;

  newIO |= readStartButton(buttons[E_PORT_START], boatData.engines.port);
  newIO |= readStartButton(buttons[E_STARBOARD_START], boatData.engines.starboard);

  newIO |= readMomentaryButton(buttons[E_PORT_DRIVE_BOW_UP], boatData.engines.port.tilt.bowUp, true);
  newIO |= readMomentaryButton(buttons[E_PORT_DRIVE_BOW_DOWN], boatData.engines.port.tilt.bowDown, true);
  newIO |= readMomentaryButton(buttons[E_STARBOARD_DRIVE_BOW_UP], boatData.engines.starboard.tilt.bowUp, true);
  newIO |= readMomentaryButton(buttons[E_STARBOARD_DRIVE_BOW_DOWN], boatData.engines.starboard.tilt.bowDown, true);

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
      boatData.lights.instruments = N2kOnOff_On;
    } else if ((boatData.lights.navigation == N2kOnOff_On) && (boatData.lights.anchor == N2kOnOff_Off)) {
      boatData.lights.anchor = N2kOnOff_On;
      boatData.lights.instruments = N2kOnOff_On;
    } else if ((boatData.lights.navigation == N2kOnOff_On) && (boatData.lights.anchor == N2kOnOff_On)) {
      boatData.lights.navigation = N2kOnOff_Off;
      boatData.lights.instruments = N2kOnOff_On;
    } else {
      boatData.lights.navigation = N2kOnOff_Off;
      boatData.lights.anchor = N2kOnOff_Off;
      boatData.lights.instruments = N2kOnOff_Off;
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

  buttons[E_SPOT_LIGHT].update();
  if (buttons[E_SPOT_LIGHT].fell()) {
    newIO |= true;
    if ((boatData.lights.spotlight.spot == N2kOnOff_Off) && (boatData.lights.spotlight.fog == N2kOnOff_Off)) {
      boatData.lights.spotlight.spot = N2kOnOff_On;
      boatData.lights.spotlight.fog = N2kOnOff_Off;
    } else if ((boatData.lights.spotlight.spot == N2kOnOff_On) && (boatData.lights.spotlight.fog == N2kOnOff_Off)) {
      boatData.lights.spotlight.spot = N2kOnOff_Off;
      boatData.lights.spotlight.fog = N2kOnOff_On;
    } else if ((boatData.lights.spotlight.spot == N2kOnOff_Off) && (boatData.lights.spotlight.fog == N2kOnOff_On)) {
      boatData.lights.spotlight.spot = N2kOnOff_Off;
      boatData.lights.spotlight.fog = N2kOnOff_Off;
    } else {
      boatData.lights.spotlight.spot = N2kOnOff_Off;
      boatData.lights.spotlight.fog = N2kOnOff_Off;
    }
  }

  newIO |= readToggleButton(buttons[E_ENGINE_ROOM_LIGHTS], boatData.lights.engineRoom);
  newIO |= readToggleButton(buttons[E_CABIN_LIGHTS], boatData.lights.cabin);

  newIO |= readToggleButton(buttons[E_WATER_PUMP], boatData.utilities.waterPump);

  // newIO |= readToggleButton(buttons[E_ACCESSORIES_RADIO], boatData.utilities.radio);
  newIO |= readToggleButton(buttons[E_FM_RADIO], boatData.utilities.fmRadio);
  newIO |= readToggleButton(buttons[E_REFRIGERATOR], boatData.utilities.refrigerator);

  return newIO;
}

void setIO(BoatData &boatData) {
  digitalWrite(O_BILGE_BLOWER, ((boatData.blowers.one == N2kOnOff_On) || (boatData.blowers.two == N2kOnOff_On)));
  digitalWrite(O_BILGE_PUMP, (
    (boatData.bilgePumps.engineRoom.on == N2kOnOff_On) || (boatData.bilgePumps.midship.on == N2kOnOff_On) ||
    (boatData.bilgePumps.engineRoom.floatSwitch == N2kOnOff_On) || (boatData.bilgePumps.midship.floatSwitch == N2kOnOff_On)
  ));
  digitalWrite(O_NAVIGATION_LIGHTS, (boatData.lights.navigation == N2kOnOff_On));
  digitalWrite(O_ANCHOR_LIGHTS, (boatData.lights.anchor == N2kOnOff_On));
  digitalWrite(O_ENGINE_ROOM_LIGHTS, (boatData.lights.engineRoom == N2kOnOff_On));
  // digitalWrite(O_ROCM_RADIO, (boatData.utilities.radio == N2kOnOff_On));

  digitalWrite(O_SERVOS, HIGH);
  digitalWrite(O_PORT_ENGINE_HOURS, HIGH);
  digitalWrite(O_STARBOARD_ENGINE_HOURS, HIGH);
  digitalWrite(O_RELAY_4, HIGH);
  digitalWrite(O_RELAY_5, HIGH);
  digitalWrite(O_RELAY_6, HIGH);
  digitalWrite(O_RELAY_7, HIGH);
  digitalWrite(O_RELAY_8, HIGH);

  digitalWrite(O_INSTRUMENT_LIGHTS, (boatData.lights.instruments == N2kOnOff_On));
  digitalWrite(O_BATHROOM_LIGHTS, (boatData.lights.head == N2kOnOff_On));
  digitalWrite(O_BERTH_LIGHTS, (boatData.lights.berth == N2kOnOff_On));
  digitalWrite(O_MOSFET_4, LOW);

}

void setServos(BoatData &boatData) {
  servos[S_PORT_ENGINE_RPM].writeMicroseconds(map(boatData.engines.port.rpm, 0, 6000, 1000, 2000));
  servos[S_PORT_ENGINE_OIL].writeMicroseconds(map(boatData.engines.port.oilPressure, 0, 549172, 1000, 2000));
  servos[S_PORT_ENGINE_TEMPERATURE].writeMicroseconds(map(boatData.engines.port.waterTemperature, 40, 120, 1000, 2000));
  servos[S_STARBOARD_ENGINE_RPM].writeMicroseconds(map(boatData.engines.starboard.rpm, 0, 6000, 1000, 2000));
  servos[S_STARBOARD_ENGINE_OIL].writeMicroseconds(map(boatData.engines.starboard.oilPressure, 0, 549172, 1000, 2000));
  servos[S_STARBOARD_ENGINE_TEMPERATURE].writeMicroseconds(map(boatData.engines.starboard.waterTemperature, 40, 120, 1000, 2000));
}

void setupAC() {
  Wire.begin(I_ONE_WIRE_BUS);
}

void readAC(BoatData &boatData) {
  byte* data;

  Wire.requestFrom(8, 8);
  if (Wire.available()) {
    data = (byte*) &boatData.ac;
    while (Wire.available()) {
      *data = Wire.read();
      data++;
    }
  }
  Serial.print("Volts ");
  Serial.println(boatData.ac.volts);
  Serial.print("Amps ");
  Serial.println(boatData.ac.amps);
}
