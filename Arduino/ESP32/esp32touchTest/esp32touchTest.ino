// ESP32 Touch Test
// Just test touch pin - Touch0 is T0 which is on GPIO 4.

const int CAPTOUCHPINS[] = {14,33,15}; // board labels match but 32/33 are swapped

void setup() {
  Serial.begin(115200);
  // delay(1000); // give me time to bring up serial monitor
  // Serial.println("ESP32 Touch Test");
  while(!Serial){
  	// wait until monitor is up
  };
  // delay(2000);
  // Serial.println("T0,T1,T2,T3,T4,T5,T6,T7,T8,T9");

}

void loop() {
	Serial.print("100,0");
	int junk = touchRead(CAPTOUCHPINS[0]);
	// for (int i = 0; i < 3; i++){
	// 	Serial.print(",");
	// 	Serial.print(touchRead(CAPTOUCHPINS[i]));
	// 	// delay(8);
	// }

  //////////////////////
  // QUERY ALL TOUCHPINS
  //////////////////////
	Serial.print(",");
	Serial.print(touchRead(T0));
	Serial.print(",");
	Serial.print(touchRead(T1));
	Serial.print(",");
	Serial.print(touchRead(T2));
	Serial.print(",");
	Serial.print(touchRead(T3));
	Serial.print(",");
	Serial.print(touchRead(T4));
	Serial.print(",");
	Serial.print(touchRead(T5));
	Serial.print(",");
	Serial.print(touchRead(T6));
	Serial.print(",");
	Serial.print(touchRead(T7));
	Serial.print(",");
	Serial.print(touchRead(T8));
	Serial.print(",");
	Serial.print(touchRead(T9));
	
  /////////////////

  // }
  Serial.println("");
  delay(20);
}