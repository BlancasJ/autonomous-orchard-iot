#include <Wire.h>
#include "Arduino.h"
#include <SPI.h>
#include <WiFi.h>
#include <TH02_dev.h>

int pecho = 2;
int ptrig = 3;
float duration, distance, container_volume;

char* ssid = "NETWORK_NAME";
char* password = "PASSWORD";
const char* host = "IP ADDRESS";

long currenttime;
long timepast;

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
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(12, HIGH);
  digitalWrite(11, HIGH);

  pinMode(pecho, INPUT);
  pinMode(ptrig, OUTPUT);

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
  sensorPercentage = sensorValue * (1 / 7.3);

  sensorValue1 = analogRead(sensorPin1);
  sensorPercentage1 = sensorValue1 * (1 / 7.3);

  sensorValue2 = analogRead(sensorPin2);
  sensorPercentage2 = sensorValue2 * (1 / 7.3);

  sensorValue3 = analogRead(sensorPin3);
  sensorPercentage3 = sensorValue3 * (1 / 7.3);

  sensorValue4 = analogRead(sensorPin4);
  sensorPercentage4 = sensorValue4 * (1 / 7.3);

  sensorValue5 = analogRead(sensorPin5);
  sensorPercentage5 = sensorValue5 * (1 / 7.3);

  moisture = ((sensorPercentage + sensorPercentage1 + sensorPercentage2 + sensorPercentage3 + sensorPercentage4 + sensorPercentage5) / 6);

  digitalWrite(ptrig, HIGH);
  delay(0.01);
  digitalWrite(ptrig, LOW);

  duration = pulseIn(pecho, HIGH);
  distance = ((duration / 2) / 29);
  container_volume = (110 - distance) * 5.5418;
  delay(500);

  temperature = TH02.ReadTemperature();
  humidity = TH02.ReadHumidity();
  delay(100);

  if (moisture <= 70) {
    digitalWrite(4, HIGH);
    currenttime = millis();
  }
  timepast = millis() - currenttime;

  if (timepast > 600000) {
    digitalWrite(4, LOW);
  }
  if (moisture >= 77) {
    digitalWrite(4, LOW);
  }

  Serial.print(temperature);
  Serial.println(" C");
  Serial.print(humidity);
  Serial.println(" %");
  Serial.print(moisture);
  Serial.println(" %");
  Serial.print(distance);
  Serial.println("cm");
  Serial.print(container_volume);
  Serial.println(" L");

  Serial.println("Connecting...");
  Serial.println(host);

  WiFiClient client;

  if (!client.connect(host, 80)) {
    Serial.println("connection failed");
    return;
  }
  client.print("GET /data/second.php?temperature=");
  client.print(temperature);
  client.print("&humidity=");
  client.print(humidity);
  client.print("&moisture=");
  client.print(moisture);
  client.print("&container_volume=");
  client.print(container_volume);
  client.println(" HTTP/1.0");
  client.println();
  Serial.println("Connected");

  if (!client.connected()) {
    Serial.println("Disconnected!");
  }
  Serial.println();
  Serial.println("closing connection");
  delay(300000);
}
