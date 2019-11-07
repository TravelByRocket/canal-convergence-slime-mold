// #include <FastLED.h>
#include <WiFi.h>
#include <ESPmDNS.h> // used by OTA
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <wifitechwrangler2.h>
// #include <vacawifi.h>

////////////////////////////////////////
// GEOMETRY ////////////////////////////
////////////////////////////////////////

// which LED strip index does each finger intersect
// NOTE use even numbers here to easily avoid empty or doubly-define pixels
int intersectF1onLED1 = 40;
int intersectF2onLED2 = 80;
int intersectF3onLED4 = 20;
int intersectF4onLED5 = 52;
int intersectF5onLED7 = 92;

int unusedPixelsLED3 = 28;
int unusedPixelsLED4 = 32;
int unusedPixelsLED5 = 20;
int unusedPixelsLED6 = 16;

int indexOffsetRightSiteStrips = 4; /// substract this off the right side LED indices to match diagram indices

// how long is each finger section (use 1-index counting)
int fingerLengths[] = {150 - intersectF1onLED1,
					   150 - unusedPixelsLED3,
					   150 - unusedPixelsLED4 - intersectF3onLED4,
					   150 - unusedPixelsLED6,
					   150 - intersectF5onLED7
					  };

// how long is each filament section (use 1-index counting)
int filamentLengths[] = {(intersectF1onLED1 + intersectF2onLED2)/2,
						 (intersectF1onLED1 + intersectF2onLED2)/2,
						 (150 - intersectF2onLED2 + intersectF3onLED4)/2,
						 (150 - intersectF2onLED2 + intersectF3onLED4)/2,
						 (150 - intersectF4onLED5 - unusedPixelsLED5)/2,
						 (150 - intersectF4onLED5 - unusedPixelsLED5)/2,
						 (intersectF4onLED5 + intersectF5onLED7)/2,
						 (intersectF4onLED5 + intersectF5onLED7)/2
						};

// NOTE run a FOR loop check that checks for missing pixels when dividing odd numbers or just use even numbers

////////////////////////////////////////
// GENERAL SETTINGS START //////////////
////////////////////////////////////////

bool isTouchedFinger[] = {false,false,false,false,false};
int activeToIndexFingerColorB[] = {0,0,0,0,0};
int activeToIndexFingerColorC[] = {fingerLengths[0],
								   fingerLengths[1],
								   fingerLengths[2],
								   fingerLengths[3],
								   fingerLengths[4]};
// bool fingerIsGrowingColorB[] = {false,false,false,false,false}; // NOTE I think this is obsolete based on new vars used, like isTouched
// bool fingerIsGrowingColorC[] = {false,false,false,false,false};
bool isEmptyFingerColorB[] = {true,true,true,true,true}; // comparisons could replace this throughout code but this will make it more readable
bool isEmptyFingerColorC[] = {true,true,true,true,true};
bool isFullFingerColorB[] = {false,false,false,false,false};
bool isFullFingerColorC[] = {false,false,false,false,false};

int activeToIndexFilamentColorB[] = {0,0,0,0,0,0,0,0}; // maybe this should actually go to -1 so it doesn't show 1 ColorB pixel all the time, but this could be fixesd by using the right inequality
int activeToIndexFilamentColorC[] = {filamentLengths[0],
									 filamentLengths[1],
									 filamentLengths[2],
									 filamentLengths[3],
									 filamentLengths[4],
									 filamentLengths[5],
									 filamentLengths[6],
									 filamentLengths[7]}; // ColorC uses common indices but starts hidden off the end
// bool filamentIsGrowingColorB[] = {false,false,false,false,false,false,false,false};
// bool filamentIsGrowingColorC[] = {false,false,false,false,false,false,false,false};
bool isEmptyFilamentColorB[] = {true,true,true,true,true,true,true,true};
bool isEmptyFilamentColorC[] = {true,true,true,true,true,true,true,true};
bool isFullFilamentColorB[] = {false,false,false,false,false,false,false,false};
bool isFullFilamentColorC[] = {false,false,false,false,false,false,false,false};

int fingerOfReference[] = {0,1,1,2,2,3,3,4}; // which finger index does each filament depend on? this could be done by some fancy math but I am not going to get into it

////////////////////////////////////////
// COLORS //////////////////////////////
////////////////////////////////////////

// initial 'a' color, will breath between a1 and a2
int aRed = 33;
int aGre = 67;
int aBlu = 0;

// initial 'a1' greenish color
int a1Red = 33;
int a1Gre = 67;
int a1Blu = 0;

// initial 'a2' yellowish color
int a2Red = 45;
int a2Gre = 55;
int a2Blu = 0;

// second 'b' reddish color
int bRed = 85;
int bGre = 15;
int bBlu = 0;

// third 'c' turqoise color
int cRed = 0;
int cGre = 70;
int cBlu = 30;

////////////////////////////////////////
// GENERAL SETTINGS START //////////////
////////////////////////////////////////

int breathPeriodSec = 5;
int breathPeriodMs = breathPeriodSec * 1000; // time in msec for full rotation through color
int delayval = 25; // delay between loops in ms

////////////////////////////////////////
// UDP SETUP START /////////////////////
////////////////////////////////////////

unsigned int localPort = 8052;
const int packetSize = 6; // 'fxyyyy' format is 6 plus null terminator
char packetBuffer[packetSize + 1]; //buffer to hold incoming packet
// 12 chars + terminator => 13
char msgIsEmptyFinger3ColorB[]   = "f3b000\0"; // send to longSiteRight when Finger 3 is not full of ColorB 
char msgIsFullFinger3ColorB[]    = "f3B000\0"; // send to longSiteRight when Finger 3 is full of ColorB 
char msgIsEmptyFilament5ColorB[] = "g5b000\0"; // send to longSiteLeft when Filament 5 is empty of ColorB
char msgIsFullFilament5ColorB[]  = "g5B000\0"; // send to longSiteLeft when Filament 5 is not empty of ColorB
char msgIsEmptyFilament5ColorC[] = "g5c000\0"; // send to longSiteLeft when Filament 5 is not full of ColorC
char msgIsFullFilament5ColorC[]  = "g5C000\0"; // send to longSiteLeft when Filament 5 is full of ColorC
const char * addressLongSiteLeft  = "192.168.1.100";
const char * addressLongSiteRight = "192.168.1.101";
const char * addressMedallion     = "192.168.1.103";

WiFiUDP Udp;