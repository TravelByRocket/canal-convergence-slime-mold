// green base state with propogating red for first garage test

#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

// Which pin on the Arduino is connected to the NeoPixels?
int LEDPIN6 = 6;
int LEDPIN7 = 7;
//int LEDPIN8 = 8;
//int LEDPIN9 = 9;

// How many NeoPixels are attached to the Arduino?
int NUMPIXELS6 = 75;
int NUMPIXELS7 = 150;
//int NUMPIXELS8 = 75;
//int NUMPIXELS9 = 75;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels6 = Adafruit_NeoPixel(NUMPIXELS6, LEDPIN6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels7 = Adafruit_NeoPixel(NUMPIXELS7, LEDPIN7, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel pixels8 = Adafruit_NeoPixel(NUMPIXELS8, LEDPIN8, NEO_GRB + NEO_KHZ800);
//Adafruit_NeoPixel pixels9 = Adafruit_NeoPixel(NUMPIXELS9, LEDPIN9, NEO_GRB + NEO_KHZ800);

int long startWave1 = 0;
int long startWave2 = 0;

int delayval = 50; // delay between loops in msec

void setup() {
  pixels6.begin();
  pixels7.begin();
//  pixels8.begin();
//  pixels9.begin();
  Serial.begin(9600);
}

int traveler = 0;

void loop() {

  for(int k = 0; k < NUMPIXELS6; k++){
    pixels6.setPixelColor(k, 0, 255, 0);
  }
  for(int l = 0; l < NUMPIXELS7; l++){
    pixels7.setPixelColor(l, 0, 255, 0);
  }
//  for(int m = 0; m < NUMPIXELS8; m++){
//    pixels8.setPixelColor(m, 0, 255, 0);
//  }
//  for(int n = 0; n < NUMPIXELS9; n++){
//    pixels9.setPixelColor(n, 0, 255, 0);
//  }

  for (int p = NUMPIXELS6 - floor(millis()-startWave1)/50; p < NUMPIXELS6; p++){
      pixels6.setPixelColor(p, 250,0,0);
  }

  for (int q = NUMPIXELS7 - floor(millis()-startWave2)/50; q < NUMPIXELS7; q++){
      pixels7.setPixelColor(q, 250,0,0);
  }
  
  pixels6.show();
  pixels7.show();
//  pixels8.show();
//  pixels9.show();
  
  delay(delayval);

  if (millis() - startWave1 > 6800){
    startWave1 = millis();
  }
  
  if (millis() - startWave2 > 11300){
    startWave2 = millis();
  }
}
