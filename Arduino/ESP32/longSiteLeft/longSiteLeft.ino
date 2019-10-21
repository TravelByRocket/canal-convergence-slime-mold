#include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h> // used by OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <wifitechwrangler2.h>

////////////////////////////////////////
// COLORS //////////////////////////////
////////////////////////////////////////

// initial 'a' greenish color
int aRed = 33;
int aGre = 67;
int aBlu = 0;

// second 'b' reddish color
int bRed = 85;
int bGre = 15;
int bBlu = 0;

// third 'c' turqoise color
int cRed = 0;
int cGre = 70;
int cBlu = 30;

////////////////////////////////////////
// LED SETUP START /////////////////////
////////////////////////////////////////

const int NUMSTRIPS = 4;
const int NUMFINGERS = 3;
const int NUMARMS = 2;
const int LEDPINS[] = {26,25,27,4}; // board labels A0,A1,27,A5

// the number of pixels used on each strip 
int NUMPIXSTRIP[] = {150, // Strip 1; all LEDs used
                     150, // Strip 2; all LEDs used
                     130, // Strip 3; 29 LEDs unused at end
                     130}; // Strip 4; 30 LEDs unused at end
// NOTE: Write to every pixel in order to erase any spurious signals

int NUMPIXFINGER[] = {110,121,102}; // the number of pixels per finger
int NUMPIXARM[] = {120,88}; // the number of pixels per arm section

CRGB ledstrips[NUMSTRIPS][150];

////////////////////////////////////////
// GENERAL SETTINGS START //////////////
////////////////////////////////////////

int delayval = 25; // delay between loops in ms

int fingerActiveToIndex[] = {0,0,0};
bool fingerIsGrowing[] = {false,false,false};
bool fingerIsFull[] = {false,false,false};

////////////////////////////////////////
// UDP SETUP START /////////////////////
////////////////////////////////////////

unsigned int localPort = 8052;
const int packetSize = 6; // 'fxyyyy' format is 6 plus null terminator
char packetBuffer[packetSize + 1]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged\0"; // a string to send back // 12 chars + terminator => 13
char  ReplyBuffer2[] = "heartbeat...\0"; // a string to send back // 12 chars + terminator => 13
const char * addressLongSiteRight = "192.168.0.101";

WiFiUDP Udp;

////////////////////////////////////////
// CUSTOM FUNCTIONS AT TOP (?BETA IDE BUG?)
////////////////////////////////////////

void setupOTA(){
  ArduinoOTA.setHostname("longSiteLeft");

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

void loopLongSiteLeft() {

  for (int p = 0; p < NUMFINGERS; p++){
    if (fingerIsGrowing[p]){
      fingerActiveToIndex[p]++;
    } else {
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
        setPixelColorFinger(n,k,bRed,bGre,bBlu);
      } else {
        setPixelColorFinger(n,k,aRed,aGre,aBlu);
      }
    }
  }

  for(int r=0; r<NUMARMS; r++){ // this makes it color a by default, color b is an adjacent 
    // ... finger is active, and c if both adjacent fingers are active
    // need to make it travel to midway before stopping and then treat the meeting point as where the turqoise starts
    for(int s=0; s<NUMPIXARM[r]; s++){
      if(fingerIsFull[r] && fingerIsFull[r+1]){
        setPixelColorArm(r,s,cRed,cGre,cBlu);
      } else if (fingerIsFull[r] || fingerIsFull[r+1]){
        setPixelColorArm(r,s,bRed,bGre,bBlu);
      } else {
        setPixelColorArm(r,s,aRed,aGre,aBlu);
      }

      //
      
    }
  }

  delay(20);
  FastLED.show();

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

    // strings from finger 1
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '1'){ // if it is finger 1
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        fingerIsGrowing[0] = true;
      } else if (packetBuffer[2] == '0'){
        fingerIsGrowing[0] = false;
      }
    }

    // strings from finger 2
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '2'){ // if it is finger 2
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        fingerIsGrowing[1] = true;
      } else if (packetBuffer[2] == '0'){
        fingerIsGrowing[1] = false;
      }
    }

    // strings from finger 3
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '3'){ // if it is finger 3
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        fingerIsGrowing[2] = true;
      } else if (packetBuffer[2] == '0'){
        fingerIsGrowing[2] = false;
      }
    }

    if(packetBuffer[0] == 'c'){ // c for color
      int colorVal = (int(packetBuffer[3] - '0') * 100) 
                   + (int(packetBuffer[4] - '0') * 10) 
                   + (int(packetBuffer[5] - '0')); // convert characters to a 0-255 integer
      if(packetBuffer[1] == 'a' && colorVal >= 0 && colorVal <= 255){ // if for initial green/toxic color 'a'
        if       (packetBuffer[2] == 'r'){aRed = colorVal;
        } else if(packetBuffer[2] == 'g'){aGre = colorVal;
        } else if(packetBuffer[2] == 'b'){aBlu = colorVal;
        }
      }
      if(packetBuffer[1] == 'b' && colorVal >= 0 && colorVal <= 255){ // if for second reddish color 'b'
        if       (packetBuffer[2] == 'r'){bRed = colorVal;
        } else if(packetBuffer[2] == 'g'){bGre = colorVal;
        } else if(packetBuffer[2] == 'b'){bBlu = colorVal;
        }
      }
      if(packetBuffer[1] == 'c' && colorVal >= 0 && colorVal <= 255){ // if for third turquise color 'c'
        if       (packetBuffer[2] == 'r'){cRed = colorVal;
        } else if(packetBuffer[2] == 'g'){cGre = colorVal;
        } else if(packetBuffer[2] == 'b'){cBlu = colorVal;
        }
      }
    }
    
  }
}

void sendToLongSiteRight(){
    Udp.beginPacket(addressLongSiteRight,localPort);
    Udp.write((const uint8_t*)ReplyBuffer, 12);
    Udp.endPacket();
}

////////////////////////////////////////
// MAIN CODE START /////////////////////
////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of repeating `?`

  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
  WiFi.mode(WIFI_STA);
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
  
  FastLED.addLeds<NEOPIXEL, 26>(ledstrips[0],150);
  FastLED.addLeds<NEOPIXEL, 25>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL, 27>(ledstrips[2],150);
  FastLED.addLeds<NEOPIXEL,  4>(ledstrips[3],150);

  Udp.begin(localPort);
}

void loop() {
  ArduinoOTA.handle();
  handleIncomingUDP();
  loopLongSiteLeft();
  delay(delayval);
}

// CONSIDER THIS FROM OTABASIC TO RECONNET ON CONNECTION LOSS
// while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//   Serial.println("Connection Failed! Rebooting...");
//   delay(5000);
//   ESP.restart();
