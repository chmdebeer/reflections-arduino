#define N2k_CAN_INT_PIN 2
#define N2k_SPI_CS_PIN 48


#include <Arduino.h>

//20
    // Port 12v
//19
#define I_PORT_ENGINE_OIL A14
//18
#define I_PORT_ENGINE_TEMP A12
//17
#define I_PORT_DRIVE_TILT A15
//16
  // OPEN
//15
#define I_STARBOARD_ENGINE_OIL A13
//14
#define I_STARBOARD_ENGINE_TEMP A9
//13
#define I_STARBOARD_DRIVE_TILT A7
//12
   // Starboard 12v
//11
   //GND
//10
#define I_FUEL A0
//09
#define I_ENGINE_BILGE_PUMP A2
//08
#define I_AUX_BATTERY A1
//07
#define I_PORT_BATTERY A8
//06
#define I_STARBOARD_BATTERY A10
//05
#define I_STEERING_ANGLE A11
//04
#define I_PORT_ENGINE_CHECK_LIGHT A5
//03
#define I_PORT_ENGINE_ALARM A6
//02
#define I_STARBOARD_ENGINE_CHECK_LIGHT A4
//01
#define I_STARBOARD_ENGINE_ALARM A3

void setup() {
  Serial.begin(115200);
  Serial.println("Start-up ");
  // analogReference(INTERNAL2V56);
  pinMode(I_PORT_ENGINE_OIL, INPUT);
  pinMode(I_PORT_ENGINE_TEMP, INPUT);
  pinMode(I_PORT_DRIVE_TILT, INPUT);
  pinMode(I_PORT_ENGINE_ALARM, INPUT_PULLUP);
  pinMode(I_PORT_ENGINE_CHECK_LIGHT, INPUT_PULLUP);

  pinMode(I_STARBOARD_ENGINE_OIL, INPUT);
  pinMode(I_STARBOARD_ENGINE_TEMP, INPUT);
  pinMode(I_STARBOARD_DRIVE_TILT, INPUT);
  pinMode(I_STARBOARD_ENGINE_ALARM, INPUT_PULLUP);
  pinMode(I_STARBOARD_ENGINE_CHECK_LIGHT, INPUT_PULLUP);

  pinMode(I_FUEL, INPUT);
  pinMode(I_STEERING_ANGLE, INPUT);
  pinMode(I_ENGINE_BILGE_PUMP, INPUT);

  pinMode(I_AUX_BATTERY, INPUT);
  pinMode(I_PORT_BATTERY, INPUT);
  pinMode(I_STARBOARD_BATTERY, INPUT);

}

void loop() {
  Serial.print("I_PORT_ENGINE_OIL ");
  Serial.println(analogRead(I_PORT_ENGINE_OIL));
  Serial.print("I_PORT_ENGINE_TEMP ");
  Serial.println(analogRead(I_PORT_ENGINE_TEMP));
  Serial.print("I_PORT_DRIVE_TILT ");
  Serial.println(analogRead(I_PORT_DRIVE_TILT));
  Serial.print("I_PORT_ENGINE_ALARM ");
  Serial.println(digitalRead(I_PORT_ENGINE_ALARM));
  Serial.print("I_PORT_ENGINE_CHECK_LIGHT ");
  Serial.println(digitalRead(I_PORT_ENGINE_CHECK_LIGHT));

  Serial.print("I_STARBOARD_ENGINE_OIL ");
  Serial.println(analogRead(I_STARBOARD_ENGINE_OIL));
  Serial.print("I_STARBOARD_ENGINE_TEMP ");
  Serial.println(analogRead(I_STARBOARD_ENGINE_TEMP));
  Serial.print("I_STARBOARD_DRIVE_TILT ");
  Serial.println(analogRead(I_STARBOARD_DRIVE_TILT));
  Serial.print("I_STARBOARD_ENGINE_ALARM ");
  Serial.println(digitalRead(I_STARBOARD_ENGINE_ALARM));
  Serial.print("I_STARBOARD_ENGINE_CHECK_LIGHT ");
  Serial.println(digitalRead(I_STARBOARD_ENGINE_CHECK_LIGHT));

  Serial.print("I_FUEL ");
  Serial.println(analogRead(I_FUEL));
  Serial.print("I_STEERING_ANGLE ");
  Serial.println(analogRead(I_STEERING_ANGLE));
  Serial.print("I_ENGINE_BILGE_PUMP ");
  Serial.println(analogRead(I_ENGINE_BILGE_PUMP));

  Serial.print("I_AUX_BATTERY ");
  Serial.println(analogRead(I_AUX_BATTERY));
  Serial.print("I_PORT_BATTERY ");
  Serial.println(analogRead(I_PORT_BATTERY));
  Serial.print("I_STARBOARD_BATTERY ");
  Serial.println(analogRead(I_STARBOARD_BATTERY));
  Serial.println("");
  delay(2000);

  if ( Serial.available() ) { Serial.read(); }
}
