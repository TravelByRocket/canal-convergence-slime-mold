// Cap touch pins D2 to D3 and neopixels on D6 going through 74AHCT125 Quad Level-Shifter
// Serial output clamped

#include <CapacitiveSensor.h>
#include <Adafruit_NeoPixel.h>

int LEDPIN = 12; // WEMOS D1 mini D6 is GPIO12
int NUMPIXELS = 10;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);

int long startWaveTime = 0; // poorly chosen variable to keep track of LED motion along strip

CapacitiveSensor cs = CapacitiveSensor(4,0); //D2 to D3 is GPIO4 to GPIO0
void setup()
{
   pinMode(LED_BUILTIN, OUTPUT); // pin 13
   Serial.begin(9600);
   pixels.begin();
   for(int i=0;i<NUMPIXELS;i++){
    pixels.setPixelColor(i,50,0,100);
   }
   delay(500);
   pixels.clear();
   pixels.show();
}

void loop()                    
{
//    startWaveTime = millis();
    long start = millis();
    long total1 =  cs.capacitiveSensor(30);
    if(total1 < 800){
      Serial.print(total1);                 // print sensor output 1
    } else {
      Serial.print(800); // limit the mac value to not wash out graph with spikes
    }

    if (total1 > 150) {
      digitalWrite(LED_BUILTIN, HIGH);
      startWaveTime = millis();
    } else {
      digitalWrite(LED_BUILTIN, LOW);
    }
    
    Serial.print(",");
    Serial.println("800"); // set a constant max for serial plotting

    pixels.clear();
    if((floor(millis()-startWaveTime)/200) < NUMPIXELS){
      pixels.setPixelColor((floor(millis()-startWaveTime)/200), pixels.Color(150,0,0));
    }
    
    pixels.show();
    
    delay(20);                             // arbitrary delay to limit data to serial port 
}
