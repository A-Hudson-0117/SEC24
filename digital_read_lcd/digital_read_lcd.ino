#include <LiquidCrystal.h>
#define DIGITAL_READ_PIN A0 // Arduino pin that connects to AOUT pin of moisture sensor

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(11, 12, 6, 7, 4, 5);

void show(String s){
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(s);
}

void setup() {
    // set up the LCD's number of columns and rows:
    lcd.begin(8, 2);
    Serial.begin(115200);
    pinMode(DIGITAL_READ_PIN, INPUT);
}

void loop() {
    int data = digitalRead(DIGITAL_READ_PIN); // read the analog value from sensor
    show("res:" + String(data)) ;
    delay(1000);

    /*
    int moisture = analogRead(Ain_PIN); // read the analog value from sensor

    Serial.print("Moisture: ");
    Serial.println(moisture);

    delay(500);
    */
}