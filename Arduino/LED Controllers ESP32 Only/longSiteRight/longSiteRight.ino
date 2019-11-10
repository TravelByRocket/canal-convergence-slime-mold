#include <FastLED.h>
const int NUMSTRIPS = 3;
CRGB ledstrips[NUMSTRIPS][150];

#include "sharedTopContent.h"
#include "sharedCustomFunctions.h"

////////////////////////////////////////
// MAIN CODE START /////////////////////
////////////////////////////////////////

void setup() {
  startSerial();
  startWiFi();
  setNameOTA();
  startOTA(); // run function to enable over-the-air updating
  startFastLED();
  startUDP();
}

unsigned long lastSendTime = 0;
void loop() {
  handleIncomingUDP();
  handleGrowingShrinking();

  if(millis() - lastSendTime > 150){
    sendOutStatuses();
    lastSendTime = millis();
  }
  
  // Serial.println("going to call colorHandling()");
  handleColoring();
  // Serial.println("did call colorHandling()");

  ArduinoOTA.handle();
  delay(delayval);
}

////////////////////////////////////////
// CUSTOM FUNCTIONS ////////////////////
////////////////////////////////////////

void setNameOTA(){ // NOTE fix the string var handling here and combine to one function that gets passed a string
  ArduinoOTA.setHostname("longSiteRight"); // NOTE produced an error using string var here
}

