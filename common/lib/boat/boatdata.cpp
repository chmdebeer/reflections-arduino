#include <Arduino.h>
#include "N2kTypes.h"
#include "N2kMessages.h"
#include "boatdata.h"
#include <math.h>

void clearBoatData(BoatData &boatData) {
  memset(&boatData, 0, sizeof(BoatData));
  boatData.utilities.doorLock = N2kOnOff_On;
}

uint64_t binaryStatusFromBoatData(unsigned char instance, BoatData boatData) {
  int offset = 1;
  tN2kBinaryStatus binaryStatus;

  binaryStatus = 0x0ULL;

  if (instance == E_IGNITION_START) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.ignition, offset++); // 1.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.start, offset++); // 1.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.ignition, offset++); // 1.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.start, offset++); // 1.4

  } else if (instance == E_ENGINE_STATUS) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.check, offset++); // 2.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.malfunction, offset++); // 2.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.check, offset++); // 2.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.malfunction, offset++); // 2.$

  } else if (instance == E_POWER_TRIM) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.trim.bowUp, offset++); // 3.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.trim.bowDown, offset++); // 3.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.trim.bowUp, offset++); // 3.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.trim.bowDown, offset++); // 3.4

  } else if (instance == E_TRIM) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.port.bowUp, offset++); // 4.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.port.bowDown, offset++); // 4.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.starboard.bowUp, offset++); // 4.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.starboard.bowDown, offset++); // 4.4

  } else if (instance == E_NUTRASALT) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.nutraSalt, offset++); // 5.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.nutraSalt, offset++); // 5.2

  } else if (instance == E_LIGHTS) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.instruments, offset++); // 6.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.cabin, offset++); // 6.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.berth, offset++); // 6.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.head, offset++); // 6.4
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.aftDeck, offset++); // 6.5
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.belowDeck, offset++); // 6.6
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.engineRoom, offset++); // 6.7
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.anchor, offset++); // 6.8
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.navigation, offset++); // 6.9
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.stern, offset++); // 6.10

  } else if (instance == E_SPOTLIGHT) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.fog, offset++); // 7.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.spot, offset++); // 7.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.up, offset++); // 7.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.down, offset++); // 7.4
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.right, offset++); // 7.5
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.left, offset++); // 7.6


  } else if (instance == E_UTILITIES_CABIN) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.horn.one, offset++); // 8.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.horn.two, offset++); // 8.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.wipers.port, offset++); // 8.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.wipers.starboard, offset++); // 8.4
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.cabinHeaterFan, offset++); // 8.5
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.fmRadio, offset++); // 8.6

  } else if (instance == E_UTILITIES_BILGE) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.blackwaterAirPump, offset++); // 9.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.macerator, offset++); // 9.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.showerDrainPump, offset++); // 9.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.midship.on, offset++); // 9.4
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.midship.floatSwitch, offset++); // 9.5

  } else if (instance == E_UTILITIES_ENGINE_ROOM) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.waterPump, offset++); // 10.1
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.blower, offset++); // 10.2
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.engineRoom.on, offset++); // 10.3
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.engineRoom.floatSwitch, offset++); // 10.4
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.doorLock, offset++); // 10.5

  }

  return binaryStatus;
}

void boatDataFromBinaryStatus(unsigned char instance, uint64_t binaryStatus, BoatData &boatData) {
  int offset = 1;

  if (instance == E_IGNITION_START) {
    boatData.engines.port.ignition = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 1.1
    boatData.engines.port.start = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 1.2
    boatData.engines.starboard.ignition = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 1.3
    boatData.engines.starboard.start = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 1.4

  } else if (instance == E_ENGINE_STATUS) {
    boatData.engines.port.check = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 2.1
    boatData.engines.port.malfunction = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 2.2
    boatData.engines.starboard.check = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 2.3
    boatData.engines.starboard.malfunction = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 2.4

  } else if (instance == E_POWER_TRIM) {
    boatData.engines.port.trim.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 3.1
    boatData.engines.port.trim.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 3.2
    boatData.engines.starboard.trim.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 3.3
    boatData.engines.starboard.trim.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 3.4

  } else if (instance == E_TRIM) {
    boatData.trim.port.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 4.1
    boatData.trim.port.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 4.2
    boatData.trim.starboard.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 4.3
    boatData.trim.starboard.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 4.4

  } else if (instance == E_NUTRASALT) {
    boatData.engines.port.nutraSalt = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 5.1
    boatData.engines.starboard.nutraSalt = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 5.2

  } else if (instance == E_LIGHTS) {
    boatData.lights.instruments = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.1
    boatData.lights.cabin = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.2
    boatData.lights.berth = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.3
    boatData.lights.head = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.4
    boatData.lights.aftDeck = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.5
    boatData.lights.belowDeck = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.6
    boatData.lights.engineRoom = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.7
    boatData.lights.anchor = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.8
    boatData.lights.navigation = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.9
    boatData.lights.stern = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 6.10

  } else if (instance == E_SPOTLIGHT) {
    boatData.lights.spotlight.fog = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 7.1
    boatData.lights.spotlight.spot = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 7.2
    boatData.lights.spotlight.up = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 7.3
    boatData.lights.spotlight.down = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 7.4
    boatData.lights.spotlight.right = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 7.5
    boatData.lights.spotlight.left = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 7.6


  } else if (instance == E_UTILITIES_CABIN) {
    boatData.utilities.horn.one = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 8.1
    boatData.utilities.horn.two = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 8.2
    boatData.utilities.wipers.port = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 8.3
    boatData.utilities.wipers.starboard = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 8.4
    boatData.utilities.cabinHeaterFan = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 8.5
    boatData.utilities.fmRadio = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 8.6

  } else if (instance == E_UTILITIES_BILGE) {
    boatData.utilities.blackwaterAirPump = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 9.1
    boatData.utilities.macerator = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 9.2
    boatData.utilities.showerDrainPump = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 9.3
    boatData.bilgePumps.midship.on = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);  // 9.4
    boatData.bilgePumps.midship.floatSwitch = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 9.5

  } else if (instance == E_UTILITIES_ENGINE_ROOM) {
    boatData.utilities.waterPump = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 10.1
    boatData.blower = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 10.2
    boatData.bilgePumps.engineRoom.on = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);  // 10.3
    boatData.bilgePumps.engineRoom.floatSwitch = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 10.4
    boatData.utilities.doorLock = N2kGetStatusOnBinaryStatus(binaryStatus, offset++); // 10.5

  }

}
