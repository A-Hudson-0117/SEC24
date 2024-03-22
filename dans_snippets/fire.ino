const int flamePin = 2;// the number of the flame pin
int State = 0;

void setup() {
// initialize the pushbutton pin as an input:
pinMode(flamePin, INPUT);
}
/* fire function
 * returns a bool
 * TRUE if fire is present
 * FALSE if fire is not present
 */
bool fire(){
  // read the state of the value:
  State = digitalRead(flamePin);
  if (State == HIGH) {
  // turn LED on:
  return true ;
  }else {
    return false ;
  }
}
void loop(){

}