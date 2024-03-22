#include <Servo.h>
#include <dht11.h>
enum main_state {BASE,FAN,WATER,ALARM};
enum display_state {A,B,C,D};

//initialize servo object
Servo myservo;
dht11 DHT ;

main_state CurrState_main;
display_state CurrState_display;



// ============================ //
//     FREQ DECLARE
// ============================ //
unsigned long WATER_FREQUENCY = 30000;
unsigned long WATER_DURATION = 6000;
unsigned long FAN_DURATION = 4000;
unsigned long ALARM_CHANGE_FREQUENCY = 400;
int AIR_HUMID = 0 ;
int TEMP_C = 0 ;



// ============================ //
//     TIMER VAR DECLARE
// ============================ //
unsigned long PREV_WATER_RUN_TIME; 
unsigned long PREV_WATER_START_TIME; 
unsigned long PREV_FAN_RUN_TIME; 
unsigned long PREV_FAN_START_TIME; 
unsigned long PREV_ALARM_START_TIME; 



// ============================ //
//     FLAG DECLARE
// ============================ //
bool update_flag; // for lcd
bool light_toggle_flag = false;
bool water_state_flag = false;
bool fan_state_flag = false;
bool alarm_state_flag = false;



// ============================ //
//     PIN DECLARE
// ============================ //
#define OUT_WATER_PIN 9
#define OUT_FAN_PIN 0
#define OUT_ALARM_PIN 0
#define OUT_LED_PIN 0
#define AOUT_MOIST_PIN A0

#define IN_HUMID_PIN 3
#define IN_TRAY_PIN 0
#define IN_LIGHT_PIN 0
#define IN_FLAME_PIN 0


// ============================ //
//         HELPER
// ============================ //

// helper-function for easy to use non-blocking timing
bool TimePeriodIsOver (unsigned long &StartTime, unsigned long TimePeriod) {
  unsigned long currentMillis = millis();  
  return currentMillis - StartTime >= TimePeriod;
}



// ============================ //
//     BASE FUNCTIONS
// ============================ //

void setup() {
  Serial.begin(115200);
  Serial.print( "\n Setup-Start  \n" );
  
  myservo.attach(OUT_WATER_PIN);

  // init states
  CurrState_main = BASE;
  CurrState_display = A;


  // init flags
  update_flag = true;


  // init vars
  

  // init pinout
  pinMode(IN_TRAY_PIN, INPUT);

  pinMode(OUT_LED_PIN, OUTPUT);

}

void loop() {

  // normal run commands
  runLight(read_photoresistor() < 2);
      
  if (is_fire()){
    CurrState_main = ALARM;
  }

  run_main_chain();

  run_lcd_chain();
}



// ============================ //
//    MAIN STATE MACHINE
// ============================ //
void run_main_chain() {

  if (digitalRead(IN_TRAY_PIN) == HIGH){
    return;
  }

  switch (CurrState_main) {
    case ALARM:
      Serial.println( "ALARM\n" );
      if TimePeriodIsOver(PREV_ALARM_RUN_TIME, ALARM_CHANGE_FREQUENCY){
        digitalWrite(OUT_ALARM_PIN, HIGH); 
        PREV_ALARM_START_TIME = millis(); 
      }
      if TimePeriodIsOver(PREV_ALARM_RUN_TIME, ALARM_CHANGE_FREQUENCY){
        digitalWrite(OUT_ALARM_PIN, LOW);
        PREV_ALARM_START_TIME = millis(); 
      }

      break;


    case BASE:
      if (tray_is_loaded()){
        return;
      }
      
      float soil_moisture = read_soil_moisture();
      
      read_humid_temp();
      
      // go to water 
      if ( water_state_flag || (soil_moisture <= 5) || TimePeriodIsOver(PREV_WATER_RUN_TIME, WATER_FREQUENCY)) {
        Serial.println( "Moving to water state\n" );
        water_state_flag = false;

        // enable component
        runWater(true); 

        // track and storage vars
        PREV_WATER_START_TIME = millis();
        CurrState_main = WATER; 
      }
      else if ( fan_state_flag ) {
        Serial.println( "Moving to fan state\n" );
        fan_state_flag = false;

        // enable component
        runFan(true); 

        // tracking vars
        PREV_WATER_START_TIME = millis();
        CurrState_main = FAN; jump to state
      }
      break;


    case WATER:
      if ( TimePeriodIsOver(PREV_WATER_START_TIME, WATER_DURATION) ) {
        Serial.println("Done running water\n");

        runWater(false); 

        // move to fan after checking time again
        myStateVar = BASE; // reset to starting step to repeat
        PREV_WATER_RUN_TIME = millis() + 500;
        fan_state_flag = true;
      }
      break;


    case FAN:
      if ( TimePeriodIsOver(PREV_FAN_START_TIME, FAN_DURATION) ) {
        Serial.println("Done running fan\n");

        runFan(false); 

        // move to fan after checking time again
        myStateVar = BASE; // reset to starting step to repeat
        PREV_WATER_RUN_TIME = millis();
        fan_state_flag = true;
      }
      break;            
  }
}



// ============================ //
//    LCD STATE MACHINE
// ============================ //
void run_lcd_chain() {
  switch (CurrState_display) {
    case sayHello:
      Serial.println("Hello user!") );
      myStateVar = startCounting;
      break;

    case startCounting:
      myCounter = 0; // reset counter-variable
      Serial.println( F("I start counting very fast") );
      myStateVar = countTo10;
      break;            
  }    
}




// ============================ //
//    run components
// ============================ //

bool is_fire(){
  return digitalRead(flamePin) == HIGH;
}

bool tray_is_loaded(){
  return digitalRead(IN_TRAY_PIN) == HIGH;
}

void runLight(bool status){
  digitalWrite(OUT_LED_PIN, status);
}

void runFan(bool status) {
  digitalWrite(OUT_FAN_PIN, status);
}

void runWater(bool status) {
  if (status){
    // go max position
    myservo.write(180) ;
    delay(10) ;
  }
  else {
    // go min position
    myservo.write(0) ;
    delay(10) ;
  }
}

int read_photoresistor(){
  return analogRead(IN_LIGHT_PIN);
}

float read_soil_moisture(){
  // calculate soild moisture percentage (max being 100, analog read is 1000)
  int moisture = (analogRead(AOUT_MOIST_PIN) / 2) ;
  return min(analogRead(IN_TRAY_PIN)/10, 100);   
}

void read_humid_temp(){
  DHT.read(IN_HUMID_PIN) ;
  AIR_HUMID = DHT.humidity ;
  TEMP_C = DHT.temperature ;
}







