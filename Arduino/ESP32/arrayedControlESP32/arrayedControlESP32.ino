// LED controller code for two HUZZAH32 boards controlling one long site of LEDs
// Same code is used for both positions and is run selectively using SELFID pins
// 

#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <bryanwifinetworks.h>

const int SELFIDPINS[] = {18,19,16}; // board labels A2,A3,A4
const int CAPTOUCHPINS[] = {14,33,15}; // 14 and 15 match the board; S/W pin 33 is labeled pin 32 on HUZZAH32
const int LEDPINS[] = {26,25,27,33}; // board labels A0,A1,27,33

enum locations {longSiteLeft,longSiteRight}; // selected later usin SELFID pins
enum locations identity; // the identity name describes where it is

// the number of pixels used on each strip
int NUMPIXSTRIP[] = {150, // Strip 1; all LEDs used
                     150, // Strip 2; all LEDs used
                     130, // Strip 3; 29 LEDs unused at end
                     130, // Strip 4; 30 LEDs unused at end
                     150, // Strip 5; 18 LEDs unsed at start
                     150, // Strip 6; 16 LEDs unused at end
                     150};// Strip 7; all LEDs used

// the number of pixels per finger
int NUMPIXFINGER[] = {100,
                       121,
                       102,
                       134,
                       140};

// the number of pixels per arm section
int NUMPIXARM[] = {120,
                   88,
                   76,
                   56};

const int NUMSTRIPS = 7;
const int NUMFINGERS = 5;
const int NUMARMS = 4;

Adafruit_NeoPixel ledstrips[4]; // var declaration here; initialized with FOR loop in SETUP

int delayval = 50; // delay between loops in ms

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of repeating `?`

  // WiFi.mode(WIFI_STA);
  // setupWiFiMulti(); // used to select from preferred networks as contained in my custom library file // NOTE: make a class?
  // setupOTA(); // run function to enable over-the-air updating

  for (int i = 0; i < 4; i++){
    ledstrips[i] = Adafruit_NeoPixel(NUMPIXSTRIP[i], LEDPINS[i], NEO_GRB + NEO_KHZ800);
    // ledstrips[i].begin();
  }

  for (int i = 0; i < 4; i++){
    // ledstrips[i] = Adafruit_NeoPixel(NUMPIXSTRIP[i], LEDPINS[i], NEO_GRB + NEO_KHZ800);
    ledstrips[i].begin();
  }

  selfIdentify();
  
  setupSite(identity);
  Serial.println("setup loop completed");
}

void loop() {
  // Serial.println("point 1");
  ArduinoOTA.handle();
  // Serial.println("point 2");
  loopSite(identity);
  // Serial.println("point 3");
  delay(delayval);
  // Serial.println("point 4");
}

void selfIdentify() {
  if        (isID(1,0,0)) { // for ID 0x100
    identity = longSiteLeft;
    Serial.print("I am longSiteLeft, ID 0x100, enum ");
  } else if (isID(0,1,0)) { // for ID 0x010
    identity = longSiteRight;
    Serial.print("I am longSiteRight, ID 0x010, enum ");
  } else {
    Serial.print("there is a problem");
  }
  Serial.println(identity);
}

bool isID(int bit0, int bit1, int bit2){
  // return true if all ID bits match, otherwise false
  int bitID[] = {bit0,bit1,bit2};

  if        (bitID[0] != digitalRead(SELFIDPINS[0])) {
    return false;
  } else if (bitID[1] != digitalRead(SELFIDPINS[1])) {
    return false;
  } else if (bitID[2] != digitalRead(SELFIDPINS[2])) {
    return false;
  } else {
    return true;
  }

}

void setupOTA(){
  if(identity == longSiteLeft){
      ArduinoOTA.setHostname("esp32longSiteLeft");
  } else if(identity == longSiteRight){
      ArduinoOTA.setHostname("esp32longSiteRight");
  }

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

void setupSite(enum locations which){ // called at the end up SETUP
  if (which == longSiteLeft){
    setupLongSiteLeft();
  } else if (which == longSiteRight) {
    setupLongSiteRight();
  } 
}

void loopSite(enum locations which){
  if (which == longSiteLeft){
    loopLongSiteLeft();
  } else if (which == longSiteRight) {
    loopLongSiteRight();
  }
}



void setupLongSiteLeft() {
  // for (int i = 0; i < 4; i++){
  //   ledstrips[i] = Adafruit_NeoPixel(NUMPIXSTRIP[i], LEDPINS[i], NEO_GRB + NEO_KHZ800);
  //   ledstrips[i].begin();
  // }
  
  
}

void setupLongSiteRight() {
  // for (int i = 4; i < NUMSTRIPS; i++){
  //   ledstrips[i] = Adafruit_NeoPixel(NUMPIXSTRIP[i], LEDPINS[i], NEO_GRB + NEO_KHZ800);
  //   ledstrips[i].begin();
  // }

}


// COMMON VARIABLES FOR LOOPS
int CAPTOUCHTHRESH[] = {20,20,20}; // touch is true if below this value
int capTouchVal[5];
int activeToIndex[] = {0,0,0,0,0,0,0};
bool isGrowing[] = {false,false,false,false,false};
bool isFull[] = {false,false,false,false,false};


void loopLongSiteLeft() {
  
  int junk = touchRead(CAPTOUCHPINS[0]); // the first reading on each loop tends to be bad
  for (int i = 0; i < 3; i++){ // read each of the cap touch pins and run logic for corresponding finger
    capTouchVal[i] = touchRead(CAPTOUCHPINS[i]);
    if (capTouchVal[i] < CAPTOUCHTHRESH[i]){
      isGrowing[i] = true;
      activeToIndex[i] += 2;
    } else {
      isGrowing[i] = false;
      activeToIndex[i] -= 2;
    }
  }
  delay(20);

  for(int m = 0; m < 3; m++){ // check indices for each finger
    if(activeToIndex[m] > NUMPIXFINGER[m]){ // prevent exceeding numpix
      activeToIndex[m] = NUMPIXFINGER[m];
      isFull[m] = true;
    } else if(activeToIndex[m] < 0){ // prevent negative index
      activeToIndex[m] = 0;
    } else {
      isFull[m] = false; // if it is not at the end of the index it will drop in here
    }
  }
  
  for(int n = 0; n < 3; n++){ // update colors for each finger
    for(int k=0; k < NUMPIXFINGER[n]; k++){ // set each pixel color
      // Serial.print("about to call setPixelColorFinger with finger and pixel ");
      // Serial.print(n);
      // Serial.print(" ");
      // Serial.println(k);
      if(k < activeToIndex[n]){
        setPixelColorFinger(n,k,100,0,0);
      } else {
        setPixelColorFinger(n,k,0,50,50);
      }
    }
  }

  // delay(100);
  for(int j=0; j < 4; j++){
    delay(20);
    ledstrips[j].show();
  }

}

void loopLongSiteRight() {

}

void setPixelColorFinger(int finger, int i, int r, int g, int b){
  switch(finger){
    case 0: // Finger 1
      ledstrips[0].setPixelColor(150 - i,r,g,b); // 
      break;
    case 1: // Finger 2
      ledstrips[2].setPixelColor(121 - i,r,g,b); // 
      break;
    case 2: // Finger 3
      ledstrips[3].setPixelColor(120 - i,r,g,b); // 
      break;
    case 3: // Finger 4
      ledstrips[5].setPixelColor(134 - i,r,g,b); // 
      break;
    case 4: // Finger 5
      ledstrips[6].setPixelColor(150 - i,r,g,b); // 
      break;
    default:
    // this should not occur
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