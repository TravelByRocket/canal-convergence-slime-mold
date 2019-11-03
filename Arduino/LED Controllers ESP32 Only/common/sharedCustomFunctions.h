////////////////////////////////////////
// COMMON OTA CODE /////////////////////
////////////////////////////////////////

void startOTA(){
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

////////////////////////////////////////
// START SERIAL ////////////////////////
////////////////////////////////////////

void startSerial(){
	Serial.begin(115200);
	Serial.println(""); //get out of the way of the line of repeating `?`
}

////////////////////////////////////////
// START WIFI //////////////////////////
////////////////////////////////////////

void startWiFi(){
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
}

////////////////////////////////////////
// START UDP ///////////////////////////
////////////////////////////////////////

void startUDP(){
	Udp.begin(localPort);
}

////////////////////////////////////////
// READ AND STORE MESSAGE //////////////
////////////////////////////////////////

void readAndStoreMessage(){
	int n = Udp.read(packetBuffer, packetSize);
    Udp.read(packetBuffer, packetSize);
    packetBuffer[n] = 0;
    Serial.print("Contents: ");
    Serial.print(packetBuffer);
    Serial.print("... at millis()%10000 of ");
    Serial.print(millis()%10000); 
    Serial.println("");
}

////////////////////////////////////////////////
// CONVERT FUNCTIONAL INDICES TO LED INDICES ///
////////////////////////////////////////////////

// CRGB ledstrips[NUMSTRIPS][150];

void finger2stripRGB(int finger, int i, int r, int g, int b){ //NOTE this name is not good because it also sets color
	switch(finger){
		// the fingers controlled by longSiteLeft
		case 0: // Finger 1
			ledstrips[0][149 - i].setRGB(r,g,b); // 
			break;
		case 1: // Finger 2
			ledstrips[2][149 - unusedPixelsLED3 - i].setRGB(r,g,b); // 
			break;
		case 2: // Finger 3
			ledstrips[3][149 - unusedPixelsLED4 - i].setRGB(r,g,b); // 
			break;
		// the filaments controlled by longSiteRight
		case 3: // Finger 4
			ledstrips[5 - indexOffsetRightSiteStrips][149 - unusedPixelsLED6 - i].setRGB(r,g,b); // 
			break;
		case 4: // Finger 5
			ledstrips[6 - indexOffsetRightSiteStrips][149 - i].setRGB(r,g,b); // 
			break;
		default:
			Serial.print("something went wrong in finger2stripRGB");
			break;
	}
}

void filament2stripRGB(int filament, int i, int r, int g, int b){
	switch(filament){
		// the filaments controlled by longSiteLeft
		case 0:
			if(i < intersectF1onLED1){
				ledstrips[0][intersectF1onLED1 - i].setRGB(r,g,b); // 
			} else {
				ledstrips[1][i - intersectF1onLED1].setRGB(r,g,b); // 
			}
			break;
		case 1:
			if(i < 149 - intersectF2onLED2){
				ledstrips[1][149 - intersectF2onLED2 - i].setRGB(r,g,b); // 
			} else {
				ledstrips[0][i - (149 - intersectF2onLED2)].setRGB(r,g,b); // 
			}
			break;
		case 2:
			if(i < 149 - intersectF2onLED2){
				ledstrips[1][i + intersectF2onLED2].setRGB(r,g,b); // 
			} else {
				ledstrips[3][i - intersectF2onLED2].setRGB(r,g,b); // 
			}
			break;
		case 3:
			if(i < intersectF3onLED4){
				ledstrips[3][intersectF3onLED4 - i].setRGB(r,g,b); // 
			} else {
				ledstrips[1][149 - intersectF3onLED4 - i].setRGB(r,g,b); // 
			}
			break;
		// the filaments controlled by longSiteRight
		case 4:
			ledstrips[4 - indexOffsetRightSiteStrips][149 - i].setRGB(r,g,b);
			break;
		case 5:
			ledstrips[4 - indexOffsetRightSiteStrips][intersectF4onLED5 + i].setRGB(r,g,b);
			break;
		case 6:
			if(i < intersectF4onLED5){
				ledstrips[4 - indexOffsetRightSiteStrips][intersectF4onLED5 - i].setRGB(r,g,b);
			} else {
				ledstrips[6 - indexOffsetRightSiteStrips][i - intersectF4onLED5].setRGB(r,g,b);
			}
			break;
		case 7:
			if(i < intersectF5onLED7){
				ledstrips[6 - indexOffsetRightSiteStrips][intersectF5onLED7 - i].setRGB(r,g,b);
			} else {
				ledstrips[4 - indexOffsetRightSiteStrips][i - intersectF5onLED7].setRGB(r,g,b);
			}
			break;
		default:
			Serial.print("something went wrong in filament2stripRGB");
			break;
	}
}

void breatheYellowGreen(){
	int breathProgressPercent;
	if (millis() < breathPeriodMs){
		breathProgressPercent = 100 * (                 (millis() % breathPeriodMs) / (breathPeriodMs / 2));
	} else {
		breathProgressPercent = 100 * (breathPeriodMs - (millis() % breathPeriodMs) / (breathPeriodMs / 2));
	}

	aRed = (a1Red * breathProgressPercent) / 100 + (a2Red * (100 - breathProgressPercent) / 100);
	aGre = (a1Gre * breathProgressPercent) / 100 + (a2Gre * (100 - breathProgressPercent) / 100);
	aBlu = (a1Blu * breathProgressPercent) / 100 + (a2Blu * (100 - breathProgressPercent) / 100);
}

void processTouchCommand(){
    // strings from finger 1
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '1'){ // if it is finger 1
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        isTouchedFinger[0] = true;
      } else if (packetBuffer[2] == '0'){
        isTouchedFinger[0] = false;
      }
    }

    // strings from finger 2
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '2'){ // if it is finger 2
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        isTouchedFinger[1] = true;
      } else if (packetBuffer[2] == '0'){
        isTouchedFinger[1] = false;
      }
    }

    // strings from finger 3
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '3'){ // if it is finger 3
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        isTouchedFinger[2] = true;
      } else if (packetBuffer[2] == '0'){
        isTouchedFinger[2] = false;
      }
    }

    // strings from finger 4
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '4'){ // if it is finger 3
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        isTouchedFinger[3] = true;
      } else if (packetBuffer[2] == '0'){
        isTouchedFinger[3] = false;
      }
    }

    // strings from finger 5
    if(packetBuffer[0] == 'f' && packetBuffer[1] == '5'){ // if it is finger 3
      if(packetBuffer[2] == '1'){ // 1 in the thousands place if touch is sensed; this is a workaround for issues with sending continuous number values
        isTouchedFinger[4] = true;
      } else if (packetBuffer[2] == '0'){
        isTouchedFinger[4] = false;
      }
    }
}

