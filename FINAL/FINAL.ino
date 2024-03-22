#include <Servo.h>
#include <dht11.h>
#include <LiquidCrystal.h>


enum main_state {BASE,FAN,WATER,ALARM};
enum display_state {SOIL,HUMID,TEMP,LIGHT,FIRE,TRAY};
#define NUM_LCD_STATES (6)


//initialize servo object
Servo myservo;
dht11 DHT ;
LiquidCrystal lcd(11, 12, 6, 7, 4, 5);

main_state CurrState_main;
display_state CurrState_display;



// ============================ //
//     FREQ DECLARE
// ============================ //
unsigned long WATER_FREQUENCY = 4000;
unsigned long WATER_DURATION = 4000;
unsigned long FAN_DURATION = 4000;
unsigned long ALARM_CHANGE_FREQUENCY = 400;
unsigned long PREV_LCD_FREQUENCY = 5000;
unsigned long LCD_TEXT_FREQUENCY = 100;
unsigned long STATE_READ_FREQUENCY = 50;


int air_humid = 0 ;
int temp_c = 0 ;
float soil_moisture = 0;


// ============================ //
//     TIMER VAR DECLARE
// ============================ //
unsigned long PREV_WATER_RUN_TIME; 
unsigned long PREV_WATER_START_TIME; 
unsigned long PREV_FAN_RUN_TIME; 
unsigned long PREV_FAN_START_TIME; 
unsigned long PREV_ALARM_START_TIME; 
unsigned long PREV_LCD_CHANGE_TIME; 
unsigned long LCD_TEXT_TIME; 
unsigned long STATE_READ_TIME; 



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
#define OUT_WATER_PIN 29
#define OUT_FAN_PIN 25
#define OUT_ALARM_PIN 30
#define OUT_LED_PIN 50

#define IN_SOIL_PIN A0
#define IN_HUMID_PIN 51
#define IN_TRAY_PIN 24
#define IN_LIGHT_PIN A3
#define IN_FLAME_PIN 2

#define MANUAL_PIN 40



// ============================ //
//         HELPER
// ============================ //

// helper-function for easy to use non-blocking timing
bool TimePeriodIsOver (unsigned long &StartTime, unsigned long TimePeriod) {
  unsigned long currentMillis = millis();  
  return currentMillis - StartTime >= TimePeriod;
}

void writeLCD(String sensor, String data){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(sensor);
  lcd.setCursor(0, 1);
  lcd.print(data);
}


// ============================ //
//     BASE FUNCTIONS
// ============================ //

void setup() {
  Serial.begin(115200);
  Serial.print( "\n Setup-Start  \n" );
  
  myservo.attach(OUT_WATER_PIN);

  lcd.begin(8, 2);


  // init states
  CurrState_main = BASE;
  CurrState_display = SOIL;


  // init flags
  update_flag = true;


  // init vars
  PREV_WATER_RUN_TIME = 500;
  PREV_WATER_START_TIME = 500;
  PREV_FAN_RUN_TIME = 500;
  PREV_FAN_START_TIME = 500;
  PREV_ALARM_START_TIME = 500;
  PREV_LCD_CHANGE_TIME = 500;

  // init pinout
  pinMode(IN_SOIL_PIN, INPUT);
  pinMode(IN_HUMID_PIN, INPUT);
  pinMode(IN_TRAY_PIN, INPUT);
  pinMode(IN_LIGHT_PIN, INPUT);
  pinMode(IN_FLAME_PIN, INPUT);

  pinMode(OUT_FAN_PIN, OUTPUT);
  pinMode(OUT_ALARM_PIN, OUTPUT);
  pinMode(OUT_LED_PIN, OUTPUT);

}

