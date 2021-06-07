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

  if (instance == 1) {
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.ignition, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.port.start, offset++);

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.ignition, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.engines.starboard.start, offset++);

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.tilt.port.bowUp, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.tilt.port.bowDown, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.tilt.starboard.bowUp, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.tilt.starboard.bowDown, offset++);

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.port.bowUp, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.port.bowDown, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.starboard.bowUp, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.trim.starboard.bowDown, offset++);

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.blowers.one, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.blowers.two, offset++);

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.engineRoom.on, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.midship.on, offset++);

  } else if (instance == 2) {

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.instruments, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.cabin, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.berth, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.head, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.aftDeck, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.belowDeck, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.engineRoom, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.anchor, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.navigation, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.stern, offset++);

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.fog, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.spot, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.up, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.down, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.right, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.lights.spotlight.left, offset++);


  } else if (instance == 3) {

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.horn.one, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.horn.two, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.wipers.port, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.wipers.starboard, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.waterPump, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.waterHeater, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.cabinHeaterFan, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.blackwaterAirPump, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.macerator, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.showerDrainPump, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.fmRadio, offset++);

  } else if (instance == 4) {

    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.engineRoom.floatSwitch, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.bilgePumps.midship.floatSwitch, offset++);
    N2kSetStatusBinaryOnStatus(binaryStatus, boatData.utilities.doorLock, offset++);

  }


  return binaryStatus;
}

void boatDataFromBinaryStatus(unsigned char instance, uint64_t binaryStatus, BoatData &boatData) {
  int offset = 1;

  if (instance == 1) {
    boatData.engines.port.ignition = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.engines.port.start = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

    boatData.engines.starboard.ignition = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.engines.starboard.start = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

    boatData.tilt.port.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.tilt.port.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.tilt.starboard.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.tilt.starboard.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

    boatData.trim.port.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.trim.port.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.trim.starboard.bowUp = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.trim.starboard.bowDown = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

    boatData.blowers.one = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.blowers.two = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

    boatData.bilgePumps.engineRoom.on = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.bilgePumps.midship.on = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

  } else if (instance == 2) {

    boatData.lights.instruments = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.cabin = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.berth = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.head = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.aftDeck = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.belowDeck = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.engineRoom = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.anchor = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.navigation = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.stern = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

    boatData.lights.spotlight.fog = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.spotlight.spot = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.spotlight.up = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.spotlight.down = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.spotlight.right = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.lights.spotlight.left = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

  } else if (instance == 3) {

    boatData.utilities.horn.one = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.horn.two = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.wipers.port = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.wipers.starboard = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.waterPump = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.waterHeater = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.cabinHeaterFan = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.blackwaterAirPump = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.macerator = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.showerDrainPump = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.fmRadio = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);


  } else if (instance == 4) {

    boatData.bilgePumps.engineRoom.floatSwitch = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.bilgePumps.midship.floatSwitch = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);
    boatData.utilities.doorLock = N2kGetStatusOnBinaryStatus(binaryStatus, offset++);

  }

}
