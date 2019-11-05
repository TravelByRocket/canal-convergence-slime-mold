#include <FastLED.h>
const int NUMSTRIPS = 4;
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
  
  handleColoring();

  ArduinoOTA.handle();
  delay(delayval);
}

////////////////////////////////////////
// CUSTOM FUNCTIONS ////////////////////
////////////////////////////////////////

void setNameOTA(){ // NOTE fix the string var handling here and combine to one function that gets passed a string
  ArduinoOTA.setHostname("longSiteLeft"); // NOTE produced an error using string var here
}

void handleGrowingShrinking(){

  //// FINGER 1-3 GROW/SHRINK COLORB 
  // The 1 and the 2+3 only vary in checking one or two filaments
  // FINGER 1 GROW/SHRINK
  if (isTouchedFinger[0] && !isFullFingerColorB[0]){ // grow ColorB if the finger is being touched and if it is not full of color B
    activeToIndexFingerColorB[0]++; // increment the index of ColorB as described above
  } else if (!isTouchedFinger[0] && !isEmptyFingerColorB[0] && isEmptyFilamentColorB[0]){ // shrink ColorB if the finger is not being touched and if the connected filament is empty of ColorB and if the finger is not empty of ColorB
    // this loop is generally applied: touchlogic && is this item empty && and is the adjacent color done moving to an extreme (this is not great language, but is a start)
    activeToIndexFingerColorB[0]--; // decrement the index of ColorB as described above
  } else {
    // do not change anything
  }
  isEmptyFingerColorB[0] = (activeToIndexFingerColorB[0] == 0); // the finger is empty of ColorB is the index is equal to 0
  isFullFingerColorB[0] = (activeToIndexFingerColorB[0] == fingerLengths[0]); // the finger is full of ColorB if the index is equal to the length of the finger (maybe add "-1")

  // FINGER 2 GROW/SHRINKG
  if (isTouchedFinger[1] && !isFullFingerColorB[1]){ // grow ColorB if the finger is being touched and if it is not full of color B
    activeToIndexFingerColorB[1]++; // increment the index of ColorB as described above
  } else if (!isTouchedFinger[1] && !isEmptyFingerColorB[1] && isEmptyFilamentColorB[1] && isEmptyFilamentColorB[2]){ // shrink ColorB if the finger is not being touched and if the connected filament is empty of ColorB and if the finger is not empty of ColorB
    // this loop is generally applied: touchlogic && is this item empty && and is the adjacent color done moving to an extreme (this is not great language, but is a start)
    activeToIndexFingerColorB[1]--; // decrement the index of ColorB as described above
  } else {
    // do not change anything
  }
  isEmptyFingerColorB[1] = (activeToIndexFingerColorB[1] == 0); // the finger is empty of ColorB is the index is equal to 0
  isFullFingerColorB[1] = (activeToIndexFingerColorB[1] == fingerLengths[1]); // the finger is full of ColorB if the index is equal to the length of the finger (maybe add "-1")

  // FINGER 3 GROW/SHRINK
  if (isTouchedFinger[2] && !isFullFingerColorB[2]){ // grow ColorB if the finger is being touched and if it is not full of color B
    activeToIndexFingerColorB[2]++; // increment the index of ColorB as described above
  } else if (!isTouchedFinger[2] && !isEmptyFingerColorB[2] && isEmptyFilamentColorB[3] && isEmptyFilamentColorB[4]){ // shrink ColorB if the finger is not being touched and if the connected filament is empty of ColorB and if the finger is not empty of ColorB
    // this loop is generally applied: touchlogic && is this item empty && and is the adjacent color done moving to an extreme (this is not great language, but is a start)
    activeToIndexFingerColorB[2]--; // decrement the index of ColorB as described above
  } else {
    // do not change anything
  }
  isEmptyFingerColorB[2] = (activeToIndexFingerColorB[2] == 0); // the finger is empty of ColorB is the index is equal to 0
  isFullFingerColorB[2] = (activeToIndexFingerColorB[2] == fingerLengths[2]); // the finger is full of ColorB if the index is equal to the length of the finger (maybe add "-1")

  // FINGER 2 and 3 GROW/SHRINK 
  // for(int i=1; i<3; i++){
  //   if (isTouchedFinger[i] && !isFullFingerColorB[i]){ // if the finger is touched and it is not full of ColorB then make it grow
  //     activeToIndexFingerColorB[i]++; // increment the index as described above
  //   } else if (!isTouchedFinger[i] && !isEmptyFingerColorB[i] && isEmptyFilamentColorB[i] && isEmptyFilamentColorB[i+1]){ // if the finger is not touched and not empty of ColorB and both connected filaments are not empty of ColorB
  //     activeToIndexFingerColorB[i]--; // decrement the index as described above
  //   } else {
  //     // do not change anything
  //   }
  //   isEmptyFingerColorB[i] = (activeToIndexFingerColorB[i] == 0); // the finger is empty of ColorB is the index is equal to zero
  //   isFullFingerColorB[i] = (activeToIndexFingerColorB[i] == fingerLengths[i]); // the finger is full if the ColorB index is equal to the length of the finger
  // }

  // Serial.print("Fingers are ColorB to indices \t");
  // for(int i=0; i<3; i++){
  //   Serial.print(activeToIndexFingerColorB[i]);
  //   Serial.print("\t");
  // }
  // Serial.println("");
  
  
  //// FILAMENTS 1-4 GROW/SHRINK COLORB

  // if(isTouchedFinger[0] && isFullFingerColorB[0] && !isFullFilamentColorB[0]){ // if the finger is being touched and the finger is full of ColorB and the filament is not full of ColorB
  //   activeToIndexFilamentColorB[0]++; // increment the index of ColorB as described above
  // } else if (!isTouchedFinger[0] && !isEmptyFilamentColorB[0] && isEmptyFilamentColorC[0]){ // if the finger is not being touched and the filament is not empty of ColorB and the filament is empty of ColorC
  //   activeToIndexFilamentColorB[0]--; // decrement the index of ColorB as described above
  // } else {
  //   // do not change anything
  // }
  // isEmptyFilamentColorB[0] = (activeToIndexFilamentColorB[0] == 0);
  // isFullFilamentColorB[0] = (activeToIndexFilamentColorB[0] == filamentLengths[0]);

  for(int j=0; j<4; j++){
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
  // for(int i=0; i<4; i++){
  //   Serial.print(activeToIndexFilamentColorB[i]);
  //   Serial.print("\t");
  // }
  // Serial.println("");

  //// Filaments 1-4 GROW/SHRINK COLORC

  // Serial.print("Filaments are ColorC to indices \t");
  // for(int i=0; i<4; i++){
  //   Serial.print(activeToIndexFilamentColorC[i]);
  //   Serial.print("\t");
  // }
  // Serial.println("");

  // Sections are (1) between fingers 1 and (2) between fingers 2 and 3
  for(int k=0; k<2; k++){
    if(isFullFilamentColorB[k * 2] && isFullFilamentColorB[k * 2 + 1] && isTouchedFinger[k] && isTouchedFinger[k + 1]){ // if adjacent filaments are both full of ColorB and if both adjacent fingers are being touched
      if(!isFullFilamentColorC[k * 2]){ // if the filament on one side is not full of ColorC then increment the position
        activeToIndexFilamentColorC[k * 2]--; // mathematical decrement here is incrementing ColorC position from end of filament toward the finger
      } 
      if (!isFullFilamentColorC[k * 2 + 1]){ // if the filament on the other side is not full of ColorC then increment the position
        activeToIndexFilamentColorC[k * 2 + 1]--; // mathematical decrement here is incrementing ColorC position from end of filament toward the finger
      }
    } else if(isEmptyFingerColorC[k] && (!isTouchedFinger[k] || !isTouchedFinger[k + 1])){ // if the finger is empty of ColorC and either finger is not being touched and the filament is not empty of ColorC
      if(!isEmptyFilamentColorC[k * 2]){
         activeToIndexFilamentColorC[k * 2]++;
      }
      if(!isEmptyFilamentColorC[k * 2 + 1]){
         activeToIndexFilamentColorC[k * 2 + 1]++;  
      }
    } else {
      // do not change anything
    }

    // flicker ColorC
    if (isFullFilamentColorB[k * 2] && isEmptyFilamentColorC[k * 2]){
      if(random(100) == 0){ // at delayval = 25 ms this triggers 160 times in 4 seconds
        activeToIndexFingerColorC[k * 2    ] -= 10;
      }
    }
    if (isFullFilamentColorB[k * 2 + 1] && isEmptyFilamentColorC[k * 2 + 1]){
      if(random(100) == 0){ // at delayval = 25 ms this triggers 160 times in 4 seconds
        activeToIndexFingerColorC[k * 2 + 1] -= 10;
      }
    }

    isEmptyFilamentColorC[k * 2]     = (activeToIndexFilamentColorC[k * 2] == filamentLengths[k * 2]);
    isEmptyFilamentColorC[k * 2 + 1] = (activeToIndexFilamentColorC[k * 2 + 1] == filamentLengths[k * 2 + 1]);
    isFullFilamentColorC[k * 2]      = (activeToIndexFilamentColorC[k * 2] == 0);
    isFullFilamentColorC[k * 2 + 1]  = (activeToIndexFilamentColorC[k * 2 + 1] == 0);
    // Serial.print("doing filament ColorC grow/shrink for k = ");
    // Serial.print(k);
    // Serial.print(" so k * 2 = ");
    // Serial.print(k * 2);
    // Serial.print(" and k * 2 + 1 is ");
    // Serial.print(k * 2 + 1);
    // Serial.println("");
  }

  //// FINGERS 1-3 GROW/SHRINK COLORC

  // finger 1 is the special case
  if (isTouchedFinger[0] && isTouchedFinger[1] && isFullFilamentColorC[0] && !isFullFingerColorC[0]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[0]--; // mathematical decrement is a visual increment for ColorC
  // } else if ((!isTouchedFinger[0] || !isTouchedFinger[1]) && !isEmptyFingerColorC[0]){ // if finger 1 or finger 2 is not touched and finger 1 is not empty of colorC then decrement the index of ColorC on finger 1
  } else if (!isEmptyFingerColorC[0]){
    activeToIndexFingerColorC[0]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[0] = activeToIndexFingerColorC[0] == fingerLengths[0];
  isFullFingerColorC[0] = activeToIndexFingerColorC[0] == 0;

  // finger 2 is not special but the code is complex here
  if (isTouchedFinger[1] && ((isTouchedFinger[0] && isFullFilamentColorC[1]) || (isTouchedFinger[2] && isFullFilamentColorC[2])) && !isFullFingerColorC[1]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[1]--; // mathematical decrement is a visual increment for ColorC
  } else if (!isEmptyFingerColorC[1]){ // if it is not growing ColorC (above) then it is shrinking COlorC
    activeToIndexFingerColorC[1]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[1] = activeToIndexFingerColorC[1] == fingerLengths[1];
  isFullFingerColorC[1] = activeToIndexFingerColorC[1] == 0;

  // finger 3 is not special but the code is complex here
  if (isTouchedFinger[2] && ((isTouchedFinger[1] && isFullFilamentColorC[3]) || (isTouchedFinger[3] && isFullFilamentColorC[4])) && !isFullFingerColorC[2]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[2]--; // mathematical decrement is a visual increment for ColorC
  } else if (!isEmptyFingerColorC[2]){ // if it is not growing ColorC (above) then it is shrinking COlorC
    activeToIndexFingerColorC[2]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[2] = activeToIndexFingerColorC[2] == fingerLengths[2];
  isFullFingerColorC[2] = activeToIndexFingerColorC[2] == 0;  

  Serial.print("Fingers are ColorC to indices \t");
  for(int i=0; i<3; i++){
    Serial.print(activeToIndexFingerColorC[i]);
    Serial.print("\t");
  }
  Serial.println("");

}

void handleColoring(){

  breatheYellowGreen(); // updates the aRed, aGre, aBlu variables that can be used below

  // // FINGERS 
  // for(int finger=0; finger<3; finger++){ // finger index
  //   for(int pixel=0; pixel<fingerLengths[finger]; pixel++){ // pixel index
  //     if (pixel > activeToIndexFingerColorB[finger] && pixel < activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorB index and less than the ColorC index then make the pixel ColorA
  //       finger2stripRGB(finger,pixel,aRed,aGre,aBlu); // make ColorA as described above
  //     } else if (pixel < activeToIndexFingerColorB[finger] && pixel < activeToIndexFingerColorC[finger]){ // if the current pixel index is less than the ColorB index then make it ColorB
  //       finger2stripRGB(finger,pixel,bRed,bGre,bBlu); // make ColorB as described above
  //     } else if (pixel > activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorC index then make it ColorC
  //       finger2stripRGB(finger,pixel,cRed,cGre,cBlu); // make ColorC as described above
  //     }
  //   }
  // }

  // FINGERS MORE EFFICIENT CODING BUT LESS READABLE
  for(int finger=0; finger<3; finger++){ // finger index
    for(int pixel=0; pixel<fingerLengths[finger]; pixel++){ // pixel index
      if (pixel >= activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorC index then make it ColorC
        finger2stripRGB(finger,pixel,cRed,cGre,cBlu); // make ColorC as described above
      } else if (pixel <= activeToIndexFingerColorB[finger]){ // if the current pixel index is less than the ColorB index then make it ColorB
        finger2stripRGB(finger,pixel,bRed,bGre,bBlu); // make ColorB as described above
      // } else if (pixel > activeToIndexFingerColorB[finger] && pixel < activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorB index and less than the ColorC index then make the pixel ColorA
      } else {
        finger2stripRGB(finger,pixel,aRed,aGre,aBlu); // make ColorA as described above
      } 
    }
  }

  // FILAMENTS 
  for(int filament=0; filament<4; filament++){ // filament index
    for(int pixel=0; pixel<filamentLengths[filament]; pixel++){ // pixel index
      // the if statement below could have been written more efficiently but it is instead writte to make more sense to a reader, going in order through colors A, B, and C
      if (pixel >= activeToIndexFilamentColorC[filament]){ // if the current pixel index is greater than the ColorC index then make it ColorC
        filament2stripRGB(filament,pixel,cRed,cGre,cBlu); // make ColorC as described above
      } else if (pixel <= activeToIndexFilamentColorB[filament]){ // if the current pixel index is less than the ColorB index then make it ColorB
        filament2stripRGB(filament,pixel,bRed,bGre,bBlu); // make ColorB as described above
      } else {
      // if (pixel > activeToIndexFilamentColorB[filament] && pixel < activeToIndexFilamentColorC[filament]){ // if the current pixel index is greater than the ColorB index and less than the ColorC index then make the pixel ColorA
        filament2stripRGB(filament,pixel,aRed,aGre,aBlu); // make ColorA as described above
      }
    }
  }

  FastLED.show();

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

void sendOutStatuses(){

  // NOTE this should only send when there are changes or for heartbeat updates but writing it here for now
  // I will temporarily limit this to run on a set interval that will hopefully not be noticed for now until I make a nicer reporting system

  // msgIsEmptyFinger3ColorB[] = "f3b000\0" send to longSiteRight when Finger 3 is not full of ColorB 
  if(!isFullFingerColorB[2]){
    Udp.beginPacket(addressLongSiteRight,localPort);
    Udp.write((const uint8_t*)msgIsEmptyFinger3ColorB, packetSize+1);
    Udp.endPacket();
    Serial.println("send msgIsEmptyFinger3ColorB");
  }
  
  // char msgIsFullFinger3ColorB[]    = "f3B000\0"; // send to longSiteRight when Finger 3 is full of ColorB 
  if(isFullFingerColorB[2]){
    Udp.beginPacket(addressLongSiteRight,localPort);
    Udp.write((const uint8_t*)msgIsFullFinger3ColorB, packetSize+1);
    Udp.endPacket();
    Serial.println("send msgIsFullFinger3ColorB");
  }

}

void startFastLED(){
  FastLED.addLeds<NEOPIXEL, 26>(ledstrips[0],150); // NOTE tried to use LEDPINS var for some reason
  FastLED.addLeds<NEOPIXEL, 25>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL, 27>(ledstrips[2],150);
  FastLED.addLeds<NEOPIXEL,  4>(ledstrips[3],150);
}