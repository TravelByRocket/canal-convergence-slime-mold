#include <CapacitiveSensor.h>

/*
 * CapitiveSense Library Demo Sketch
 * Paul Badger 2008
 * Uses a high value resistor e.g. 10M between send pin and receive pin
 * Resistor effects sensitivity, experiment with values, 50K - 50M. Larger resistor values yield larger sensor values.
 * Receive pin is the sensor pin - try different amounts of foil/metal on this pin
 */

CapacitiveSensor   cs_13_12 = CapacitiveSensor(13,12);        // pin 12 is sensor pin
CapacitiveSensor   cs_11_10 = CapacitiveSensor(11,10);        // pin 10 is sensor pin
CapacitiveSensor   cs_09_08 = CapacitiveSensor(9,8);        // pin 8 is sensor pin

void setup()                    
{
   Serial.begin(9600);
   pinMode(5,OUTPUT);
   pinMode(6,OUTPUT);
   pinMode(7,OUTPUT);
}

void loop()                    
{
    long finger3 =  cs_13_12.capacitiveSensor(30);
    long finger2 =  cs_11_10.capacitiveSensor(30);
    long finger1 =  cs_09_08.capacitiveSensor(30);

    Serial.print(finger1);                  // print sensor output 1
    Serial.print(",");                  // print sensor output 1
    Serial.print(finger2);                  // print sensor output 1
    Serial.print(",");                  // print sensor output 1
    Serial.print(finger3);                  // print sensor output 1
    Serial.println("");                  // print sensor output 1

    if(finger1 > 2000){
      digitalWrite(5,HIGH);
    } else {
      digitalWrite(5,LOW);
    }

    if(finger2 > 2000){
      digitalWrite(6,HIGH);
    } else {
      digitalWrite(6,LOW);
    }
    
    if(finger3 > 2000){
      digitalWrite(7,HIGH);
    } else {
      digitalWrite(7,LOW);
    }

    delay(20);                             // arbitrary delay to limit data to serial port 
}
