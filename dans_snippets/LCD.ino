#include <LiquidCrystal.h>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(11, 12, 6, 7, 4, 5);

void show(int i){
    lcd.setCursor(0, 0);
    lcd.print("data");
    lcd.setCursor(7, 0);
    lcd.print(i);
    lcd.setCursor(0, 1) ;
    lcd.print("stuff") ;
    lcd.setCursor(7, 1) ;
    lcd.print((i /2) + 1) ;
}

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(8, 2);
}

void loop() {
  for(int i = 0; i < 10 ; i++){
    show(i) ;
    delay(1000);
  }
}