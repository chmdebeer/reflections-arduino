#include <Arduino.h>
#include "N2kTypes.h"
#include "N2kMessages.h"
#include <Bounce2.h>
#include "boatdata.h"
#include "utils.h"

void blink(int pin) {
  digitalWrite(pin, !digitalRead(pin));
}

double readAtd(int pin, long fromMin, long fromMax, long toMin, long toMax, double scale) {
  int atdValue;

  atdValue = analogRead(pin);
  atdValue = map(atdValue, fromMin, fromMax, toMin, toMax);
  return (((double)atdValue)/scale);

}

bool readStartButton(Bounce &button, Engine &engine) {
  bool newIO = false;

  button.update();

  if (engine.ignition == N2kOnOff_On) {
    if (engine.start == N2kOnOff_On) {
      if ( button.rose() ) {
        // Serial.println("Ignition On : Starter Off");
        engine.start = N2kOnOff_Off;
        newIO = true;
      }
    } else {
      if ( (button.read() == LOW) && (button.duration() > 500) && (engine.rpm < 10)) {
        // Serial.println("Ignition On : Starter On");
        engine.start = N2kOnOff_On;
        newIO = true;
      }
      if ( button.rose() && !engine.ready) {
        engine.ready = true;
        button.update();
      }
      if ( button.rose() && engine.ready ) {
        // Serial.println("Ignition Off : Starter Off");
        engine.ready = false;
        engine.ignition = N2kOnOff_Off;
        newIO = true;
      }
    }
  } else {
    if ( button.fell() ) {
      // Serial.println("Ignition On : Starter Off");
      engine.ignition = N2kOnOff_On;
      newIO = true;
    }
  }

  return newIO;
}

void blinkStartLed(BoatData &boatData, int portLed, int starboardLed) {
  if (boatData.engines.port.ignition == N2kOnOff_On) {
    if (boatData.engines.port.rpm < 500) {
      blink(portLed);
    } else {
      digitalWrite(portLed, HIGH);
    }
  } else {
    digitalWrite(portLed, LOW);
  }

  if (boatData.engines.starboard.ignition == N2kOnOff_On) {
    if (boatData.engines.starboard.rpm < 500) {
      blink(starboardLed);
    } else {
      digitalWrite(starboardLed, HIGH);
    }
  } else {
    digitalWrite(starboardLed, LOW);
  }
}

bool readToggleButton(Bounce &button, tN2kOnOff &field) {
  bool newIO = false;

  button.update();
  if (button.fell()) {
    newIO = true;
    if (field != N2kOnOff_On) {
      field = N2kOnOff_On;
    } else {
      field = N2kOnOff_Off;
    }
  }
  return newIO;
}

bool readMomentaryButton(Bounce &button, tN2kOnOff &field, bool fall) {
  bool newIO = false;

  button.update();
  if (button.fell()) {
    newIO |= true;
    field = fall ? N2kOnOff_On : N2kOnOff_Off;
  }
  if (button.rose()) {
    newIO |= true;
    field = fall ? N2kOnOff_Off : N2kOnOff_On;
  }

  return newIO;
}

