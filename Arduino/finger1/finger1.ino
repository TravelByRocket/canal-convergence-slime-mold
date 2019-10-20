#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>

#include <ArduinoOTA.h>
#include <CapacitiveSensor.h>
//#include <ESP8266WiFiMulti.h>
//ESP8266WiFiMulti wifiMulti; // keep above bryanwifinetworks.h
//#include <bryanwifinetworks.h>
#include <wifitechwrangler2.h>

CapacitiveSensor   cs_1_2 = CapacitiveSensor(5,4);        // D1 (GPIO5) charger to D2 (GPIO4) sensor

unsigned int localPort = 8052;      // local port to listen on

// buffers for receiving and sending data
const int packetSize = 6; // f10000 for finger 1-9 and then four digitts for the cap touch result
char sendHIGH[] = "f11000\0";       // a string to send back // 6 chars + terminator => 7
char sendLOW[]  = "f10000\0";
const char * addressLongSiteLeft = "192.168.0.100";


WiFiUDP Udp;

void handleSendHIGH(){
  Serial.print("sending ");
  Serial.print(sendHIGH);
  Serial.println("");
  Udp.beginPacket(addressLongSiteLeft, localPort);
  Udp.write((const uint8_t*)sendHIGH, packetSize+1);
  Udp.endPacket();
}

void handleSendLOW(){
  Serial.print("sending ");
  Serial.print(sendLOW);
  Serial.println("");
  Udp.beginPacket(addressLongSiteLeft, localPort);
  Udp.write((const uint8_t*)sendLOW, packetSize+1);
  Udp.endPacket();
}

void setupOTA(){
  ArduinoOTA.setHostname("finger1");

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
  
//  setupWiFiMulti();

  setupOTA();
  
  Udp.begin(localPort);
}

void loop()                    
{
  ArduinoOTA.handle();
  
  int total1 =  cs_1_2.capacitiveSensor(30);

  Serial.print("the captouch value is ");
  Serial.print(total1);
  Serial.print("\t");

  if(total1 > 350){
    handleSendHIGH();
  } else {
    handleSendLOW();
  }

  delay(100 + random(20)); // arbitrary delay
}
