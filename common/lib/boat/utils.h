#ifndef UTILS_H
#define UTILS_H

void blink(int pin);
double readAtd(int pin, long fromMin, long fromMax, long toMin, long toMax, double scale);
bool readToggleButton(Bounce &button, tN2kOnOff &field);
bool readMomentaryButton(Bounce &button, tN2kOnOff &field, bool fall = true);
bool readStartButton(Bounce &button, Engine &engine);
void blinkStartLed(BoatData &boatData, int portLed, int starboardLed);
bool newTrimAngle(BowUpDown &side);
void printBoatData(BoatData &boatData);
int freeMemory();

#endif
