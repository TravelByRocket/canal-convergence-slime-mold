#include <Adafruit_NeoPixel.h>

class LEDSection {
  int NUMPIXELS;
  int conversionArray[NUMPIXELS][2]; // led strip i, strip index j
  int counter = 0; // keep track of where indices have already been process

  public: 
    Button(int NUMPIXELS)
    
    void addSectionIndices(int LEDSTRIPID, int START, INT END){ // call this for each LED strip portion of each 
      int slicelength = abs(END - START) + 1
      START < END ? direction = 1 : direction = -1;

      for(int i = START; i < END; i += direction){
        conversionArray[0][counter] = LEDSTRIPID;
        conversionArray[1][counter] = i;
      }

      counter++; // update the last-processed index

    }

}

// Which pin on the Arduino is connected to the NeoPixels?
int PINLEDSTRIP0 = 4;
int PINLEDSTRIP1 = 0;
int PINLEDSTRIP2 = 2;
int PINLEDSTRIP3 = 15;

// How many NeoPixels are attached to the Arduino?
int NUMPIXELSLEDSTRIP0 = 150;
int NUMPIXELSLEDSTRIP1 = 150;
int NUMPIXELSLEDSTRIP2 = 150;
int NUMPIXELSLEDSTRIP3 = 150;

int NUMPIXELSSECTIONA = 150;
int NUMPIXELSSECTIONB = 150;
int NUMPIXELSSECTIONC = 150;
int NUMPIXELSSECTIOND = 150;

bool MOVESINDEXDIRECTIONA = false;
bool MOVESINDEXDIRECTIONB = false;
bool MOVESINDEXDIRECTIONC = true;
bool MOVESINDEXDIRECTIOND = true;



// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels0 = Adafruit_NeoPixel(NUMPIXELSLEDSTRIP0, PINLEDSTRIP0, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels1 = Adafruit_NeoPixel(NUMPIXELSLEDSTRIP1, PINLEDSTRIP1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels2 = Adafruit_NeoPixel(NUMPIXELSLEDSTRIP2, PINLEDSTRIP2, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel pixels3 = Adafruit_NeoPixel(NUMPIXELSLEDSTRIP3, PINLEDSTRIP3, NEO_GRB + NEO_KHZ800);

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

  for(int k = 0; k < NUMPIXELSLEDSTRIP0; k++){ // set base color
    pixels0.setPixelColor(k, 0, 50, 50);
    pixels1.setPixelColor(k, 0, 100, 0);
  }
  for(int l = 0; l < NUMPIXELSLEDSTRIP2; l++){
    pixels2.setPixelColor(l, 0, 0, 100);
    pixels3.setPixelColor(l, 50, 0, 50);
  }

  for (int p = 0; p < floor(millis()-startWave1)/50; p++){ // red from beginning of strip to end
      pixels0.setPixelColor(p, 100,0,0);
      pixels1.setPixelColor(p, 100,0,0);
  }

  for (int q = NUMPIXELSLEDSTRIP2 - floor(millis()-startWave2)/50; q < NUMPIXELSLEDSTRIP2; q++){ // red from end of strip to beginning
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

void convertSectionIndexToStripIndex(){
  // pass in section, section index, color
  // do convert to strip index and color
  // return nothing

  // but is this doing too much at once?
}

void sectionA(){
  // set strip start point
  // set strip end point
  // if multi strip, next start/end section
}