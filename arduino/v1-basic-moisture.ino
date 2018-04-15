#include <Wire.h>
#include "rgb_lcd.h"
#include <TH02_dev.h>
#include "Arduino.h"

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

float sensorPercentageAverage = 0;
float temperature = 0;
float humidity = 0;

rgb_lcd lcd;

void setup() {
  Serial.begin(9600);
  lcd.begin(16, 2);
  TH02.begin();
  delay(100);
  lcd.setRGB(100, 150, 255);
  pinMode(4, OUTPUT);
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

  sensorPercentageAverage = ((sensorPercentage + sensorPercentage1 + sensorPercentage2 + sensorPercentage3) / 4);

  temperature = TH02.ReadTemperature();
  humidity = TH02.ReadHumidity();
  delay(500);

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(sensorPercentageAverage);
  lcd.println("%\r\n");

  lcd.setCursor(0, 1);
  lcd.print(temperature);
  lcd.println("C\r\n");

  lcd.setCursor(9, 1);
  lcd.print(humidity);
  lcd.println("%\r\n");

  delay(500);

  if (sensorPercentageAverage >= 70 && sensorPercentageAverage <= 75) {
    digitalWrite(4, HIGH);
    delay(500);
  } else {
    digitalWrite(4, LOW);
    delay(500);
  }
  delay(5000);
}
