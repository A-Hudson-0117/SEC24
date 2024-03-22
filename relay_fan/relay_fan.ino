#define FanPin 40

void setup() {
  pinMode(40, OUTPUT);
}

void loop() {
  digitalWrite(FanPin, HIGH);
  delay(5000);
  digitalWrite(FanPin, LOW);
  delay(2000);
}