void handleGrowingShrinking(){

  //// FINGER 1-3 GROW/SHRINK COLORB 
  // Finger 5 only has to check one filament
  // FINGER 4 GROW/SHRINK 
  if (isTouchedFinger[3] && !isFullFingerColorB[3]){ // if the finger is touched and it is not full of ColorB then make it grow
    activeToIndexFingerColorB[3]++; // increment the index as described above
  } else if (!isTouchedFinger[3] && !isEmptyFingerColorB[3] && isEmptyFilamentColorB[5] && isEmptyFilamentColorB[6]){ // if the finger is not touched and not empty of ColorB and both connected filaments are not empty of ColorB
    activeToIndexFingerColorB[3]--; // decrement the index as described above
  } else {
    // do not change anything
  }
  isEmptyFingerColorB[3] = (activeToIndexFingerColorB[3] == 0); // the finger is empty of ColorB is the index is equal to zero
  isFullFingerColorB[3] = (activeToIndexFingerColorB[3] == fingerLengths[3]); // the finger is full if the ColorB index is equal to the length of the finger

  // FINGER 5 GROW/SHRINK
  if (isTouchedFinger[4] && !isFullFingerColorB[4]){ // grow ColorB if the finger is being touched and if it is not full of color B
    activeToIndexFingerColorB[4]++; // increment the index of ColorB as described above
  } else if (!isTouchedFinger[4] && !isEmptyFingerColorB[4] && isEmptyFilamentColorB[7]){ // shrink ColorB if the finger is not being touched and if the connected filament is empty of ColorB and if the finger is not empty of ColorB
    activeToIndexFingerColorB[4]--; // decrement the index of ColorB as described above
  } else {
    // do not change anything
  }
  isEmptyFingerColorB[4] = (activeToIndexFingerColorB[4] == 0); // the finger is empty of ColorB is the index is equal to 0
  isFullFingerColorB[4] = (activeToIndexFingerColorB[4] == fingerLengths[4]); // the finger is full of ColorB if the index is equal to the length of the finger (maybe add "-1")

  // Serial.print("Fingers are ColorB to indices\t\t");
  // for(int i=3; i<5; i++){
  //   Serial.print(activeToIndexFingerColorB[i]);
  //   Serial.print("\t");
  // }
  // Serial.println("");

  //// FILAMENTS 5-8 GROW/SHRINK COLORB

  for(int j=4; j<8; j++){
    if(isTouchedFinger[fingerOfReference[j]] && isFullFingerColorB[fingerOfReference[j]] && !isFullFilamentColorB[j]){ // if the finger is being touched and the finger is full of ColorB and the filament is not full of ColorB
      activeToIndexFilamentColorB[j]++; // increment the index of ColorB as described above
    } else if (!isTouchedFinger[fingerOfReference[j]] && !isEmptyFilamentColorB[j] && isEmptyFilamentColorC[j]){ // if the finger is not being touched and the filament is not empty of ColorB and the filament is empty of ColorC
      activeToIndexFilamentColorB[j]--; // decrement the index of ColorB as described above
    } else {
      // do not change anything
    }
    isEmptyFilamentColorB[j] = (activeToIndexFilamentColorB[j] == 0);
    isFullFilamentColorB[j] = (activeToIndexFilamentColorB[j] == filamentLengths[j]);
  }
  // Serial.print("Filaments are ColorB to indices \t");
  // for(int i=4; i<8; i++){
  //   Serial.print(activeToIndexFilamentColorB[i]);
  //   Serial.print("\t");
  // }
  // Serial.println("");

  // Filaments 5-8 GROW/SHRINK COLORC
  Serial.print("Filaments are empty ColorC \t\t");
  for(int i=4; i<8; i++){
    Serial.print(isEmptyFilamentColorC[i]);
    Serial.print("\t");
  }
  Serial.println("");

  Serial.print("Filaments are full ColorC \t\t");
  for(int i=4; i<8; i++){
    Serial.print(isFullFilamentColorC[i]);
    Serial.print("\t");
  }
  Serial.println("");

  // Sections are (1) between fingers 3 and 4 and (2) between fingers 4 and 5
  for(int k=2; k<4; k++){
    if(isFullFilamentColorB[k * 2] && isFullFilamentColorB[k * 2 + 1] && isTouchedFinger[k] && isTouchedFinger[k + 1]){ // if adjacent filaments are both full of ColorB and if both adjacent fingers are being touched
      // Serial.println("in area of expanding ColorC");
      if(!isFullFilamentColorC[k * 2]){ // if the filament on one side is not full of ColorC then increment the position
        activeToIndexFilamentColorC[k * 2]--; // mathematical decrement here is incrementing ColorC position from end of filament toward the finger
        // Serial.println("now expanding ColorC for filament k * 2");
      } 
      if (!isFullFilamentColorC[k * 2 + 1]){ // if the filament one the other side if not full of ColorC then increment the position
        activeToIndexFilamentColorC[k * 2 + 1]--; // mathematical decrement here is incrementing ColorC position from end of filament toward the finger
        // Serial.println("now expanding ColorC for filament k * 2 + 1");
      }
    } else if(isEmptyFingerColorC[k] && (!isTouchedFinger[k] || !isTouchedFinger[k + 1])){ // if the finger is empty of ColorC and either finger is not being touched and the filament is not empty of ColorC
      // Serial.println("in area of pushing ColorC back to where it started");
      if(!isEmptyFilamentColorC[k * 2]){
         activeToIndexFilamentColorC[k * 2]++;
         // Serial.println("moving ColorC back for filament k * 2");
      }
      if(!isEmptyFilamentColorC[k * 2 + 1]){
         activeToIndexFilamentColorC[k * 2 + 1]++;
         // Serial.println("moving ColorC back for filament k * 2 + 1");
      }
    } else {
      // do not change anything
    }
    
    // flicker ColorC
    if (isFullFilamentColorB[k * 2] && isEmptyFilamentColorC[k * 2]){
      if(random(12) == 0){ // at delayval = 25 ms this triggers 160 times in 4 seconds
        activeToIndexFilamentColorC[k * 2    ] -= random(8, 14);;
        // Serial.print("flickering ColorC for k * 2 = ");
        // Serial.println(k * 2);
      }
    }
    if (isFullFilamentColorB[k * 2 + 1] && isEmptyFilamentColorC[k * 2 + 1]){
      if(random(12) == 0){ // at delayval = 25 ms this triggers 160 times in 4 seconds
        activeToIndexFilamentColorC[k * 2 + 1] -= random(8, 14);
        // Serial.print("flickering ColorC for k * 2 + 1 = ");
        // Serial.println(k * 2 + 1);
      }
    }

    // Serial.print("Filaments are ColorC to indices \t");
    // for(int i=0; i<4; i++){
    //   Serial.print(activeToIndexFilamentColorC[i]);
    //   Serial.print("\t");
    // }
    // Serial.println("");

    isEmptyFilamentColorC[k * 2]     = (activeToIndexFilamentColorC[k * 2    ] == filamentLengths[k * 2    ]);
    isEmptyFilamentColorC[k * 2 + 1] = (activeToIndexFilamentColorC[k * 2 + 1] == filamentLengths[k * 2 + 1]);
    isFullFilamentColorC [k * 2]     = (activeToIndexFilamentColorC[k * 2    ] == 0);
    isFullFilamentColorC [k * 2 + 1] = (activeToIndexFilamentColorC[k * 2 + 1] == 0);

  }
  Serial.print("Filaments are ColorC to indices \t");
  for(int i=4; i<8; i++){
    Serial.print(activeToIndexFilamentColorC[i]);
    Serial.print("\t");
  }
  Serial.println("");

  //// FINGERS 4-5 GROW/SHRINK COLORC

  // finger 4 is not special but the code is complex here
  if (isTouchedFinger[3] && ((isTouchedFinger[2] && isFullFilamentColorC[5]) || (isTouchedFinger[4] && isFullFilamentColorC[6])) && !isFullFingerColorC[3]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[3]--; // mathematical decrement is a visual increment for ColorC
  } else if (!isEmptyFingerColorC[3]){ // if it is not growing ColorC (above) then it is shrinking COlorC
    // NOTE this logic makes it bounce between 0 and 1 on each loop and is true for all sites as of 20191104 at 11:52:12
    activeToIndexFingerColorC[3]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[3] = activeToIndexFingerColorC[3] == fingerLengths[3];
  isFullFingerColorC[3] = activeToIndexFingerColorC[3] == 0;

  // finger 5 is the special case
  if (isTouchedFinger[3] && isTouchedFinger[4] && isFullFilamentColorC[7] && !isFullFingerColorC[4]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[4]--; // mathematical decrement is a visual increment for ColorC
  } else if (!isEmptyFingerColorC[4]){ // if finger 1 or finger 2 is not touched and finger 1 is not empty of colorC then decrement the index of ColorC on finger 1
    activeToIndexFingerColorC[4]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[4] = activeToIndexFingerColorC[4] == fingerLengths[4];
  isFullFingerColorC[4] = activeToIndexFingerColorC[4] == 0;

  Serial.print("Fingers are ColorC to indices\t\t");
    for(int i=3; i<5; i++){
      Serial.print(activeToIndexFingerColorC[i]);
      Serial.print("\t");
    }
  Serial.println("");

}

void handleColoring(){

  // Serial.println("going to call breatheYellowGreen()");
  breatheYellowGreen(); // updates the aRed, aGre, aBlu variables that can be used below
  // Serial.println("did call breatheYellowGreen()");

  // FINGERS MORE EFFICIENT CODING BUT LESS READABLE
  for(int finger=3; finger<5; finger++){ // finger index
    for(int pixel=0; pixel<fingerLengths[finger]; pixel++){ // pixel index
      if (pixel >= activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorC index then make it ColorC
        finger2stripRGB(finger,pixel,cRed,cGre,cBlu); // make ColorC as described above
      } else if (pixel < activeToIndexFingerColorB[finger]){ // if the current pixel index is less than the ColorB index then make it ColorB
        finger2stripRGB(finger,pixel,bRed,bGre,bBlu); // make ColorB as described above
      // } else if (pixel > activeToIndexFingerColorB[finger] && pixel < activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorB index and less than the ColorC index then make the pixel ColorA
      } else {
        finger2stripRGB(finger,pixel,aRed,aGre,aBlu); // make ColorA as described above
      } 
    }
  }

  // FILAMENTS 
  for(int filament=4; filament<8; filament++){ // filament index
    for(int pixel=0; pixel<filamentLengths[filament]; pixel++){ // pixel index
      // the if statement below could have been written more efficiently but it is instead writte to make more sense to a reader, going in order through colors A, B, and C
      if (pixel >= activeToIndexFilamentColorC[filament]){ // if the current pixel index is greater than the ColorC index then make it ColorC
        filament2stripRGB(filament,pixel,cRed,cGre,cBlu); // make ColorC as described above
      } else if (pixel < activeToIndexFilamentColorB[filament]){ // if the current pixel index is less than the ColorB index then make it ColorB
        filament2stripRGB(filament,pixel,bRed,bGre,bBlu); // make ColorB as described above
      } else {
      // if (pixel > activeToIndexFilamentColorB[filament] && pixel < activeToIndexFilamentColorC[filament]){ // if the current pixel index is greater than the ColorB index and less than the ColorC index then make the pixel ColorA
        filament2stripRGB(filament,pixel,aRed,aGre,aBlu); // make ColorA as described above
      }
    }
  }

  // Serial.println("going to call fastLED.show()");
  FastLED.show();
  // Serial.println("did call fastLED.show()");

}

void handleIncomingUDP(){
  int packetSize = Udp.parsePacket(); // this variable is redefined here for some reason and moving the code below to its own function destroys the delicate balance of power here
  if (packetSize) {
    // readAndStoreMessage(); // stores in packetBufer
    int n = Udp.read(packetBuffer, packetSize);
    Udp.read(packetBuffer, packetSize);
    packetBuffer[n] = 0;
    Serial.print("Contents: ");
    Serial.print(packetBuffer);
    Serial.print("... at millis()%10000 of ");
    Serial.print(millis()%10000); 
    Serial.println("");
    processTouchCommand();
    processColorCommand();
    processSystemCommand();
    processSharedStateCommand();
  }
}

unsigned long lastSendTimeTouches = 0;
int prevTouches = 0;
int currTouches = 0;
void sendOutStatuses(){

  // NOTE this should only send when there are changes or for heartbeat updates but writing it here for now
  // I will temporarily limit this to run on a set interval that will hopefully not be noticed for now until I make a nicer reporting system
  
  //// FUNCTIONS FOR RIGHTSIDE
  // char msgIsEmptyFilament5ColorB[] = "g5b000\0"; // send to longSiteLeft when Filament 5 is empty of ColorB
  if(isEmptyFilamentColorB[4]){
    Udp.beginPacket(addressLongSiteLeft,localPort);
    Udp.write((const uint8_t*)msgIsEmptyFilament5ColorB, packetSize+1);
    Udp.endPacket();
    Serial.println("send msgIsEmptyFilament5ColorB");
  }
  
  // char msgIsFullFilament5ColorB[]  = "g5B000\0"; // send to longSiteLeft when Filament 5 is not empty of ColorB
  if(!isEmptyFilamentColorB[4]){
    Udp.beginPacket(addressLongSiteLeft,localPort);
    Udp.write((const uint8_t*)msgIsFullFilament5ColorB, packetSize+1);
    Udp.endPacket();
    Serial.println("send msgIsFullFilament5ColorB");
  }

  // char msgIsEmptyFilament5ColorC[] = "g5c000\0"; // send to longSiteLeft when Filament 5 is not full of ColorC
  if(!isFullFilamentColorC[4]){
    Udp.beginPacket(addressLongSiteLeft,localPort);
    Udp.write((const uint8_t*)msgIsEmptyFilament5ColorC, packetSize+1);
    Udp.endPacket();
    Serial.println("send msgIsEmptyFilament5ColorC");
  }

  // char msgIsFullFilament5ColorC[]  = "g5C000\0"; // send to longSiteLeft when Filament 5 is full of ColorC
  if(isFullFilamentColorC[4]){
    Udp.beginPacket(addressLongSiteLeft,localPort);
    Udp.write((const uint8_t*)msgIsFullFilament5ColorC, packetSize+1);
    Udp.endPacket();
    Serial.println("send msgIsFullFilament5ColorC");
  }

  int touchCount = 0;
  for(int i=3; i<5; i++){
    if(isTouchedFinger[i]){
      touchCount++;
    }
  }

  prevTouches = currTouches;
  currTouches = touchCount;

  if(millis() - lastSendTimeTouches > 300 || prevTouches != currTouches){
    if(touchCount == 0){
      Udp.beginPacket(addressMedallion,localPort);
      Udp.write((const uint8_t*)msgLongSiteRight0Touch, packetSize+1);
      Udp.endPacket();
    } else if(touchCount == 1){
      Udp.beginPacket(addressMedallion,localPort);
      Udp.write((const uint8_t*)msgLongSiteRight1Touch, packetSize+1);
      Udp.endPacket();
    } else if(touchCount == 2){
      Udp.beginPacket(addressMedallion,localPort);
      Udp.write((const uint8_t*)msgLongSiteRight2Touch, packetSize+1);
      Udp.endPacket();
    }
    lastSendTimeTouches = millis();
  }

}

void startFastLED(){
  FastLED.addLeds<NEOPIXEL, 26>(ledstrips[0],150); // NOTE tried to use LEDPINS var for some reason
  FastLED.addLeds<NEOPIXEL, 27>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL,  4>(ledstrips[2],150);
}