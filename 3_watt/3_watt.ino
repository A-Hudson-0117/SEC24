
// 
//  YOU WILL BLIND YOURSELF IF YOU ARE NOT READY
// 
//  IT IS REALLY BRIGHT
// 
// EVEN IF WE PUT IT ON A 3 VOLT RAIL
// 


void setup() {
  // initialize digital pin 13 as an output.
  pinMode(13, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  analogWrite(13, 100); // turn the LED on (HIGH is the voltage high
  delay(1000); // wait for a second
  digitalWrite(13, LOW); // turn the LED off by making the voltage LOW
  delay(1000); // wait for a second
}