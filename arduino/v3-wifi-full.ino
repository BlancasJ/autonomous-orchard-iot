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
float sensorPin4 = A4;
float sensorValue4 = 0;
float sensorPercentage4 = 0;
float sensorPin5 = A5;
float sensorValue5 = 0;
float sensorPercentage5 = 0;

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

  sensorValue4 = analogRead(sensorPin4);
  sensorPercentage4 = sensorValue4 * 10 / 95;

  sensorValue5 = analogRead(sensorPin5);
  sensorPercentage5 = sensorValue5 * 10 / 95;

  moisture = ((sensorPercentage + sensorPercentage1 + sensorPercentage2 + sensorPercentage3 + sensorPercentage4 + sensorPercentage5) / 6);

  temperature = TH02.ReadTemperature();
  humidity = TH02.ReadHumidity();
  delay(500);

  Serial.print(temperature);
  Serial.println(" C");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print(moisture);
  Serial.println(" %");

  Serial.println("Connecting...");
  WiFiClient client;
  if (client.connect(host, 80) > 0) {
    client.print("GET /data/second.php?temperature=");
    client.print(temperature);
    client.print("&humidity=");
    client.println(humidity);
    client.print("&moisture=");
    client.println(moisture);
    client.println(" HTTP/1.0");
    client.println();
    Serial.println("Connected");
  } else {
    Serial.println("Connection failed");
  }
  if (!client.connected()) {
    Serial.println("Disconnected!");
  }
  client.stop();
  client.flush();
  delay(60000);
}
