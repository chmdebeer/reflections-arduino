#include <WiFi.h>
#include "KasaSmartPlug.h"

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

#define DHTPIN 14     // Digital pin connected to the DHT sensor 
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
#define LED_BUILTIN 2

DHT_Unified dht(DHTPIN, DHTTYPE);

bool heaterOn = false;

uint32_t delayMS;

const char *ssid = "TELUS6529";
const char *password = "939my44a4r";

//const char *ssid = "Reflections";
//const char *password = "wpar3f13ction5";

KASAUtil kasaUtil;

void turnHeater(bool turnOn) {
  Serial.print("Turning heater ");

  KASASmartPlug *testPlug = kasaUtil.GetSmartPlug("Reflections Heater");
//   KASASmartPlug *testPlug = new KASASmartPlug("Kitchen","192.168.1.71");

  if (testPlug != NULL) {
    if (turnOn) {
      Serial.println("On");
      testPlug->SetRelayState(1);
    } else {
      Serial.println("Off");
      testPlug->SetRelayState(0);
    }  
    heaterOn = turnOn;
  }
}


void setup()
{
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


  found = kasaUtil.ScanDevices();
  Serial.printf("\r\n Found device = %d", found);

  // Print out devices name and ip address found..
  for (int i = 0; i < found; i++)
  {
    KASASmartPlug *p = kasaUtil.GetSmartPlugByIndex(i);
    Serial.printf("\r\n %d. %s IP: %s Relay: %d", i, p->alias, p->ip_address, p->state);  
  } 

  dht.begin();
  Serial.println(F("DHTxx Unified Sensor Example"));
  // Print temperature sensor details.
  sensor_t sensor;
  dht.temperature().getSensor(&sensor);
  Serial.println(F("------------------------------------"));
  Serial.println(F("Temperature Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
  Serial.println(F("------------------------------------"));
  // Print humidity sensor details.
  dht.humidity().getSensor(&sensor);
  Serial.println(F("Humidity Sensor"));
  Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
  Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
  Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
  Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
  Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
  Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
  Serial.println(F("------------------------------------"));
  // Set delay between sensor readings based on sensor details.
  delayMS = sensor.min_delay / 1000;

}

void loop()
{
  // Delay between measurements.
  delay(delayMS);
  // Get temperature event and print its value.
  sensors_event_t event;
  dht.temperature().getEvent(&event);
  if (isnan(event.temperature)) {
    Serial.println(F("Error reading temperature!"));
  }
  else {
    Serial.print(F("Temperature: "));
    Serial.print(event.temperature);
    Serial.println(F("°C"));
  }
  if (event.temperature < 3.0) {
    if (!heaterOn) {
      turnHeater(true);
    }
  } else if (event.temperature > 5.0) {
    if (heaterOn) {
      turnHeater(false);
    }
  }
  
  if (heaterOn) {
    digitalWrite(LED_BUILTIN, LOW);
    delay(200);
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(200);
    digitalWrite(LED_BUILTIN, LOW);
  }
  
  // Get humidity event and print its value.
  dht.humidity().getEvent(&event);
  if (isnan(event.relative_humidity)) {
    Serial.println(F("Error reading humidity!"));
  }
  else {
    Serial.print(F("Humidity: "));
    Serial.print(event.relative_humidity);
    Serial.println(F("%"));
  }

}
