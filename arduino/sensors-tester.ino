float average = 0;

void setup() {
  Serial.begin(9600);
  pinMode(5, OUTPUT);
}

void loop() {
  digitalWrite(5, HIGH);
  float s1 = analogRead(A0);
  float s2 = analogRead(A1);
  float s3 = analogRead(A2);
  float s4 = analogRead(A3);
  float s5 = analogRead(A4);
  float s6 = analogRead(A5);

  average = ((s1 + s2 + s3 + s4 + s5 + s6) / 6);

  Serial.println(average * (1 / 7.3));
  delay(3000);
}
