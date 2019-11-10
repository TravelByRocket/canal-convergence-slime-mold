#include <FastLED.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <CapacitiveSensor.h>
#include <wifitechwrangler2.h>
// #include <wifihotspot_rocketphonexs.h>

bool isMovingOutward = true;
CRGB colorFeedMedallion;
CRGB colorFeedFilament;
bool canTransition = false;
int stepsSinceLastChange = 0;
int programToRun = 0;
int colorChoice = 0;
int sourceIndex = 0;

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

const int medallionRadiusPx = 37;
const int filamentRadiusPx = 75; // this is more like a half of a length but everythung on this site is radial so keeping with that nomenclature

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
  
  Serial.print("------------");
  Serial.println("");

  Serial.print("number of fingers touched is\t");
  Serial.print(fingersActiveAll);
  Serial.println("");

  handleIncomingUDP();

  handleColoring();

  ArduinoOTA.handle();
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

void medallion2stripRGB(int i, int r, int g, int b){
  ledstrips[0][medallionRadiusPx + i].setRGB(r,g,b);
  ledstrips[0][medallionRadiusPx - i].setRGB(r,g,b);
  
  ledstrips[1][medallionRadiusPx + i].setRGB(r,g,b);
  ledstrips[1][medallionRadiusPx - i].setRGB(r,g,b);
  
  ledstrips[2][medallionRadiusPx + i].setRGB(r,g,b);
  ledstrips[2][medallionRadiusPx - i].setRGB(r,g,b);
  
  ledstrips[3][filamentRadiusPx + i].setRGB(r,g,b);
  ledstrips[3][filamentRadiusPx - i].setRGB(r,g,b);
}

void filament2stripRGB(int i, int r, int g, int b){
  ledstrips[3][filamentRadiusPx + i].setRGB(r,g,b);
  ledstrips[3][filamentRadiusPx - i].setRGB(r,g,b);
}

void medallionShiftOutward(){
  for(int destinationIndex=medallionRadiusPx; destinationIndex>0; destinationIndex--){ // start at outside of range and move inward as colors are shifted outward
    sourceIndex = destinationIndex - 1;
    ledstrips[0][medallionRadiusPx + destinationIndex] = ledstrips[0][medallionRadiusPx + sourceIndex]; // each pixel get assigned to the color just inside of it
    ledstrips[0][medallionRadiusPx - destinationIndex] = ledstrips[0][medallionRadiusPx - sourceIndex]; // on both side
    ledstrips[1][medallionRadiusPx + destinationIndex] = ledstrips[1][medallionRadiusPx + sourceIndex]; // each pixel get assigned to the color just inside of it
    ledstrips[1][medallionRadiusPx - destinationIndex] = ledstrips[1][medallionRadiusPx - sourceIndex]; // on both side
    ledstrips[2][medallionRadiusPx + destinationIndex] = ledstrips[2][medallionRadiusPx + sourceIndex]; // each pixel get assigned to the color just inside of it
    ledstrips[2][medallionRadiusPx - destinationIndex] = ledstrips[2][medallionRadiusPx - sourceIndex]; // on both side
  }
  ledstrips[0][medallionRadiusPx] = colorFeedMedallion;
  ledstrips[1][medallionRadiusPx] = colorFeedMedallion;
  ledstrips[2][medallionRadiusPx] = colorFeedMedallion;
}

void medallionShiftInward(){
  for(int destinationIndex=0; destinationIndex<medallionRadiusPx; destinationIndex++){ // start at outside of range and move inward as colors are shifted outward
    sourceIndex = destinationIndex + 1;
    ledstrips[0][medallionRadiusPx + destinationIndex] = ledstrips[0][medallionRadiusPx + sourceIndex]; // each pixel get assigned to the color just inside of it
    ledstrips[0][medallionRadiusPx - destinationIndex] = ledstrips[0][medallionRadiusPx - sourceIndex]; // on both side
    ledstrips[1][medallionRadiusPx + destinationIndex] = ledstrips[1][medallionRadiusPx + sourceIndex]; // each pixel get assigned to the color just inside of it
    ledstrips[1][medallionRadiusPx - destinationIndex] = ledstrips[1][medallionRadiusPx - sourceIndex]; // on both side
    ledstrips[2][medallionRadiusPx + destinationIndex] = ledstrips[2][medallionRadiusPx + sourceIndex]; // each pixel get assigned to the color just inside of it
    ledstrips[2][medallionRadiusPx - destinationIndex] = ledstrips[2][medallionRadiusPx - sourceIndex]; // on both side
  }
  ledstrips[0][ 0] = colorFeedMedallion;
  ledstrips[0][74] = colorFeedMedallion;
  ledstrips[1][ 0] = colorFeedMedallion;
  ledstrips[1][74] = colorFeedMedallion;
  ledstrips[2][ 0] = colorFeedMedallion;
  ledstrips[2][74] = colorFeedMedallion;
}

