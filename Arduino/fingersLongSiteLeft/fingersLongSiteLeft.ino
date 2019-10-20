#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <CapacitiveSensor.h>
#include <wifitechwrangler2.h>

CapacitiveSensor   cs_1_2 = CapacitiveSensor(5,4);        // D1 (GPIO5) charger to D2 (GPIO4) sensor
unsigned int localPort = 8052;      // local port to listen on

// buffers for receiving and sending data
const int packetSize = 6; // fabbbb for finger 1-9 and then four digits for the cap touch result
//char sendHIGH[] = "f11000\0";       // a string to send back // 6 chars + terminator => 7
//char sendLOW[]  = "f10000\0";
char touchHIGH[packetSize+1];
char touchLOW[packetSize+1];

const char * destination = "192.168.0.100"; // LongSiteLeft
//const char * destination = "192.168.0.101"; // LongSiteRight
//const char * destination = "192.168.0.102"; // ShortSite

int fingerNumber;
String siteName;
int long lastSendTime = 0;
int reminderTime = 200; // how long to wait to send touch state even if there has not been a change; just in case there is an occasional data drop
bool lastState = false;
bool changedState = false;

bool beingTouched = false;

WiFiUDP Udp;

void setup()                    
{
  Serial.begin(115200);
  Serial.println(""); //get out of the way of anything above
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID,PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  String macAddr = WiFi.macAddress();
  if        (macAddr == "2C:F4:32:4A:36:8D"){
    fingerNumber = 1;
    siteName = "finger1";
    char sendHIGH[] = "f11000\0";       // a string to send back // 6 chars + terminator => 7
    char sendLOW[]  = "f10000\0";
  } else if (macAddr == "2C:F4:32:4A:38:44"){
    fingerNumber = 2;
    siteName = "finger2";
    char sendHIGH[] = "f21000\0";       // a string to send back // 6 chars + terminator => 7
    char sendLOW[]  = "f20000\0";
  } else if (macAddr == "2C:F4:32:4A:37:0C"){
    fingerNumber = 3;
    siteName = "finger3";
    char sendHIGH[] = "f31000\0";       // a string to send back // 6 chars + terminator => 7
    char sendLOW[]  = "f30000\0";
  }

  setupOTA();
  
  Udp.begin(localPort); // NOTE might not be needed if only sending
}

void loop()                    
{  
  int capTouchValue =  cs_1_2.capacitiveSensor(30);

  Serial.print("the captouch value is ");
  Serial.print(capTouchValue);
  Serial.print("\t");

  if(capTouchValue > 350){
    beingTouched = true;
  } else {
    beingTouched = false;
  }

  if (beingTouched == lastState){
    changedState = false;
  } else {
    changedState = true;
  }
  lastState = beingTouched;

  if (changedState || millis() - lastSendTime > reminderTime){ // send state if it is different than it was last loop or if reminderTime has elapsed
    sendState();
    lastSendTime = millis();
  }

  ArduinoOTA.handle();
  
}

void sendState(){
  Serial.print("sending ");
  
  Udp.beginPacket(destination, localPort);
  if (beingTouched){
    Serial.print(touchHIGH);
    Udp.write((const uint8_t*)touchHIGH, packetSize+1);
  } else {
    Serial.print(touchLOW);
    Udp.write((const uint8_t*)touchLOW, packetSize+1);
  }
  Udp.endPacket();
  Serial.println("");
}

void setupOTA(){
  
  ArduinoOTA.setHostname(siteName.c_str());

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
