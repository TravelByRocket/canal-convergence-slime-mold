#include <Adafruit_NeoPixel.h>

// CONSIDER Object/Class version of each section but may be overkill, at least while planning
//class LEDSection {
//  int NUMPIXELS;
//  int conversionArray[NUMPIXELS][2]; // led strip i, strip index j
//  int counter = 0; // keep track of where indices have already been process
//
//  public: 
//    Button(int NUMPIXELS)
//    
//    void addSectionIndices(int LEDSTRIPID, int START, INT END){ // call this for each LED strip portion of each 
//      int slicelength = abs(END - START) + 1
//      START < END ? direction = 1 : direction = -1;
//
//      for(int i = START; i < END; i += direction){
//        conversionArray[0][counter] = LEDSTRIPID;
//        conversionArray[1][counter] = i;
//      }
//
//      counter++; // update the last-processed index
//
//    }
//}

// Which pin on the Arduino is connected to the NeoPixels?
int PINLEDSTRIP0 = 4;
int PINLEDSTRIP1 = 0;
int PINLEDSTRIP2 = 2;
int PINLEDSTRIP3 = 15;

// How many NeoPixels are attached to the Arduino?
int NUMPIXLEDSTRIP0 = 150;
int NUMPIXLEDSTRIP1 = 150;
int NUMPIXLEDSTRIP2 = 130;
int NUMPIXLEDSTRIP3 = 150;

int NUMPIXSECTA = 255; // 150px (5.0m) on strip 0 -> 105px (3.5m) on strip 1
int NUMPIXSECTB = 225; // 120px (4.0m) on strip 2 ->  45px (1.5m) on strip 1 -> 60px (2.0m) on strip 3
int NUMPIXSECTC = 150;
int NUMPIXSECTD = 150;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel ledstrip0 = Adafruit_NeoPixel(NUMPIXLEDSTRIP0, PINLEDSTRIP0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledstrip1 = Adafruit_NeoPixel(NUMPIXLEDSTRIP1, PINLEDSTRIP1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledstrip2 = Adafruit_NeoPixel(NUMPIXLEDSTRIP2, PINLEDSTRIP2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledstrip3 = Adafruit_NeoPixel(NUMPIXLEDSTRIP3, PINLEDSTRIP3, NEO_GRB + NEO_KHZ800);

//CapacitiveSensor cs5_4 = CapacitiveSensor(5,4);
//CapacitiveSensor cs3_2 = CapacitiveSensor(3,2);

int long startWave1 = 0;
int long startWave2 = 0;

int delayval = 50; // delay between loops in ms

void setup() {
  ledstrip0.begin();
  ledstrip1.begin();
  ledstrip2.begin();
  ledstrip3.begin();
  Serial.begin(9600);
}

int traveler = 0;

void loop() {

  for(int m = 0; m < NUMPIXSECTA; m++){ // set base color
    setPixelColorSectA(m,0,50,50);
  }

  for(int n = 0; n < NUMPIXSECTB; n++){ // set base color
    setPixelColorSectB(n,50,0,50);
  }

//  for(int k = 0; k < NUMPIXLEDSTRIP0; k++){ // set base color
//    ledstrip0.setPixelColor(k, 0, 50, 50);
//    ledstrip1.setPixelColor(k, 0, 100, 0);
//  }
//  
//  for(int l = 0; l < NUMPIXLEDSTRIP2; l++){
//    ledstrip2.setPixelColor(l, 0, 0, 100);
//    ledstrip3.setPixelColor(l, 50, 0, 50);
//  }

  for (int p = 0; p < floor(millis()-startWave1)/25; p++){ // red from beginning of strip to end
      setPixelColorSectA(p,100,0,0);
  }

  for (int q = 0; q < floor(millis()-startWave2)/25; q++){ // red from beginning of strip to end
      setPixelColorSectB(q,100,0,0);
  }

//  for (int q = NUMPIXLEDSTRIP2 - floor(millis()-startWave2)/50; q < NUMPIXLEDSTRIP2; q++){ // red from end of strip to beginning
//      setPixelColorSectB(m,100,0,0);
//  }

//  for (int p = 0; p < floor(millis()-startWave1)/50; p++){ // red from beginning of strip to end
//      ledstrip0.setPixelColor(p, 100,0,0);
//      ledstrip1.setPixelColor(p, 100,0,0);
//  }
//
//  for (int q = NUMPIXLEDSTRIP2 - floor(millis()-startWave2)/50; q < NUMPIXLEDSTRIP2; q++){ // red from end of strip to beginning
//      ledstrip2.setPixelColor(q, 100,0,0);
//      ledstrip3.setPixelColor(q, 100,0,0);
//  }
  
  ledstrip0.show();
  ledstrip1.show();
  ledstrip2.show();
  ledstrip3.show();
  
  delay(delayval);

  if (millis() - startWave1 > 6800){
    startWave1 = millis();
  }
  
  if (millis() - startWave2 > 7300){
    startWave2 = millis();
  }
}

void setPixelColorSectA(int i, int r, int g, int b){
  if (i < 150) {
    ledstrip0.setPixelColor(150 - i,r,g,b); // against strip indices
  } else if (i < 150 + 105) {
    ledstrip1.setPixelColor(i - 150,r,g,b); // with strip indices
  }
}

void setPixelColorSectB(int i, int r, int g, int b){ //120, 45, 60
  if (i < 120) {
    ledstrip2.setPixelColor(120 - i,r,g,b); // against strip indices
  } else if (i < 120 + 45) {
    ledstrip1.setPixelColor(i - 15,r,g,b); // with strip indices
  } else if (i < 105 + 45 + 60) {
    ledstrip3.setPixelColor(i - 120 - 45,r,g,b); // with strip indices
  }
}