void processColorCommand(){
    if(packetBuffer[0] == 'c'){ // c for color
		int colorVal = (int(packetBuffer[3] - '0') * 100) 
		       + (int(packetBuffer[4] - '0') * 10) 
		       + (int(packetBuffer[5] - '0')); // convert characters to a 0-255 integer
		if(packetBuffer[1] == 'a' && colorVal >= 0 && colorVal <= 255){ // if for initial GREENish color 'a'
			if       (packetBuffer[2] == 'r'){a1Red = colorVal;
			} else if(packetBuffer[2] == 'g'){a1Gre = colorVal;
			} else if(packetBuffer[2] == 'b'){a1Blu = colorVal;
			}
		}
		if(packetBuffer[1] == 'A' && colorVal >= 0 && colorVal <= 255){ // if for initial YELLOWish color 'a'
			if       (packetBuffer[2] == 'r'){a2Red = colorVal;
			} else if(packetBuffer[2] == 'g'){a2Gre = colorVal;
			} else if(packetBuffer[2] == 'b'){a2Blu = colorVal;
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

void processSystemCommand(){
	if(packetBuffer[0] == '0' && packetBuffer[1] == '0' && packetBuffer[2] == '0'){
		ESP.restart();
	}
}

////////////////////////////////////////
// AUTO WIFI RECONNECT /////////////////
////////////////////////////////////////

// CONSIDER THIS FROM OTABASIC TO RECONNET ON CONNECTION LOSS
// while (WiFi.waitForConnectResult() != WL_CONNECTED) {
//   Serial.println("Connection Failed! Rebooting...");
//   delay(5000);
//   ESP.restart();
