#include <FastLED.h>
#include <WiFi.h>
//#include <WiFiMulti.h>
//WiFiMulti wifiMulti; // must keep above bryanwifinetworks.h
#include <ESPmDNS.h> // used by OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
//#include <bryanwifinetworks.h>
#include <wifitechwrangler2.h>

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

// GET TOUCH VALUES HERE

//UNCOMMENT THIS SECTION WHEN ABLE TO PROCESS UDP
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
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '1' && packetBuffer[2] == '0'){
      fingerIsGrowing[0] = false;
    } else if (packetBuffer[0] == 'f' && packetBuffer[1] == '1' && packetBuffer[2] == '1'){
      fingerIsGrowing[0] = true;
    } else {
//      Serial.println("Not finger 1");
    }

    // strings from finger 2
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '2' && packetBuffer[2] == '0'){
      fingerIsGrowing[1] = false;
    } else if (packetBuffer[0] == 'f' && packetBuffer[1] == '2' && packetBuffer[2] == '1'){
      fingerIsGrowing[1] = true;
    } else {
//      Serial.println("Not finger 2");
    }

    // strings from finger 3
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '3' && packetBuffer[2] == '0'){
      fingerIsGrowing[2] = false;
    } else if (packetBuffer[0] == 'f' && packetBuffer[1] == '3' && packetBuffer[2] == '1'){
      fingerIsGrowing[2] = true;
    } else {
//      Serial.println("Not finger 3");
    }
    
  }
}

void sendToLongSiteRight(){
    Udp.beginPacket(addressLongSiteRight,localPort);
//    Udp.write(ReplyBuffer);
    Udp.write((const uint8_t*)ReplyBuffer, 12); // keep until the line above is working
//    Udp.write((const uint8_t*)ReplyBuffer); // keep until the line above is working
    Udp.endPacket();
}

////////////////////////////////////////
// MAIN CODE START /////////////////////
////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  Serial.println(""); //get out of the way of the line of repeating `?`

  WiFi.mode(WIFI_STA);
//  setupWiFiMulti(); // used to select from preferred networks as contained in my custom library file // NOTE: make a class?
  WiFi.begin(SSID.c_str(),PASS.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
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
