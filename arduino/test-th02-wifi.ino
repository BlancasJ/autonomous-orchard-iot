#include <Wire.h>
#include <TH02_dev.h>
#include "Arduino.h"
#include <SPI.h>
#include <WiFi.h>

char* ssid = "NETWORK_NAME";
char* password = "PASSWORD";
const char* host = "IP ADDRESS";
float temp, hum;

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
  temp = TH02.readTemperature();
  hum = TH02.readHumidity();
  Serial.print("connecting to ");
  Serial.println(host);
  delay(500);

  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }

  String url = "/dht11.php";
  String key = "?pass=1234";
  String param1 = "&Temperatura=";
  String param2 = "&Humedad=";

  Serial.print("Requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + key + param1 + temp + param2 + hum + " HTTP/1.1\r\n" + "Host: " + host + "\r\n" + "Connection: close\r\n\r\n");
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
}
