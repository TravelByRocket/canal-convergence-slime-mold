// 20190901 Huzzah ESP8266 installed on semi-permanent board with four LED connections

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
int LEDPIN0 = 4;
int LEDPIN1 = 0;
int LEDPIN2 = 2;
int LEDPIN3 = 15;

// How many NeoPixels are attached to the Arduino?
int NUMPIXELS0 = 150;
int NUMPIXELS1 = 150;
int NUMPIXELS2 = 150;
int NUMPIXELS3 = 150;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels0 = Adafruit_NeoPixel(NUMPIXELS0, LEDPIN0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(NUMPIXELS1, LEDPIN1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(NUMPIXELS2, LEDPIN2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3 = Adafruit_NeoPixel(NUMPIXELS3, LEDPIN3, NEO_GRB + NEO_KHZ800);

//CapacitiveSensor cs5_4 = CapacitiveSensor(5,4);
//CapacitiveSensor cs3_2 = CapacitiveSensor(3,2);

int long startWave1 = 0;
int long startWave2 = 0;

int delayval = 50; // delay between loops in ms

void setup() {
  pixels0.begin();
  pixels1.begin();
  pixels2.begin();
  pixels3.begin();
  Serial.begin(9600);
}

int traveler = 0;

void loop() {

  for(int k = 0; k < NUMPIXELS0; k++){ // set base color
    pixels0.setPixelColor(k, 0, 50, 50);
    pixels1.setPixelColor(k, 0, 100, 0);
  }
  for(int l = 0; l < NUMPIXELS2; l++){
    pixels2.setPixelColor(l, 0, 0, 100);
    pixels3.setPixelColor(l, 50, 0, 50);
  }

  for (int p = 0; p < floor(millis()-startWave1)/50; p++){ // red from beginning of strip to end
      pixels0.setPixelColor(p, 100,0,0);
      pixels1.setPixelColor(p, 100,0,0);
  }

  for (int q = NUMPIXELS2 - floor(millis()-startWave2)/50; q < NUMPIXELS2; q++){ // red from end of strip to beginning
      pixels2.setPixelColor(q, 100,0,0);
      pixels3.setPixelColor(q, 100,0,0);
  }
  
  pixels0.show();
  pixels1.show();
  pixels2.show();
  pixels3.show();
  
  delay(delayval);

  if (millis() - startWave1 > 6800){
    startWave1 = millis();
  }
  
  if (millis() - startWave2 > 11300){
    startWave2 = millis();
  }
}
