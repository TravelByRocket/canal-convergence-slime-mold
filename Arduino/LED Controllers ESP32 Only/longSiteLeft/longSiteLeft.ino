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

void loop() {
  handleIncomingUDP();
  handleGrowingShrinking();
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

  // FINGER 2 and 3 GROW/SHRINK 
  for(int i=1; i<3; i++){
    if (isTouchedFinger[i] && !isFullFingerColorB[i]){ // if the finger is touched and it is not full of ColorB then make it grow
      activeToIndexFingerColorB[i]++; // increment the index as described above
    } else if (!isTouchedFinger[i] && !isEmptyFingerColorB[i] && isEmptyFilamentColorB[i] && isEmptyFilamentColorB[i+1]){ // if the finger is not touched and not empty of ColorB and both connected filaments are not empty of ColorB
      activeToIndexFingerColorB[i]--; // decrement the index as described above
    } else {
      // do not change anything
    }
    isEmptyFingerColorB[i] = (activeToIndexFingerColorB[i] == 0); // the finger is empty of ColorB is the index is equal to zero
    isFullFingerColorB[i] = (activeToIndexFingerColorB[i] == fingerLengths[i]); // the finger is full if the ColorB index is equal to the length of the finger
  }
  
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
    } else if (!isTouchedFinger[j] && !isEmptyFilamentColorB[j] && isEmptyFilamentColorC[j]){ // if the finger is not being touched and the filament is not empty of ColorB and the filament is empty of ColorC
      activeToIndexFilamentColorB[j]--; // decrement the index of ColorB as described above
    } else {
      // do not change anything
    }
    isEmptyFilamentColorB[j] = (activeToIndexFilamentColorB[j] == 0);
    isFullFilamentColorB[j] = (activeToIndexFilamentColorB[j] == filamentLengths[j]);
  }

  //// Filaments 1-4 GROW/SHRINK COLORC
  // Sections are (1) between fingers 1 and (2) between fingers 2 and 3
  for(int k=0; k<2; k++){
    if(isFullFilamentColorB[k * 2] && isFullFilamentColorB[k * 2 + 1] && isTouchedFinger[k * 2] && isTouchedFinger[2 * k + 1]){ // if adjacent filaments are both full of ColorB and if both adjacent fingers are being touched
      if(!isFullFilamentColorC[k * 2]){ // if the filament on one side is not full of ColorC then increment the position
        activeToIndexFilamentColorC[k]--; // mathematical decrement here is incrementing ColorC position from end of filament toward the finger
      } else if (!isFullFilamentColorC[k * 2 + 1]){ // if the filament one the other side if not full of ColorC then increment the position
        activeToIndexFilamentColorC[k + 1]--; // mathematical decrement here is incrementing ColorC position from end of filament toward the finger
      }
    } else if(isEmptyFingerColorC[k * 2] && (!isTouchedFinger[k * 2] || !isTouchedFinger[k * 2 + 1]) && !isEmptyFilamentColorC[k * 2]){ // if the finger is empty of ColorC and either finger is not being touched and the filament is not empty of ColorC
      activeToIndexFilamentColorC[k * 2]++; // mathematical increment here is decrementing ColorC position back to where they meet
    } else {
      // do not change anything
    }
    isEmptyFilamentColorC[k * 2]     = (activeToIndexFilamentColorC[k * 2] == filamentLengths[k * 2]);
    isEmptyFilamentColorC[k * 2 + 1] = (activeToIndexFilamentColorC[k * 2 + 1] == filamentLengths[k * 2 + 1]);
    isFullFilamentColorC[k * 2]      = (activeToIndexFilamentColorC[k * 2] == 0);
    isFullFilamentColorC[k * 2 + 1]  = (activeToIndexFilamentColorC[k * 2 + 1] == 0);
  }

  //// FINGERS 1-3 GROW/SHRINK COLORC

  // finger 1 is the special case
  if (isTouchedFinger[0] && isTouchedFinger[1] && isFullFilamentColorC[0] && !isFullFingerColorC[0]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[0]--; // mathematical decrement is a visual increment for ColorC
  } else if ((!isTouchedFinger[0] || !isTouchedFinger[1]) && !isEmptyFingerColorC[0]){ // if finger 1 or finger 2 is not touched and finger 1 is not empty of colorC then decrement the index of ColorC on finger 1
    activeToIndexFingerColorC[0]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[0] = activeToIndexFingerColorC[0] == fingerLengths[0];
  isFullFingerColorC[0] = activeToIndexFingerColorC[0] == 0;

  // finger 2 is not special but the code is complex here
  if (isTouchedFinger[1] && ((isTouchedFinger[0] && isFullFilamentColorC[1]) || (isTouchedFinger[2] && isFullFilamentColorC[2])) && !isFullFingerColorC[1]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[1]--; // mathematical decrement is a visual increment for ColorC
  } else if (!isFullFingerColorC[1]){ // if it is not growing ColorC (above) then it is shrinking COlorC
    activeToIndexFingerColorC[1]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[1] = activeToIndexFingerColorC[1] == fingerLengths[1];
  isFullFingerColorC[1] = activeToIndexFingerColorC[1] == 0;

  // finger 3 is not special but the code is complex here
  if (isTouchedFinger[2] && ((isTouchedFinger[1] && isFullFilamentColorC[3]) || (isTouchedFinger[3] && isFullFilamentColorC[4])) && !isFullFingerColorC[2]){ // if finger 1 is touched and finger 2 is touched and filament 1 is full of ColorC and finger 1 is not full of ColorC then increment ColorC
    activeToIndexFingerColorC[2]--; // mathematical decrement is a visual increment for ColorC
  } else if (!isFullFingerColorC[2]){ // if it is not growing ColorC (above) then it is shrinking COlorC
    activeToIndexFingerColorC[2]++;
  } else {
    // do not change anything
  }
  isEmptyFingerColorC[2] = activeToIndexFingerColorC[2] == fingerLengths[2];
  isFullFingerColorC[2] = activeToIndexFingerColorC[2] == 0;  

}

void handleColoring(){

  breatheYellowGreen(); // updates the aRed, aGre, aBlu variables that can be used below

  // FINGERS 
  for(int finger=0; finger<3; finger++){ // finger index
    for(int pixel=0; pixel<fingerLengths[finger]; pixel++){ // pixel index
      // the if statement below could have been written more efficiently but it is instead writte to make more sense to a reader, going in order through colors A, B, and C
      if (pixel > activeToIndexFingerColorB[finger] && pixel < activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorB index and less than the ColorC index then make the pixel ColorA
        finger2stripRGB(finger,pixel,aRed,aGre,aBlu); // make ColorA as described above
      } else if (pixel < activeToIndexFingerColorB[finger]){ // if the current pixel index is less than the ColorB index then make it ColorB
        finger2stripRGB(finger,pixel,bRed,bGre,bBlu); // make ColorB as described above
      } else if (pixel > activeToIndexFingerColorC[finger]){ // if the current pixel index is greater than the ColorC index then make it ColorC
        finger2stripRGB(finger,pixel,cRed,cGre,cBlu); // make ColorC as described above
      }
    }
  }

  // FILAMENTS 
  for(int filament=0; filament<4; filament++){ // filament index
    for(int pixel=0; pixel<filamentLengths[filament]; pixel++){ // pixel index
      // the if statement below could have been written more efficiently but it is instead writte to make more sense to a reader, going in order through colors A, B, and C
      if (pixel > activeToIndexFilamentColorB[filament] && pixel < activeToIndexFilamentColorC[filament]){ // if the current pixel index is greater than the ColorB index and less than the ColorC index then make the pixel ColorA
        filament2stripRGB(filament,pixel,aRed,aGre,aBlu); // make ColorA as described above
      } else if (pixel < activeToIndexFilamentColorB[filament]){ // if the current pixel index is less than the ColorB index then make it ColorB
        filament2stripRGB(filament,pixel,bRed,bGre,bBlu); // make ColorB as described above
      } else if (pixel > activeToIndexFilamentColorC[filament]){ // if the current pixel index is greater than the ColorC index then make it ColorC
        filament2stripRGB(filament,pixel,cRed,cGre,cBlu); // make ColorC as described above
      }
    }
  }

  FastLED.show();

}

void handleIncomingUDP(){
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    readAndStoreMessage(); // stores in packetBufer
    processTouchCommand();
    processColorCommand();
  }
}

void sendToLongSiteRight(){
  Udp.beginPacket(addressLongSiteRight,localPort);
  // Udp.write((const uint8_t*)finger3full, 12);
  Udp.endPacket();
}

void startFastLED(){
  FastLED.addLeds<NEOPIXEL, 26>(ledstrips[0],150); // NOTE tried to use LEDPINS var for some reason
  FastLED.addLeds<NEOPIXEL, 25>(ledstrips[1],150);
  FastLED.addLeds<NEOPIXEL, 27>(ledstrips[2],150);
  FastLED.addLeds<NEOPIXEL,  4>(ledstrips[3],150);
}