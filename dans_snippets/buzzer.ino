//control buzzer

int buzzpin = 3 ;
void setup() {
  int buzzpin = 3 ;
  pinMode(buzzpin, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  digitalWrite(buzzpin, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);                      // wait for a second
  digitalWrite(buzzpin, LOW);   // turn the LED off by making the voltage LOW
  delay(1000);                      // wait for a second
}