void filamentShiftOutward(){
  for(int destinationIndex=filamentRadiusPx; destinationIndex>0; destinationIndex--){ // start at outside of range and move inward as colors are shifted outward
    sourceIndex = destinationIndex - 1;
    ledstrips[3][filamentRadiusPx + destinationIndex - 1] = ledstrips[3][filamentRadiusPx + sourceIndex - 1]; // each pixel get assigned to the color just inside of it
    ledstrips[3][filamentRadiusPx - destinationIndex    ] = ledstrips[3][filamentRadiusPx - sourceIndex    ]; // on both side
  }
  ledstrips[3][filamentRadiusPx - 1] = colorFeedFilament; // set the interior pixels to the fed color
  ledstrips[3][filamentRadiusPx    ] = colorFeedFilament;
}

void filamentShiftInward(){
  for(int destinationIndex=0; destinationIndex<filamentRadiusPx; destinationIndex++){ 
    sourceIndex = destinationIndex + 1;
    ledstrips[3][filamentRadiusPx + destinationIndex - 1] = ledstrips[3][filamentRadiusPx + sourceIndex - 1]; 
    ledstrips[3][filamentRadiusPx - destinationIndex    ] = ledstrips[3][filamentRadiusPx - sourceIndex    ]; 
  }
  ledstrips[3][  0] = colorFeedFilament; 
  ledstrips[3][149] = colorFeedFilament;
}

