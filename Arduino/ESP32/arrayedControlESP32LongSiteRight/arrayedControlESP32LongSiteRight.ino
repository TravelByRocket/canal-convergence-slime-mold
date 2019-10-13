#include <FastLED.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <bryanwifinetworks.h>

const int NUMSTRIPS = 3;
const int NUMFINGERS = 2;
const int NUMARMS = 2;

const int CAPTOUCHPINS[] = {14,33}; // 14 and 15 match the board; S/W pin 33 is labeled pin 32 on HUZZAH32...
// ... The pin seems like it could be jumping between 32 and 33 based on the libraries I have active. Possible?
const int LEDPINS[] = {26,27,4}; // board labels A0,27,A5

// the number of pixels used on each strip
int NUMPIXSTRIP[] = {132, // Strip 1; all LEDs used
                     134, // Strip 2; all LEDs used
                     150}; // Strip 3; 29 LEDs unused at end};
// NOTE: Write to every pixel in order to erase any spurious signals

int NUMPIXFINGER[] = {134,140}; // the number of pixels per finger
int NUMPIXARM[] = {76,56}; // the number of pixels per arm section

CRGB ledstrips[NUMSTRIPS][150];

int delayval = 25; // delay between loops in ms

int CAPTOUCHTHRESH[] = {12,12}; // touch is true if below this value; decrease value of triggered without touch
int capTouchVal[NUMFINGERS];
int fingerActiveToIndex[] = {0,0};
bool fingerIsGrowing[] = {false,false};
bool fingerIsFull[] = {false,false};

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of repeating `?`

  WiFi.mode(WIFI_STA);
  setupWiFiMulti(); // used to select from preferred networks as contained in my custom library file // NOTE: make a class?
  setupOTA(); // run function to enable over-the-air updating

  FastLED.addLeds<NEOPIXEL, 26>(ledstrips[0],150);
  FastLED.addLeds<NEOPIXEL, 27>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL,  4>(ledstrips[2],150);
}

void loop() {
  ArduinoOTA.handle();
  loopLongSiteRight();
  
  delay(delayval);
}

void setupOTA(){
  ArduinoOTA.setHostname("esp32longSiteRight");

  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();
}

void loopLongSiteRight() {
  
//  int junk = touchRead(CAPTOUCHPINS[0]); // the first reading on each loop tends to be bad so put in `junk` temp var
  for (int i = 0; i < NUMFINGERS; i++){ // read each of the cap touch pins and run logic for corresponding finger; one cap touch per finger
    capTouchVal[i] = digitalRead(CAPTOUCHPINS[i]);
    Serial.print("touchRead at pin ");
    Serial.print(CAPTOUCHPINS[i]);
    Serial.print(" is ");
    Serial.println(capTouchVal[i]);
  }
  // delay(20);

  for (int p = 0; p < NUMFINGERS; p++){
    if (capTouchVal[p] == HIGH){
      fingerIsGrowing[p] = true;
      fingerActiveToIndex[p]++;
    } else {
      fingerIsGrowing[p] = false;
      fingerActiveToIndex[p]--;
    }  
  }
  
  for(int m = 0; m < NUMFINGERS; m++){ // check indices for each finger
    if(fingerActiveToIndex[m] > NUMPIXFINGER[m]){ // prevent exceeding numpix
      fingerActiveToIndex[m] = NUMPIXFINGER[m];
      fingerIsFull[m] = true;
    } else if(fingerActiveToIndex[m] < 0){ // prevent negative index
      fingerActiveToIndex[m] = 0;
    } else {
      fingerIsFull[m] = false; // if it is not at the end of the index it will drop in here
    }
  }
  
  for(int n = 0; n < NUMFINGERS; n++){ // update colors for each finger
    for(int k=0; k < NUMPIXFINGER[n]; k++){ // set each pixel color
      if(k < fingerActiveToIndex[n]){
        setPixelColorFinger(n,k,100,0,0);
      } else {
        setPixelColorFinger(n,k,0,50,50);
      }
    }
  }

  for(int r=0; r<NUMARMS; r++){
    for(int s=0; s<NUMPIXARM[r]; s++){
      setPixelColorArm(r,s,0,50*(1-r),50*r);
    }
  }

  delay(20);
  FastLED.show();

}

void setPixelColorFinger(int finger, int i, int r, int g, int b){
  switch(finger){
    case 0: // Finger 4
      ledstrips[1][150 - 1 - i].setRGB(r,g,b); // 
      break;
    case 1: // Finger 5
      ledstrips[2][134 - 1 - i].setRGB(r,g,b); // 
      break;
    default:
      Serial.print("something went wrong");
      break;
  }
}

void setPixelColorArm(int arm, int i, int r, int g, int b){
  switch(arm){
    case 0: // Arm 3
      ledstrips[0][132 - i].setRGB(r,g,b); // 
      break;
    case 1: // Arm 4
      if(i < 56){
        ledstrips[0][56 - i].setRGB(r,g,b); // 
      } else {
        ledstrips[1][i - 56].setRGB(r,g,b); // 
      }
      break;
    default:
      Serial.print("something went wrong");
      break;
  }
}


// CONSIDER THIS FROM OTABASIC TO RECONNET ON CONNECTION LOSS
// while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//   Serial.println("Connection Failed! Rebooting...");
//   delay(5000);
//   ESP.restart();

// UNUSED ALTERNATIVE IMPLEMENTATION OF SELF-ID FUNCTION
// bool isID_alt(int bit0, int bit1, int bit2){ 
//   int IDbits = 3;
//   int bitID[] = {bit0,bit1,bit2};
//   bool bitMatches[IDbits];

//   for (int i = 0; i < IDbits; i++){
//     if (bitID[i] = digitalRead(SELFIDPINS[i])){
//       bitMatches[i] = true;
//     } else {
//       bitMatches[i] = false;
//     }
//   }

//   for (int j = 0; j < IDbits; j++){
//     if (bitMatches[0] && bitMatches[1] && bitMatches[2]){
//       return true;
//     } else {
//       return false
//     }
//   }

// }
