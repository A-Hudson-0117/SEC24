#include <LiquidCrystal.h>
#define A_IN_PIN A0 // Arduino pin that connects to AOUT pin of moisture sensor

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(11, 12, 6, 7, 4, 5);

void writeLCD(string sensor, string data){
    lcd.setCursor(0, 0);
    lcd.print(sensor);
    lcd.setCursor(0, 1);
    lcd.print(data);
}

void setup() {
    // set up the LCD's number of columns and rows:
    lcd.begin(8, 2);
    Serial.begin(115200);
    pinMode(A_IN_PIN,OUTPUT);
}

void loop() {
    int moisture = analogRead(A_IN_PIN); // read the analog value from sensor
    show(moisture) ;
    delay(1000);

    /*
    int moisture = analogRead(Ain_PIN); // read the analog value from sensor

    Serial.print("Moisture: ");
    Serial.println(moisture);

    delay(500);
    */
}