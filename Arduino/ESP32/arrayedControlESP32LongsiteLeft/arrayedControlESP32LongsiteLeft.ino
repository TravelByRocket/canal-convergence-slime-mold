#include <FastLED.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <bryanwifinetworks.h>

const int NUMSTRIPS = 4;
const int NUMFINGERS = 3;
const int NUMARMS = 2;

const int CAPTOUCHPINS[] = {14,33,15}; // 14 and 15 match the board; S/W pin 33 is labeled pin 32 on HUZZAH32...
// ... The pin seems like it could be jumping between 32 and 33 based on the libraries I have active. Possible?
const int LEDPINS[] = {26,25,27,4}; // board labels A0,A1,27,A5

// the number of pixels used on each strip
int NUMPIXSTRIP[] = {150, // Strip 1; all LEDs used
                     150, // Strip 2; all LEDs used
                     130, // Strip 3; 29 LEDs unused at end
                     130}; // Strip 4; 30 LEDs unused at end

int NUMPIXFINGER[] = {110,121,102}; // the number of pixels per finger
int NUMPIXARM[] = {120,88}; // the number of pixels per arm section

CRGB ledstrips[NUMSTRIPS][150];

int delayval = 50; // delay between loops in ms

int CAPTOUCHTHRESH[] = {20,20,20}; // touch is true if below this value
int capTouchVal[NUMFINGERS];
int fingerActiveToIndex[] = {0,0,0};
bool fingerIsGrowing[] = {false,false,false};
bool fingerIsFull[] = {false,false,false};

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of repeating `?`

  WiFi.mode(WIFI_STA);
  setupWiFiMulti(); // used to select from preferred networks as contained in my custom library file // NOTE: make a class?
  setupOTA(); // run function to enable over-the-air updating

  FastLED.addLeds<NEOPIXEL, 26>(ledstrips[0],150);
  FastLED.addLeds<NEOPIXEL, 25>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL, 27>(ledstrips[2],150);
  FastLED.addLeds<NEOPIXEL,  4>(ledstrips[3],150);
}

void loop() {
  ArduinoOTA.handle();
  loopLongSiteLeft();
  delay(delayval);
}

void setupOTA(){
  ArduinoOTA.setHostname("esp32longSiteLeft");

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

void loopLongSiteLeft() {
  
  int junk = touchRead(CAPTOUCHPINS[0]); // the first reading on each loop tends to be bad so put in `junk` temp var
  for (int i = 0; i < NUMFINGERS; i++){ // read each of the cap touch pins and run logic for corresponding finger; one cap touch per finger
    capTouchVal[i] = touchRead(CAPTOUCHPINS[i]);
    Serial.print("touchRead at pin ");
    Serial.print(CAPTOUCHPINS[i]);
    Serial.print(" is ");
    Serial.println(capTouchVal[i]);
  }
  // delay(20);

  for (int p = 0; p < NUMFINGERS; p++){
    if (capTouchVal[p] < CAPTOUCHTHRESH[p]){
      fingerIsGrowing[p] = true;
      fingerActiveToIndex[p] += 2;
    } else {
      fingerIsGrowing[p] = false;
      fingerActiveToIndex[p] -= 2;
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
      setPixelColorArm(r,s,50,0,50);
    }
  }

  // delay(100);
  for(int j=0; j < 4; j++){
    delay(20);
    FastLED.show();
  }

}

void setPixelColorFinger(int finger, int i, int r, int g, int b){
  switch(finger){
    case 0: // Finger 1
      ledstrips[0][150 - 1 - i].setRGB(r,g,b); // 
      break;
    case 1: // Finger 2
      ledstrips[2][121 - 1 - i].setRGB(r,g,b); // 
      break;
    case 2: // Finger 3
      ledstrips[3][120 - 1 - i].setRGB(r,g,b); // 
      break;
    default:
      Serial.print("something went wrong");
      break;
  }
}

void setPixelColorArm(int arm, int i, int r, int g, int b){
  switch(arm){
    case 0: // Arm 1
      if(i < 40){
        ledstrips[0][40 - i].setRGB(r,g,b); // 
      } else {
        ledstrips[1][i - 40].setRGB(r,g,b); // 
      }
      break;
    case 1: // Arm 2
      if(i < 70){
        ledstrips[1][i + 80].setRGB(r,g,b); // 
      } else {
        ledstrips[3][i - 70].setRGB(r,g,b); // 
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