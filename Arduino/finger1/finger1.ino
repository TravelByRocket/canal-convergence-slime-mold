
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//#include <wifitechwrangler.h>
//#include <wifiucbwireless.h>
#include <wifihotspot_rocketphonexs.h>
#include <CapacitiveSensor.h>

CapacitiveSensor   cs_1_2 = CapacitiveSensor(5,4);        // D1 (GPIO5) charger to D2 (GPIO4) sensor

unsigned int localPort = 8052;      // local port to listen on

// buffers for receiving and sending data
const int packetSize = 6; // f10000 for finger 1-9 and then four digitts for the cap touch result
//char packetBuffer[packetSize + 1]; //buffer to hold incoming packet,
//char packetBuffer[] = "f10000\0";
char sendHIGH[] = "f11000\0";       // a string to send back // 6 chars + terminator => 7
char sendLOW[]  = "f10000\0";
const char * udpAddress = "192.168.87.61";

WiFiUDP Udp;

void handleSend(char msg[]){
  Serial.print("sending ");
  Serial.print(msg);
  Serial.println("");
  Udp.beginPacket(udpAddress, localPort);
  Udp.write((const uint8_t*)msg, packetSize);
  Udp.endPacket();
}

void setup()                    
{
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID,PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
}

void loop()                    
{
  int total1 =  cs_1_2.capacitiveSensor(30);

  Serial.print("the captouch value is ");
  Serial.print(total1);
  Serial.print("\t");

  if(total1 > 350){
    handleSend(sendHIGH);
  } else {
    handleSend(sendLOW);
  }

  
  delay(200 + random(100)); // arbitrary delay
}