void all2stripRGB(int i, int r, int g, int b){
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

void handleColoring(){
  stepsSinceLastChange++;

  Serial.print("running program\t\t");
  Serial.print(programToRun);
  Serial.println("");

  Serial.print("steps since last change is\t");
  Serial.print(stepsSinceLastChange);
  Serial.println("");

  Serial.print("can transition is\t\t");
  Serial.print(canTransition ? "true" : "false");
  Serial.println("");

  switch (programToRun) {
    case 0:
      baselineColorBouncing();
      break;
    case 1:
      baselineColorBouncing();
      break;
    case 2:
      // alternate orange and red emanating out from the center
      if(colorChoice == 0){
        colorFeedMedallion.setRGB(85,15,0);
        colorFeedFilament.setRGB(85,15,0);
      } else if(colorChoice == 1){
        colorFeedMedallion.setRGB(100,0,0);
        colorFeedFilament.setRGB(100,0,0);
      } else {
        colorChoice = 0;
        colorFeedMedallion.setRGB(85,15,0);
        colorFeedFilament.setRGB(85,15,0);
      }

      // shift always outward
      medallionShiftOutward();
      filamentShiftOutward();

      // trigger at 100% medallion radius 
      if(stepsSinceLastChange == medallionRadiusPx){
        stepsSinceLastChange = 0;
        colorChoice = (colorChoice + 1) % 2;
        canTransition = true;
      } else {
        canTransition = false;
      }
      break;
    case 3:
      // red turquise orange turqoise
      if(colorChoice == 0){
        colorFeedMedallion.setRGB(85,15,0); // orange
        colorFeedFilament.setRGB(85,15,0);
      } else if(colorChoice == 1){
        colorFeedMedallion.setRGB(33,67,0); // greenish
        colorFeedFilament.setRGB(33,67,0);
      } else if(colorChoice == 2){
        colorFeedMedallion.setRGB(100,0,0); // red 
        colorFeedFilament.setRGB(100,0,0);
      } else if(colorChoice == 3){
        colorFeedMedallion.setRGB(0,70,30); // turqoise
        colorFeedFilament.setRGB(0,70,30);
      } else {
        colorChoice = 0;
        colorFeedMedallion.setRGB(85,15,0);
        colorFeedFilament.setRGB(85,15,0);
      }

      // shift always outward
      medallionShiftOutward();
      filamentShiftOutward();

      // trigger at 100% medallion radius 
      if(stepsSinceLastChange == medallionRadiusPx){
        stepsSinceLastChange = 0;
        colorChoice = (colorChoice + 1) % 4;
        canTransition = true;
      } else {
        canTransition = false;
      }

      break;
    case 4:
      // red turqoise orange lime red cyan orange yellow turqoise lime cyan
      if(colorChoice == 0){
        colorFeedMedallion.setRGB(100,0,0); // red
        colorFeedFilament.setRGB(100,0,0);
      } else if(colorChoice == 1){
        colorFeedMedallion.setRGB(0,30,70); // turquoise
        colorFeedFilament.setRGB(0,30,70);
      } else if(colorChoice == 2){
        colorFeedMedallion.setRGB(85,15,0); // orange 
        colorFeedFilament.setRGB(85,15,0);
      } else if(colorChoice == 3){
        colorFeedMedallion.setRGB(33,67,0); // greenish
        colorFeedFilament.setRGB(33,67,0);
      } else if(colorChoice == 4){
        colorFeedMedallion.setRGB(100,0,0); // red
        colorFeedFilament.setRGB(100,0,0);
      } else if(colorChoice == 5){
        colorFeedMedallion.setRGB(0,50,50); // cyan
        colorFeedFilament.setRGB(0,50,50);
      } else if(colorChoice == 6){
        colorFeedMedallion.setRGB(85,15,0); // orange
        colorFeedFilament.setRGB(85,15,0);
      } else if(colorChoice == 7){
        colorFeedMedallion.setRGB(45,55,0); // yellowish
        colorFeedFilament.setRGB(45,55,0);
      } else if(colorChoice == 8){
        colorFeedMedallion.setRGB(0,70,30); // turquoise
        colorFeedFilament.setRGB(0,70,30);
      } else if(colorChoice == 9){
        colorFeedMedallion.setRGB(33,67,0); // greenish
        colorFeedFilament.setRGB(33,67,0);
      } else if(colorChoice == 10){
        colorFeedMedallion.setRGB(0,50,50); // cyan
        colorFeedFilament.setRGB(0,50,50);
      }

      // shift always outward
      medallionShiftOutward();
      filamentShiftOutward();

      // trigger at 75% medallion radius 
      if(stepsSinceLastChange == medallionRadiusPx / 2){
        stepsSinceLastChange = 0;
        colorChoice = (colorChoice + 1) % 10;
        canTransition = true;
      } else {
        canTransition = false;
      }

      break;
    case 5:
      // set cyan if moving outward and turqoise if moving inward
      if(isMovingOutward){
        colorFeedMedallion.setRGB(0,50,50); // cyan
        colorFeedFilament.setRGB(0,50,50);
      } else {
        colorFeedMedallion.setRGB(0,70,30); // turqoise
        colorFeedFilament.setRGB(0,70,30);
      }

      // shift in or out depending on states
      if(        isMovingOutward && stepsSinceLastChange < medallionRadiusPx){
        medallionShiftOutward();
        filamentShiftOutward();
      } else if ( isMovingOutward && stepsSinceLastChange >= medallionRadiusPx){
        filamentShiftOutward();
      } else if (!isMovingOutward && stepsSinceLastChange < medallionRadiusPx){
        medallionShiftInward();
        filamentShiftInward();
      } else if (!isMovingOutward && stepsSinceLastChange >= medallionRadiusPx){
        filamentShiftInward();
      }

      // bounce each time it is has traveled the length of a filament radius, in either direction
      if(stepsSinceLastChange == filamentRadiusPx){
        isMovingOutward = !isMovingOutward;
        stepsSinceLastChange = 0;
        canTransition = true;
      } else {
        canTransition = false;
      }

      break;
    default:
      // do something
      break;
  }


  // }

  // CASE 0 if 0-2 fingers are touched -- breathe between two colors as we have already designed, using the toxic yellow and toxic green
  // transition to more touches emanate the red/orange from the center
  // transition to less touches emanate ColorA from center so red goes extinct
  // if 3-4 fingers are touched -- trigger 100%, trim ~30%; red and orange alternate emanating from the center but stopping at ~1/4 to ~1/3 out from the center; make the distance they travel adjustable and make it the lesser for 3 touches and greater for 4 touches; trigger radius 50-100% but trimmed to 25-35%
  // 
  // 
  // CASE 1 if 5-6 points are touched -- trigger 100%, trim ~30% on medallion and not trim on filament; same as the previous state but the emanation continues all the way out on the filament and the color rotation will be red turquise orange turqoise
  // 
  // 
  // CASE 2 if 7 points are touched -- trigger 100%, no trim
  // 
  // 
  // CASE 3 if 8 points are touched -- trigger 75%, no trim
  // 
  // 
  // CASE 4 if 9 points are touched -- emanate a ColorC that breathes between turqoise and cyan but with the action of the normal state
  
  // fingersActiveAll

  if(canTransition){ // switch the active program only if the program has indicated it is at a switching point
    programToRun = fingersActiveAll;
  }

  FastLED.show();
}

void baselineColorBouncing(){
  // set greenish if moving outward and yellow if moving inward
  if(isMovingOutward){
    colorFeedMedallion.setRGB(33,67,0); // greenish
    colorFeedFilament.setRGB(33,67,0);
  } else {
    colorFeedMedallion.setRGB(45,55,0); // yellowish
    colorFeedFilament.setRGB(45,55,0);
  }

  // shift in or out depending on states
  if(        isMovingOutward && stepsSinceLastChange < medallionRadiusPx){
    medallionShiftOutward();
    filamentShiftOutward();
  } else if ( isMovingOutward && stepsSinceLastChange >= medallionRadiusPx){
    filamentShiftOutward();
  } else if (!isMovingOutward && stepsSinceLastChange < medallionRadiusPx){
    medallionShiftInward();
    filamentShiftInward();
  } else if (!isMovingOutward && stepsSinceLastChange >= medallionRadiusPx){
    filamentShiftInward();
  }

  // bounce each time it is has traveled the length of a filament radius, in either direction
  if(stepsSinceLastChange == filamentRadiusPx){
    isMovingOutward = !isMovingOutward;
    stepsSinceLastChange = 0;
    canTransition = true;
  } else {
    canTransition = false;
  }

}

// CONSIDER THIS FROM OTABASIC TO RECONNET ON CONNECTION LOSS
// while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//   Serial.println("Connection Failed! Rebooting...");
//   delay(5000);
//   ESP.restart();
