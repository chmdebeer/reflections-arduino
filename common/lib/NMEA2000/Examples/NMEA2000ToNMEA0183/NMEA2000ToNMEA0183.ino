/*
 NMEA0183/NMEA2000 library. NMEA2000 -> NMEA0183
   Reads some messages from NMEA2000 and converts them to NMEA0183
   format to NMEA0183_out (Serial on Arduino or /dev/tnt0 on RPi).
   Also forwards all NMEA2000 bus messages in Actisense format.

   The example is designed for sending N2k data to OpenCPN on RPi with PiCAN2.
   It can be used also on PC with some Arduino/Teensy board to provide
   NMEA0183 (and also NMEA2000) data to PC. Example has been tested on
   RPi3B, ESP32, Arduino DUE, Arduino Mega and Teensy.

 To use this example you need install also:
   - NMEA2000 library
   - NMEA0183 library
   - Related CAN libraries.

 The example works with default settings on Arduino/Teensy boards and on
 Rasberry Pi 3 with PiCAN2 shield.

 On Rasberry Pi you need also Code Block environment and tty0tty (virtual
 null modem cable)
*/

// This file is just Arduino IDE wrapper. Project has been originally written to
// Rasberry Pi, so you will find Arduino IDE familiar setup() and loop()
// functions in main.cpp.

