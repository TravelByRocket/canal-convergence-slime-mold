#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <CapacitiveSensor.h>
#include <wifitechwrangler2.h>

//////////////////////////////////
// TABLE OF COLORS TO TRY ////////
//////////////////////////////////
//  102,255,51 too pastel of a green
//  205,250,5 good green-yellow (mostly yellow) toxic color
//  75,250,5 good green-yellow (mostly green) toxic color
//  120,250,5 good green-yellow in between the two above
//  179,234,68 weak
//  249,91,6 weak
//  250,172,5
//  61,213,195
//  6,249,212
//  255,30,0 a good orange-red

// CURRENT KEEPER SET
//  200,30,0 orange-red
//  33,69,0 limey greeny
//  0,70,30 turquise

int insideR = 33;
int insideG = 67;
int insideB = 0;

int outsideR = 0;
int outsideG = 70;
int outsideB = 30;

const int NUMSTRIPS = 4;
const int LEDPINS[] = {4,0,2,15};

// the number of pixels used on each strip
int NUMPIXSTRIP[] = {75,
                     75,
                     75,
                     150};
// NOTE: Write to every pixel in order to erase any spurious signals

CRGB ledstrips[NUMSTRIPS][150];

int delayval = 70; // delay between loops in ms

int activationIndex = 0;
int fingersActiveLL = 0;
int fingersActiveLR = 0;
int fingersActiveSS = 0;
int fingersActiveAll = 0;

////////////////////////////////////////
// UDP SETUP START /////////////////////
////////////////////////////////////////

unsigned int localPort = 8052;
const int packetSize = 6; // 'fxyyyy' format is 6 plus null terminator
char packetBuffer[packetSize + 1]; //buffer to hold incoming packet,

WiFiUDP Udp;

////////////////////////////////////////
// MAIN CODE START /////////////////////
////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of repeating `?`

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);
//  setupWiFiMulti(); // used to select from preferred networks as contained in my custom library file // NOTE: make a class?
  WiFi.begin(SSID.c_str(),PASS.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Network name: ");
  Serial.println(WiFi.SSID());
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  setupOTA(); // run function to enable over-the-air updating

  FastLED.addLeds<NEOPIXEL, 4>(ledstrips[0],150);
  FastLED.addLeds<NEOPIXEL, 0>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL, 2>(ledstrips[2],150);
  FastLED.addLeds<NEOPIXEL,15>(ledstrips[3],150);

  Udp.begin(localPort);
}

void loop() {
  ArduinoOTA.handle();
  loopMedallion();
  handleIncomingUDP();
  delay(delayval);
}

void setupOTA(){
  ArduinoOTA.setHostname("medallion");

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

bool isGrowing = true;

void loopMedallion() {

  // designing interactions
  

  // if 0-2 fingers are touched -- breathe between two colors as we have already designed, using the toxic yellow and toxic green
  // transition to more touches emanate the red/orange from the center
  // transition to less touches emanate ColorA from center so red goes extinct
  // if 3-4 fingers are touched -- trigger 100%, trim ~30%; red and orange alternate emanating from the center but stopping at ~1/4 to ~1/3 out from the center; make the distance they travel adjustable and make it the lesser for 3 touches and greater for 4 touches; trigger radius 50-100% but trimmed to 25-35%
  // 
  // 
  // if 5-6 points are touched -- trigger 100%, trim ~30% on medallion and not trim on filament; same as the previous state but the emanation continues all the way out on the filament and the color rotation will be red turquise orange turqoise
  // 
  // 
  // if 7 points are touched -- trigger 100%, no trim
  // 
  // 
  // if 8 points are touched -- trigger 75%, no trim
  // 
  // 
  // if 9 points are touched -- emanate a ColorC that breathes between turqoise and cyan but with the action of the normal state

  if(isGrowing){
    activationIndex++;
  } else {
    activationIndex--;
  }

  if(activationIndex == 75 || activationIndex == 0){
    isGrowing = !isGrowing;
  }
  
  for(int k=0; k < 75; k++){ // set each pixel color
    if(k < activationIndex){
      setPixelColorMedallion(k,insideR,insideG,insideB);
    } else {
      setPixelColorMedallion(k,outsideR,outsideG,outsideB);
    }
  }

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

void handleIncomingUDP(){
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int n = Udp.read(packetBuffer, packetSize);
    Udp.read(packetBuffer, packetSize);
    packetBuffer[n] = 0;
    Serial.print("Contents: ");
    Serial.print(packetBuffer);
    Serial.print("... at millis()%10000 of ");
    Serial.print(millis()%10000); 
    Serial.println("");

    // strings from longSiteLeft
    if(packetBuffer[0] == 'l' && packetBuffer[1] == 'l'){ // if it is long site left
      fingersActiveLL = (int) packetBuffer[2] - '0';
    }

    // strings from finger 2
    if(packetBuffer[0] == 'l' && packetBuffer[1] == 'r'){ // if it is long site right
      fingersActiveLR = (int) packetBuffer[2] - '0';
    }

    // strings from finger 3
    if(packetBuffer[0] == 's' && packetBuffer[1] == 's'){ // if it is short site
      fingersActiveSS = (int) packetBuffer[2] - '0';
    }

    // strings from finger 3
    if(packetBuffer[0] == '0' && packetBuffer[1] == '0'){ // if reset code sent
      ESP.restart();
    }

    fingersActiveAll = fingersActiveLL + fingersActiveLR + fingersActiveSS;

    // change colors
    if(packetBuffer[0] == 'c'){ // c for color
      int colorVal = (int(packetBuffer[3] - '0') * 100) 
                   + (int(packetBuffer[4] - '0') * 10) 
                   + (int(packetBuffer[5] - '0')); // convert characters to a 0-255 integer
      if(packetBuffer[1] == 'i' && colorVal >= 0 && colorVal <= 255){ // if for inside medallion color
        if       (packetBuffer[2] == 'r'){insideR = colorVal;
        } else if(packetBuffer[2] == 'g'){insideG = colorVal;
        } else if(packetBuffer[2] == 'b'){insideB = colorVal;
        }
      }
      if(packetBuffer[1] == 'o' && colorVal >= 0 && colorVal <= 255){ // if for outside medallion color
        if       (packetBuffer[2] == 'r'){outsideR = colorVal;
        } else if(packetBuffer[2] == 'g'){outsideG = colorVal;
        } else if(packetBuffer[2] == 'b'){outsideB = colorVal;
        }
      }
    }
    
  }
}

// CONSIDER THIS FROM OTABASIC TO RECONNET ON CONNECTION LOSS
// while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//   Serial.println("Connection Failed! Rebooting...");
//   delay(5000);
//   ESP.restart();
