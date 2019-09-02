// Simple program to control an RGB LED light using the Neopixel library
// Adapted from NeoPixel Ring simple sketch (c) 2013 Shae Erisson under the GPLv3
// Cycles through HSV colors to test LED strips after making new connectors

#include <Adafruit_NeoPixel.h>

// Which pin on the Arduino is connected to the NeoPixels?
int LEDPIN = 6;

// How many NeoPixels are attached to the Arduino?
int NUMPIXELS = 75;

// When we setup the NeoPixel library, we tell it how many pixels, and which pin to use to send signals.
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

int delayval = 5; // delay in msec

void setup() {
  pixels.begin(); // This initializes the NeoPixel library.
  Serial.begin(9600);
}

int hue = 0;

void loop() {
//  uint32_t rgbcolor = pixels.ColorHSV(hue%100);

  for(int j = 0; j < NUMPIXELS;j++){
    pixels.setPixelColor(j, pixels.ColorHSV(hue%65536,255,255*0.50));
  }  
  pixels.show();
  delay(20);
  hue += 300;
  Serial.println(hue);
}
