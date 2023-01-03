#include <Arduino.h>
#include <Bounce2.h>
#include <N2kTypes.h>
#include "BoatData.h"
#include "io.h"
#include <Wire.h>
#include "utils.h"

#define BOUNCE_INTERVAL 25
enum debounce {
  E_BOW_THRUSTER_ON,
  E_BOW_THRUSTER_PORT,
  E_BOW_THRUSTER_STARBOARD,
  E_PORT_START,
  E_STARBOARD_START,
  E_DRIVE_BOW_UP,
  E_DRIVE_BOW_DOWN,
  E_PORT_DRIVE_BOW_UP,
  E_PORT_DRIVE_BOW_DOWN,
  E_STARBOARD_DRIVE_BOW_UP,
  E_STARBOARD_DRIVE_BOW_DOWN,
  E_TRIM_BOW_UP,
  E_TRIM_BOW_DOWN,
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

void setupIO() {

  buttons[I_BOW_THRUSTER_ON].attach(I_PORT_START, INPUT_PULLUP);
  buttons[I_BOW_THRUSTER_PORT].attach(I_PORT_START, INPUT_PULLUP);
  buttons[I_BOW_THRUSTER_STARBOARD].attach(I_PORT_START, INPUT_PULLUP);

  buttons[E_PORT_START].attach(I_PORT_START, INPUT_PULLUP);
  buttons[E_STARBOARD_START].attach(I_STARBOARD_START, INPUT_PULLUP);

  buttons[E_DRIVE_BOW_UP].attach(I_DRIVE_BOW_UP, INPUT_PULLUP);
  buttons[E_DRIVE_BOW_DOWN].attach(I_DRIVE_BOW_DOWN, INPUT_PULLUP);

  buttons[E_PORT_DRIVE_BOW_UP].attach(I_PORT_DRIVE_BOW_UP, INPUT_PULLUP);
  buttons[E_PORT_DRIVE_BOW_DOWN].attach(I_PORT_DRIVE_BOW_DOWN, INPUT_PULLUP);
  buttons[E_STARBOARD_DRIVE_BOW_UP].attach(I_STARBOARD_DRIVE_BOW_UP, INPUT_PULLUP);
  buttons[E_STARBOARD_DRIVE_BOW_DOWN].attach(I_STARBOARD_DRIVE_BOW_DOWN, INPUT_PULLUP);

  buttons[E_TRIM_BOW_UP].attach(I_TRIM_BOW_UP, INPUT_PULLUP);
  buttons[E_TRIM_BOW_DOWN].attach(I_TRIM_BOW_DOWN, INPUT_PULLUP);

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

  pinMode(O_BOW_THRUSTER_ON, OUTPUT);

  pinMode(O_BILGE_PUMP, OUTPUT);
  pinMode(O_ENGINE_ROOM_LIGHTS, OUTPUT);
  pinMode(O_ACCESSORIES_RADIO, OUTPUT);
  pinMode(O_BILGE_BLOWER, OUTPUT);
  pinMode(O_NAVIGATION_LIGHTS, OUTPUT);
  pinMode(O_ANCHOR_LIGHTS, OUTPUT);

  pinMode(O_PORT_START, OUTPUT);
  pinMode(O_STARBOARD_START, OUTPUT);

  pinMode(O_RELAY_1, OUTPUT);
  pinMode(O_RELAY_2 , OUTPUT);
  pinMode(O_RELAY_3 , OUTPUT);
  pinMode(O_RELAY_4, OUTPUT);
  pinMode(O_RELAY_5, OUTPUT);
  pinMode(O_RELAY_6, OUTPUT);
  pinMode(O_RELAY_7, OUTPUT);
  pinMode(O_RELAY_8, OUTPUT);

  pinMode(O_INSTRUMENT_LIGHTS, OUTPUT);
  pinMode(O_BATHROOM_LIGHTS, OUTPUT);
  pinMode(O_MOSFET_4, OUTPUT);

}

bool readIO(BoatData &boatData, SwitchBankInstance instance) {
  bool newIO = false;
  tN2kOnOff value;

  if (instance == E_IGNITION_START) {
    newIO |= readStartButton(buttons[E_PORT_START], boatData.engines.port);
    newIO |= readStartButton(buttons[E_STARBOARD_START], boatData.engines.starboard);
    // boatData.engines.port.ignition // 1.1
    // boatData.engines.port.start // 1.2
    // boatData.engines.starboard.ignition // 1.3
    // boatData.engines.starboard.start // 1.4

  } else if (instance == E_ENGINE_STATUS) {
    // boatData.engines.port.check // 2.1
    // boatData.engines.port.malfunction // 2.2
    // boatData.engines.starboard.check // 2.3
    // boatData.engines.starboard.malfunction // 2.$

  } else if (instance == E_POWER_TRIM) {
    // boatData.engines.port.trim.bowUp // 3.1
    // boatData.engines.port.trim.bowDown // 3.2
    // boatData.engines.starboard.trim.bowUp // 3.3
    // boatData.engines.starboard.trim.bowDown // 3.4
    newIO |= readMomentaryButton(buttons[E_PORT_DRIVE_BOW_UP], boatData.engines.port.trim.bowUp, true);
    newIO |= readMomentaryButton(buttons[E_PORT_DRIVE_BOW_DOWN], boatData.engines.port.trim.bowDown, true);
    newIO |= readMomentaryButton(buttons[E_STARBOARD_DRIVE_BOW_UP], boatData.engines.starboard.trim.bowUp, true);
    newIO |= readMomentaryButton(buttons[E_STARBOARD_DRIVE_BOW_DOWN], boatData.engines.starboard.trim.bowDown, true);

    newIO |= readMomentaryButton(buttons[E_DRIVE_BOW_UP], value, true);
    if (value == N2kOnOff_On) {
      boatData.engines.port.trim.bowUp = N2kOnOff_On;
      boatData.engines.starboard.trim.bowUp = N2kOnOff_On;
    }

    newIO |= readMomentaryButton(buttons[E_DRIVE_BOW_DOWN], value, true);
    if (value == N2kOnOff_On) {
      boatData.engines.port.trim.bowDown = N2kOnOff_On;
      boatData.engines.starboard.trim.bowDown = N2kOnOff_On;
    }

  } else if (instance == E_TRIM) {
    // boatData.trim.port.bowUp // 4.1
    // boatData.trim.port.bowDown // 4.2
    // boatData.trim.starboard.bowUp // 4.3
    // boatData.trim.starboard.bowDown // 4.4
    newIO |= readMomentaryButton(buttons[E_PORT_TRIM_BOW_UP], boatData.trim.port.bowUp, true);
    newIO |= readMomentaryButton(buttons[E_PORT_TRIM_BOW_DOWN], boatData.trim.port.bowDown, true);
    newIO |= readMomentaryButton(buttons[E_STARBOARD_TRIM_BOW_UP], boatData.trim.starboard.bowUp, true);
    newIO |= readMomentaryButton(buttons[E_STARBOARD_TRIM_BOW_DOWN], boatData.trim.starboard.bowDown, true);

    newIO |= readMomentaryButton(buttons[E_TRIM_BOW_UP], value, true);
    if (value == N2kOnOff_On) {
      boatData.trim.port.bowUp = N2kOnOff_On;
      boatData.trim.starboard.bowUp = N2kOnOff_On;
    }

    newIO |= readMomentaryButton(buttons[E_TRIM_BOW_DOWN], value, true);
    if (value == N2kOnOff_On) {
      boatData.trim.port.bowDown = N2kOnOff_On;
      boatData.trim.starboard.bowDown = N2kOnOff_On;
    }

  } else if (instance == E_NUTRASALT) {
    // boatData.engines.port.nutraSalt // 5.1
    // boatData.engines.starboard.nutraSalt // 5.2

  } else if (instance == E_LIGHTS) {
    // boatData.lights.instruments // 6.1
    // boatData.lights.cabin // 6.2
    // boatData.lights.berth // 6.3
    // boatData.lights.head // 6.4
    // boatData.lights.aftDeck // 6.5
    // boatData.lights.belowDeck // 6.6
    // boatData.lights.engineRoom // 6.7
    // boatData.lights.anchor // 6.8
    // boatData.lights.navigation // 6.9
    // boatData.lights.stern // 6.10
    newIO |= readToggleButton(buttons[E_ENGINE_ROOM_LIGHTS], boatData.lights.engineRoom);
    newIO |= readToggleButton(buttons[E_CABIN_LIGHTS], boatData.lights.cabin);

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

  } else if (instance == E_SPOTLIGHT) {
    // boatData.lights.spotlight.fog // 7.1
    // boatData.lights.spotlight.spot // 7.2
    // boatData.lights.spotlight.up // 7.3
    // boatData.lights.spotlight.down // 7.4
    // boatData.lights.spotlight.right // 7.5
    // boatData.lights.spotlight.left // 7.6

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

  } else if (instance == E_UTILITIES_CABIN) {
    // boatData.utilities.horn.one // 8.1
    // boatData.utilities.horn.two // 8.2
    // boatData.utilities.wipers.port // 8.3
    // boatData.utilities.wipers.starboard // 8.4
    // boatData.utilities.cabinHeaterFan // 8.5
    // boatData.utilities.fmRadio // 8.6

    newIO |= readToggleButton(buttons[E_FM_RADIO], boatData.utilities.fmRadio);

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

  } else if (instance == E_UTILITIES_BILGE) {
    // boatData.utilities.blackwaterAirPump // 9.1
    // boatData.utilities.macerator // 9.2
    // boatData.utilities.showerDrainPump // 9.3
    // boatData.utilities.refrigerator // 9.4
    newIO |= readToggleButton(buttons[E_REFRIGERATOR], boatData.utilities.refrigerator);


  } else if (instance == E_UTILITIES_ENGINE_ROOM) {
    // boatData.utilities.waterPump // 10.1
    // boatData.blower // 10.2
    // boatData.utilities.doorLock // 10.3
    newIO |= readToggleButton(buttons[E_WATER_PUMP], boatData.utilities.waterPump);
    newIO |= readToggleButton(buttons[E_BILGE_BLOWER], boatData.blower);

  } else if (instance == E_BILGE_PUMPS) {
    // boatData.bilgePumps.midship.on // 11.1
    // boatData.bilgePumps.midship.floatSwitch // 11.2

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
  } else if (instance == E_BOW_THRUSTER) {
    newIO |= readToggleButton(buttons[E_BOW_THRUSTER_ON], boatData.engines.bowThruster.power);
    newIO |= readMomentaryButton(buttons[E_BOW_THRUSTER_PORT], boatData.engines.bowThruster.toPort, true);
    newIO |= readMomentaryButton(buttons[E_BOW_THRUSTER_STARBOARD], boatData.engines.bowThruster.toStarboard, true);
  }

  return newIO;
}

void setIO(BoatData &boatData, SwitchBankInstance instance) {
  if (instance == E_IGNITION_START) {
    // boatData.engines.port.ignition // 1.1
    // boatData.engines.port.start // 1.2
    // boatData.engines.starboard.ignition // 1.3
    // boatData.engines.starboard.start // 1.4

  } else if (instance == E_ENGINE_STATUS) {
    // boatData.engines.port.check // 2.1
    // boatData.engines.port.malfunction // 2.2
    // boatData.engines.starboard.check // 2.3
    // boatData.engines.starboard.malfunction // 2.$

  } else if (instance == E_POWER_TRIM) {
    // boatData.engines.port.trim.bowUp // 3.1
    // boatData.engines.port.trim.bowDown // 3.2
    // boatData.engines.starboard.trim.bowUp // 3.3
    // boatData.engines.starboard.trim.bowDown // 3.4

  } else if (instance == E_TRIM) {
    // boatData.trim.port.bowUp // 4.1
    // boatData.trim.port.bowDown // 4.2
    // boatData.trim.starboard.bowUp // 4.3
    // boatData.trim.starboard.bowDown // 4.4

  } else if (instance == E_NUTRASALT) {
    // boatData.engines.port.nutraSalt // 5.1
    // boatData.engines.starboard.nutraSalt // 5.2

  } else if (instance == E_LIGHTS) {
    // boatData.lights.instruments // 6.1
    // boatData.lights.cabin // 6.2
    // boatData.lights.berth // 6.3
    // boatData.lights.head // 6.4
    // boatData.lights.aftDeck // 6.5
    // boatData.lights.belowDeck // 6.6
    // boatData.lights.engineRoom // 6.7
    // boatData.lights.anchor // 6.8
    // boatData.lights.navigation // 6.9
    // boatData.lights.stern // 6.10
    digitalWrite(O_INSTRUMENT_LIGHTS, (boatData.lights.instruments == N2kOnOff_On));
    digitalWrite(O_BERTH_LIGHTS, (boatData.lights.berth == N2kOnOff_On));
    digitalWrite(O_BATHROOM_LIGHTS, (boatData.lights.head == N2kOnOff_On));

    digitalWrite(O_ENGINE_ROOM_LIGHTS, (boatData.lights.engineRoom == N2kOnOff_On));
    digitalWrite(O_ANCHOR_LIGHTS, (boatData.lights.anchor == N2kOnOff_On));
    digitalWrite(O_NAVIGATION_LIGHTS, (boatData.lights.navigation == N2kOnOff_On));

  } else if (instance == E_SPOTLIGHT) {
    // boatData.lights.spotlight.fog // 7.1
    // boatData.lights.spotlight.spot // 7.2
    // boatData.lights.spotlight.up // 7.3
    // boatData.lights.spotlight.down // 7.4
    // boatData.lights.spotlight.right // 7.5
    // boatData.lights.spotlight.left // 7.6

  } else if (instance == E_UTILITIES_CABIN) {
    // boatData.utilities.horn.one // 8.1
    // boatData.utilities.horn.two // 8.2
    // boatData.utilities.wipers.port // 8.3
    // boatData.utilities.wipers.starboard // 8.4
    // boatData.utilities.cabinHeaterFan // 8.5
    // boatData.utilities.fmRadio // 8.6

  } else if (instance == E_UTILITIES_BILGE) {
    // boatData.utilities.blackwaterAirPump // 9.1
    // boatData.utilities.macerator // 9.2
    // boatData.utilities.showerDrainPump // 9.3

  } else if (instance == E_UTILITIES_ENGINE_ROOM) {
    // boatData.utilities.waterPump // 10.1
    // boatData.blower // 10.2
    // boatData.utilities.doorLock // 10.3
    digitalWrite(O_BILGE_BLOWER, (boatData.blower == N2kOnOff_On));
    
  } else if (instance == E_BILGE_PUMPS) {
    // boatData.bilgePumps.midship.on // 9.4
    // boatData.bilgePumps.midship.floatSwitch // 9.5
    // boatData.bilgePumps.engineRoom.on // 10.3
    // boatData.bilgePumps.engineRoom.floatSwitch // 10.4

    digitalWrite(O_BILGE_PUMP, (
      (boatData.bilgePumps.engineRoom.on == N2kOnOff_On) || (boatData.bilgePumps.midship.on == N2kOnOff_On) ||
      (boatData.bilgePumps.engineRoom.floatSwitch == N2kOnOff_On) || (boatData.bilgePumps.midship.floatSwitch == N2kOnOff_On)
    ));
  }
  // digitalWrite(O_ROCM_RADIO, (boatData.utilities.radio == N2kOnOff_On));

  digitalWrite(O_RELAY_1, HIGH);
  digitalWrite(O_RELAY_2, HIGH);
  digitalWrite(O_RELAY_3, HIGH);
  digitalWrite(O_RELAY_4, HIGH);
  digitalWrite(O_RELAY_5, HIGH);
  digitalWrite(O_RELAY_6, HIGH);
  digitalWrite(O_RELAY_7, HIGH);
  digitalWrite(O_RELAY_8, HIGH);

  digitalWrite(O_MOSFET_4, LOW);

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
}
