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

const int NUMSTRIPS = 3;
const int NUMFINGERS = 2;
const int NUMARMS = 2;
const int LEDPINS[] = {26,27,4}; // board labels A0,27,A5

// the number of pixels used on each strip
int NUMPIXSTRIP[] = {132, // Strip 1; all LEDs used
                     134, // Strip 2; all LEDs used
                     150}; // Strip 3; 29 LEDs unused at end};
                     
// NOTE: Write to every pixel in order to erase any spurious signals

int NUMPIXFINGER[] = {134,140}; // the number of pixels per finger
int NUMPIXARM[] = {76,56}; // the number of pixels per arm section

CRGB ledstrips[NUMSTRIPS][150];

////////////////////////////////////////
// GENERAL SETTINGS START //////////////
////////////////////////////////////////

int delayval = 25; // delay between loops in ms

int fingerActiveToIndex[] = {0,0};
bool fingerIsGrowing[] = {false,false};
bool fingerIsFull[] = {false,false};

////////////////////////////////////////
// UDP SETUP START /////////////////////
////////////////////////////////////////

unsigned int localPort = 8052;
const int packetSize = 6; // 'fxyyyy' format is 6 plus null terminator
char packetBuffer[packetSize + 1]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged\0"; // a string to send back // 12 chars + terminator => 13
char  ReplyBuffer2[] = "heartbeat...\0"; // a string to send back // 12 chars + terminator => 13
const char * addressLongSiteLeft = "192.168.0.100";

WiFiUDP Udp;

////////////////////////////////////////
// CUSTOM FUNCTIONS AT TOP (?BETA IDE BUG?)
////////////////////////////////////////

void setupOTA(){
  ArduinoOTA.setHostname("longSiteRight");

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

// fill space to match lineswith other side of site


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


// fill space to match lineswith other side of site


void loopLongSiteRight() {

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

    // strings from finger 4
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '4'){ // if it is finger 4
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        fingerIsGrowing[0] = true;
      } else if (packetBuffer[2] == '0'){
        fingerIsGrowing[0] = false;
      }
    }

    // strings from finger 5
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '5'){ // if it is finger 4
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        fingerIsGrowing[1] = true;
      } else if (packetBuffer[2] == '0'){
        fingerIsGrowing[1] = false;
      }
    }


    // fill space to match lines with other side code






    
  }
}

void sendToLongSiteLeft(){
    Udp.beginPacket(addressLongSiteLeft,localPort);
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

  FastLED.addLeds<NEOPIXEL, 26>(ledstrips[0],150);
  FastLED.addLeds<NEOPIXEL, 27>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL,  4>(ledstrips[2],150);

  
  Udp.begin(localPort);
}

void loop() {
  ArduinoOTA.handle();
  handleIncomingUDP();
  loopLongSiteRight();
  delay(delayval);
}

// CONSIDER THIS FROM OTABASIC TO RECONNET ON CONNECTION LOSS
// while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//   Serial.println("Connection Failed! Rebooting...");
//   delay(5000);
//   ESP.restart();
