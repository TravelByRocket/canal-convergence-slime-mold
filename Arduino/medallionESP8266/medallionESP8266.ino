#include <FastLED.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <bryanwifinetworks.h>

const int NUMSTRIPS = 4;
const int LEDPINS[] = {4,0,2,15};

// the number of pixels used on each strip
int NUMPIXSTRIP[] = {75,
                     75,
                     75,
                     150};
// NOTE: Write to every pixel in order to erase any spurious signals

CRGB ledstrips[NUMSTRIPS][150];

int delayval = 25; // delay between loops in ms

int activationIndex = 0;

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of repeating `?`

  WiFi.mode(WIFI_STA);
  setupWiFiMulti(); // used to select from preferred networks as contained in my custom library file // NOTE: make a class?
  setupOTA(); // run function to enable over-the-air updating

  FastLED.addLeds<NEOPIXEL, 4>(ledstrips[0],150);
  FastLED.addLeds<NEOPIXEL, 0>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL, 2>(ledstrips[2],150);
  FastLED.addLeds<NEOPIXEL,15>(ledstrips[3],150);
}

void loop() {
  ArduinoOTA.handle();
  loopMedallion();

  delay(delayval);
}

void setupOTA(){
  ArduinoOTA.setHostname("esp32longSiteRight");

    ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }

    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void loopMedallion() {
  
  for(int k=0; k < 150; k++){ // set each pixel color
    if(k < activationIndex){
      setPixelColorMedallion(k,100,0,0);
    } else {
      setPixelColorMedallion(k,0,50,50);
    }
  }

  if(activationIndex < 150){
    activationIndex++;
  } else {
    activationIndex = 0;
  }

  delay(20);
  FastLED.show();

}

void setPixelColorMedallion(int i, int r, int g, int b){

  if(i < 37){
    ledstrips[0][37 + i].setRGB(r,g,b);
    ledstrips[0][37 - i].setRGB(r,g,b);
    
    ledstrips[1][37 + i].setRGB(r,g,b);
    ledstrips[1][37 - i].setRGB(r,g,b);
    
    ledstrips[2][37 + i].setRGB(r,g,b);
    ledstrips[2][37 - i].setRGB(r,g,b);
    
    ledstrips[3][75 + i].setRGB(r,g,b);
    ledstrips[3][75 - i].setRGB(r,g,b);
  } else {
    ledstrips[3][75 + i].setRGB(r,g,b);
    ledstrips[3][75 - i].setRGB(r,g,b);
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
