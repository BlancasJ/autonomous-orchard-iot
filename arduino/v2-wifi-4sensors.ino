#include <Wire.h>
#include <TH02_dev.h>
#include "Arduino.h"
#include <SPI.h>
#include <WiFi.h>

char* ssid = "NETWORK_NAME";
char* password = "PASSWORD";
const char* host = "IP ADDRESS";

float sensorPin = A0;
float sensorValue = 0;
float sensorPercentage = 0;
float sensorPin1 = A1;
float sensorValue1 = 0;
float sensorPercentage1 = 0;
float sensorPin2 = A2;
float sensorValue2 = 0;
float sensorPercentage2 = 0;
float sensorPin3 = A3;
float sensorValue3 = 0;
float sensorPercentage3 = 0;

float moisture = 0;
float temperature = 0;
float humidity = 0;

void setup() {
  Serial.begin(9600);

  TH02.begin();
  delay(100);
  pinMode(4, OUTPUT);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  sensorValue = analogRead(sensorPin);
  sensorPercentage = sensorValue * 10 / 95;

  sensorValue1 = analogRead(sensorPin1);
  sensorPercentage1 = sensorValue1 * 10 / 95;

  sensorValue2 = analogRead(sensorPin2);
  sensorPercentage2 = sensorValue2 * 10 / 95;

  sensorValue3 = analogRead(sensorPin3);
  sensorPercentage3 = sensorValue3 * 10 / 95;

  moisture = ((sensorPercentage + sensorPercentage1 + sensorPercentage2 + sensorPercentage3) / 4);

  temperature = TH02.ReadTemperature();
  humidity = TH02.ReadHumidity();
  delay(500);

  Serial.print("connecting to ");
  Serial.println(host);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/second.php";
  String param1 = "&Temperature=";
  String param2 = "&Humidity=";
  String param3 = "&Moisture=";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET  ") + url + param1 + String(temperature) + param2 + String(humidity) + param3 + String(moisture) + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }

  while (client.available()) {
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }

  Serial.println();
  Serial.println("closing connection");

  delay(6000);

  if (moisture >= 70 && moisture <= 75) {
    digitalWrite(4, HIGH);
    delay(500);
  } else {
    digitalWrite(4, LOW);
    delay(500);
  }
  delay(5000);
}