void loop() {

  // normal run commands
      
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

  if (TimePeriodIsOver(STATE_READ_TIME, STATE_READ_FREQUENCY)){
    if(read_photoresistor() < 75){
      runLight(true);
    }else{
      runLight(false);
    }

    Serial.println("state: " + String(CurrState_main));
    
    soil_moisture = read_soil_moisture_percent();
      
    read_humid_temp();

    STATE_READ_TIME = millis();
  }

  if (digitalRead(IN_TRAY_PIN) == HIGH){
    return;
  }

  switch (CurrState_main) {
    case ALARM:
      if (TimePeriodIsOver(PREV_ALARM_START_TIME, ALARM_CHANGE_FREQUENCY)){
        Serial.println( "ALARM\n" );
        digitalWrite(OUT_ALARM_PIN, HIGH); 
        PREV_ALARM_START_TIME = millis(); 
      }

      break;

    Serial.println(CurrState_main);

    case BASE:
      digitalWrite(OUT_ALARM_PIN, LOW);

      if (tray_is_loaded() == true){
        Serial.println("TRAY");
        return;
      }
      
      // int res = analogRead(MANUAL_PIN);
      // Serial.println(res);
      // if (res == HIGH){
      //   water_state_flag = true;
      // }

      // go to water 
      if ( fan_state_flag ) {
        Serial.println( "Moving to fan state\n" );
        fan_state_flag = false;

        // enable component
        runFan(true); 

        // tracking vars
        PREV_WATER_START_TIME = millis();
        CurrState_main = FAN; //jump to state
      } 
      else if ( water_state_flag || (soil_moisture <= 4) || TimePeriodIsOver(PREV_WATER_RUN_TIME, WATER_FREQUENCY)) {
        Serial.println( "Moving to water state\n" );
        water_state_flag = false;

        // enable component
        runWater(true); 

        // track and storage vars
        PREV_WATER_START_TIME = millis();
        CurrState_main = WATER; 
      }
      break;


    case WATER:
      Serial.println("running water\n\n");
      if ( TimePeriodIsOver(PREV_WATER_START_TIME, WATER_DURATION) ) {
        Serial.println("Done running water\n");

        runWater(false); 

        // move to fan after checking time again
        CurrState_main = BASE; // reset to starting step to repeat
        PREV_WATER_RUN_TIME = millis() + 100;
        fan_state_flag = true;
      }
      break;


    case FAN:
      Serial.println("running fan\n\n");
      if ( TimePeriodIsOver(PREV_WATER_START_TIME, FAN_DURATION) ) {
        Serial.println("Done running fan\n");

        runFan(false); 

        // move to fan after checking time again
        CurrState_main = BASE; // reset to starting step to repeat
        PREV_WATER_RUN_TIME = millis();
      }
      break;            
  }
}



// ============================ //
//    LCD STATE MACHINE
// ============================ //
void run_lcd_chain() {
  if ( TimePeriodIsOver(LCD_TEXT_TIME, LCD_TEXT_FREQUENCY) ) {
    String line = "";
    String data = "";

    switch (CurrState_display) {
      case SOIL:
        line = "SOIL";
        data = String(read_soil_moisture_percent()) + "%";
        break;


      case HUMID:
        line = "HUMID";
        data = String(air_humid);
        break;


      case TEMP:
        line = "TEMP";
        data = String(temp_c);
        break;


      case LIGHT:
        line = "LIGHT";
        data = String(read_photoresistor());
        break;


      case FIRE:
        line = "FIRE";
        data = String(is_fire());
        break;


      case TRAY:
        line = "TRAY";
        data = String(tray_is_loaded());
        break;
    }   
    Serial.println(line);
    Serial.println(data);
    writeLCD(line, data);

    LCD_TEXT_TIME = millis();
  }

  if ( TimePeriodIsOver(PREV_LCD_CHANGE_TIME, PREV_LCD_FREQUENCY) ) {
    Serial.println("Next timer state\n");
    
    // increment reset starting enum to repeat
    CurrState_display = CurrState_display + 1;
    if(CurrState_display >= NUM_LCD_STATES) {
      CurrState_display = 0;
    }

    // move to fan after checking time again
    PREV_LCD_CHANGE_TIME = millis();
  }
}




// ============================ //
//    run components
// ============================ //

bool is_fire(){
  return digitalRead(IN_FLAME_PIN) == LOW;
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
  }
  else {
    // go min position
    myservo.write(0) ;
  }
}

int read_photoresistor(){
  return analogRead(IN_LIGHT_PIN);
}

float read_soil_moisture_percent(){
  // calculate soild moisture percentage (max being 100, analog read is 1000)
  int moisture = analogRead(IN_SOIL_PIN) ;
  return min(analogRead(IN_TRAY_PIN)/10, 100);   
}

void read_humid_temp(){
  DHT.read(IN_HUMID_PIN) ;
  air_humid = DHT.humidity ;
  temp_c = DHT.temperature ;
}