void printBoatData(BoatData &boatData) {
  Serial.println("+============================================|");

  Serial.print("+ Wipers: ");
  if (boatData.utilities.wipers.port == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  if (boatData.utilities.wipers.starboard == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.print(" | Navigation Lights: ");
  if (boatData.lights.navigation == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.println(" |");

  Serial.print("| Horn:       ");
  if (boatData.utilities.horn.one == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.print(" | Anchor Lights:     ");
  if (boatData.lights.anchor == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.println(" |");

  Serial.print("| Blower 1:   ");
  if (boatData.blowers.one == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.print(" | Engine Bilge:      ");
  if (boatData.bilgePumps.engineRoom.on == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.println(" |");

  Serial.print("| Blower 2:   ");
  if (boatData.blowers.two == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.print(" | Midship Bilge:     ");
  if (boatData.bilgePumps.midship.on == N2kOnOff_On) { Serial.print("  On"); } else { Serial.print(" Off"); }
  Serial.println(" |");

  Serial.println("+--------------------------------------------|");
  Serial.println("|                Drive                       |");
  Serial.print("| Port ");
  if (boatData.tilt.port.bowUp == N2kOnOff_On) { Serial.print("  Up  "); } else if (boatData.tilt.port.bowDown == N2kOnOff_On) { Serial.print(" Down "); } else { Serial.print("      "); }
  Serial.print("      | Starboard        ");
  if (boatData.tilt.starboard.bowUp == N2kOnOff_On) { Serial.print("  Up  "); } else if (boatData.tilt.starboard.bowDown == N2kOnOff_On) { Serial.print(" Down "); } else { Serial.print("      "); }
  Serial.println(" |");

  Serial.println("+--------------------------------------------|");
  Serial.println("|                Trim                        |");
  Serial.print("| Port ");
  if (boatData.trim.port.bowUp == N2kOnOff_On) { Serial.print("  Up  "); } else if (boatData.trim.port.bowDown == N2kOnOff_On) { Serial.print(" Down "); } else { Serial.print("      "); }
  Serial.print("      | Starboard        ");
  if (boatData.trim.starboard.bowUp == N2kOnOff_On) { Serial.print("  Up  "); } else if (boatData.trim.starboard.bowDown == N2kOnOff_On) { Serial.print(" Down "); } else { Serial.print("      "); }
  Serial.println(" |");

  Serial.println("+--------------------------------------------|");
  Serial.println("|                Start                       |");
  Serial.print("| Port ");
  if (boatData.engines.port.start == N2kOnOff_On) {
     Serial.print("Start ");
     // digitalWrite(O_PORT_START, HIGH);
  } else {
     Serial.print("      ");
     // digitalWrite(O_PORT_START, LOW);
  }
  Serial.print("      | Starboard        ");
  if (boatData.engines.starboard.start == N2kOnOff_On) {
    Serial.print("Start ");
    // digitalWrite(O_STARBOARD_START, HIGH);
  } else {
    Serial.print("      ");
    // digitalWrite(O_STARBOARD_START, LOW);
  }
  Serial.println(" |");

  Serial.println("+--------------------------------------------|");
  Serial.println("|                Spotlight                   |");
  Serial.print("| Fog:         ");
  if (boatData.lights.spotlight.fog == N2kOnOff_On) {
    Serial.print(" On");
  } else {
    Serial.print("Off");
  }

  Serial.print(" | Spot:               ");
  if (boatData.lights.spotlight.spot == N2kOnOff_On) {
    Serial.print(" On");
  } else {
    Serial.print("Off");
  }
  Serial.println(" |");

  Serial.print("| Up:          ");
  if (boatData.lights.spotlight.up == N2kOnOff_On) {
    Serial.print(" On");
  } else {
    Serial.print("Off");
  }

  Serial.print(" | Down:               ");
  if (boatData.lights.spotlight.down == N2kOnOff_On) {
    Serial.print(" On");
  } else {
    Serial.print("Off");
  }
  Serial.println(" |");

  Serial.print("| Left:        ");
  if (boatData.lights.spotlight.left == N2kOnOff_On) {
    Serial.print(" On");
  } else {
    Serial.print("Off");
  }

  Serial.print(" | Right               ");
  if (boatData.lights.spotlight.right == N2kOnOff_On) {
    Serial.print(" On");
  } else {
    Serial.print("Off");
  }
  Serial.println(" |");

  Serial.println("+============================================|");
}

#ifdef __arm__
// should use uinstd.h to define sbrk but Due causes a conflict
extern "C" char* sbrk(int incr);
#else  // __ARM__
extern char *__brkval;
#endif  // __arm__

int freeMemory() {
  char top;
#ifdef __arm__
  return &top - reinterpret_cast<char*>(sbrk(0));
#elif defined(CORE_TEENSY) || (ARDUINO > 103 && ARDUINO != 151)
  return &top - __brkval;
#else  // __arm__
  return __brkval ? &top - __brkval : &top - __malloc_heap_start;
#endif  // __arm__
}
