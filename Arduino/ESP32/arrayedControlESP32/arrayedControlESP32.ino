#include <Adafruit_NeoPixel.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <bryanwifinetworks.h>

const int SELFIDPINS[] = {34,39,36}; // board labels A2,A3,A4
const int CAPTOUCHPINS[] = {14,32,15}; // board labels match
const int LEDPINS[] = {26,25,27,33}; // board labels A0,A1,27,33

enum locations {longSiteLeft,longSiteRight,shortSiteLeft,shortSiteRight,shortSite};
enum locations identity; // the identity name describes where it is

// How many NeoPixels are attached to the Arduino?
int NUMPIX[] = {150,150,130,130,150,150,150, // seven LED strips in long site
                150,150,150,150,150}; // five LED strips in the short site

int NUMPIXSECTA = 255; // 150px (5.0m) on strip 0 -> 105px (3.5m) on strip 1
int NUMPIXSECTB = 225; // 120px (4.0m) on strip 2 ->  45px (1.5m) on strip 1 -> 60px (2.0m) on strip 3
int NUMPIXSECTC = 150;
int NUMPIXSECTD = 150;

const int NUMSTRIPS = 12;
Adafruit_NeoPixel ledstrips[NUMSTRIPS];

int long startWave1 = 0;
int long startWave2 = 0;

int long activatedTimeSectA = 0; // time the sensor was touched
int long activatedTimeSectB = 0;

int long deactivatedTimeSectA = 0; // time the sensor was untouched
int long deactivatedTimeSectB = 0;

int wavePosSecA = 0;
int wavePosSecB = 0;
int waveSpeed = 50; // pixels/second

bool growingA = true;
bool growingB = true;

int delayval = 50; // delay between loops in ms

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of Q marks

  WiFi.mode(WIFI_STA);
  setupWiFiMulti(); // this comes from my custom library file // NOTE make a class?
  setupOTA();
  
  for (int i = 0; i < NUMSTRIPS; i++){
    ledstrips[i] = Adafruit_NeoPixel(NUMPIX[i], LEDPINS[i], NEO_GRB + NEO_KHZ800);
  }
  
  
  // ledstrip0.begin();
  // ledstrip1.begin();
  // ledstrip2.begin();
  // ledstrip3.begin();
  selfIdentify();
  
  setupSite(identity);

}

int traveler = 0;

void loop() {
  ArduinoOTA.handle();
  loopSite(identity);
  
  // if (wavePosSecA < NUMPIXSECTA && growingA){ // if growing but not reach the end
  //   wavePosSecA = (int) ((waveStartTimeA/1000) * waveSpeed); // CONSIDER change to += so that not completely reset each touch, i.e. can bounce
  //   wavePosSecA > NUMPIXSECTA ? wavePosSecA = NUMPIXSECTA; // must clamp the index max of NUMPIX
  // } else if (wavePosSecA > 0 && !growingA){ // if shrinking but not reached the start
  //   wavePosSecA = (int) NUMPIXSECTA - ((waveStartTimeA/1000) * waveSpeed);
  // }
  

  for(int m = 0; m < NUMPIXSECTA; m++){ // set base color
    setPixelColorSectA(m,0,50,50);
  }

  for(int n = 0; n < NUMPIXSECTB; n++){ // set base color
    setPixelColorSectB(n,50,0,50);
  }

  for (int p = 0; p < floor(millis()-startWave1)/25; p++){ // red from beginning of strip to end
      setPixelColorSectA(p,100,0,0);
  }

  for (int q = 0; q < floor(millis()-startWave2)/25; q++){ // red from beginning of strip to end
      setPixelColorSectB(q,100,0,0);
  }
  
  // ledstrip0.show();
  // ledstrip1.show();
  // ledstrip2.show();
  // ledstrip3.show();
  
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
    // ledstrip0.setPixelColor(150 - i,r,g,b); // against strip indices; offset by length of strip and subtract index
  } else if (i < 150 + 105) {
    // ledstrip1.setPixelColor(i - 150,r,g,b); // with strip indices; go with index but offset by length of strip
  }
}

void setPixelColorSectB(int i, int r, int g, int b){ //120, 45, 60
  if (i < 120) {
    // ledstrip2.setPixelColor(120 - i,r,g,b); // against strip indices; offset by length of strip and subtract index
  } else if (i < 120 + 45) {
    // ledstrip1.setPixelColor(i - 15,r,g,b); // with strip indices; offset by difference of indices between strips 2 and 1
  } else if (i < 105 + 45 + 60) {
    // ledstrip3.setPixelColor(i - 120 - 45,r,g,b); // with strip indices; offset by length of strip 2 and portion on strip 1
  }
}

void selfIdentify() {
  if        (isID(1,0,0)) { // for ID 0x100
    identity = longSiteLeft;
    Serial.print("I am longSiteLeft, ID 0x100, enum ");
  } else if (isID(0,1,0)) { // for ID 0x010
    identity = longSiteRight;
    Serial.print("I am longSiteRight, ID 0x010, enum ");
  } else if (isID(0,0,1)) { // for ID 0x001
    identity = shortSite;
    Serial.print("I am shortSite, ID 0x001, enum ");
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

void setupSite(enum locations which){
  if (which == longSiteLeft){
    setupLongSiteLeft();
  } else if (which == longSiteRight) {
    setupLongSiteRight();
  } else if (which == shortSite) {
    setupShortSite();
  }
}

void loopSite(enum locations which){
  if (which == longSiteLeft){
    loopLongSiteLeft();
  } else if (which == longSiteRight) {
    loopLongSiteRight();
  } else if (which == shortSite) {
    loopShortSite();
  }
}



void setupLongSiteLeft() {
  // for (int i = 0; i < 4){

  // }
  
}
void setupLongSiteRight() {

}
void setupShortSite() {

}
void loopLongSiteLeft() {

}
void loopLongSiteRight() {

}
void loopShortSite() {

}

// CONSIDER THIS FROM OTABASIC
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