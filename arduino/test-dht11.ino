#include "DHT.h"

#define DHTPIN 8
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(9600);
  dht.begin();
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println("Humidity: ");
  Serial.println(h);
  Serial.println("Temperature: ");
  Serial.println(t);
  delay(20000);
}
