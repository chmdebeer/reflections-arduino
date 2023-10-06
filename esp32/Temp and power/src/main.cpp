#include <Arduino.h>
#include <WiFi.h>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 14     // Digital pin connected to the DHT sensor 
#define DHTTYPE DHT22     // DHT 22 (AM2302)
#define LED_BUILTIN 2

DHT_Unified dht(DHTPIN, DHTTYPE);

bool heaterOn = false;

uint32_t delayMS;

const char *ssid = "TELUS6529";
const char *password = "939my44a4r";

// const char *ssid = "Reflections";
// const char *password = "wpar3f13ction5";

void setup() {
  int found;
  Serial.begin(115200);
  Serial.println("Starting");

  pinMode (LED_BUILTIN, OUTPUT);

  // connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN) == HIGH ? LOW : HIGH));
    delay(500);
    Serial.print(".");
  }
  Serial.println(" CONNECTED");

}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(LED_BUILTIN, (digitalRead(LED_BUILTIN) == HIGH ? LOW : HIGH));
  delay(1000);

}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}