#include <dht11.h>
dht11 DHT;
#define HUMID_PIN 3
int HUMID = 0 ;
int TEMP_C = 0 ;
void setup(){
  //serial monitor for debugging purposes
Serial.begin(115200);

}


/*humid sensor
 *when called writes humidity and temp measurements to
 *HUMID and TEMP_C variables
 */
int humid_sensor(){
  //read data
  DHT.read(HUMID_PIN) ;
  HUMID = DHT.humidity ;
  TEMP_C = DHT.temperature ;

  Serial.print(DHT.humidity,1);
  Serial.print(",\t");
  Serial.println(DHT.temperature,1) ;


}
void loop(){
  humid_sensor() ;
  delay(1000) ;

}