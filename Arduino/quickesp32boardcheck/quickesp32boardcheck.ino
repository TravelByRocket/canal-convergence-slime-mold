// TWINKLING Simple program to control an RGB LED light using the Neopixel library
// Adapted from NeoPixel Ring simple sketch (c) 2013 Shae Erisson under the GPLv3

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
const int LEDPINS[] = {26,25,27,33}; // board labels A0,A1,27,33

// How many NeoPixels are attached to the Arduino?
int NUMPIXELS = 8;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel ledstrips[4];

int delayval = 200; // delay for half a second

int method = 1;
bool faster = true;

void setup() {
  for (int i = 0; i < 4; i++){
    ledstrips[i] = Adafruit_NeoPixel(NUMPIXELS, LEDPINS[i], NEO_GRB + NEO_KHZ800);
  }
  for (int i = 0; i < 4; i++){
    ledstrips[i].begin(); // This initializes the NeoPixel library.
  }
}

void loop() {

  // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
    
    for (int i = 0; i < 4; i++){
    ledstrips[i].clear();
    ledstrips[i].setPixelColor(floor(random(0,8)), ledstrips[i].Color(i*50,250-(i*50),150));
    ledstrips[i].setPixelColor(floor(random(0,8)), ledstrips[i].Color(i*50,250-(i*50),150));
    ledstrips[i].setPixelColor(floor(random(0,8)), ledstrips[i].Color(i*50,250-(i*50),150));
    ledstrips[i].show(); // This sends the updated pixel color to the hardware.
    }
    
    delay(delayval); // Delay for a period of time (in milliseconds).

}
