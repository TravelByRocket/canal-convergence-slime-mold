/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include <WiFiUdp.h>
// WiFi network name and password:
// const char * networkName = "your-ssid";
// const char * networkPswd = "your-password";
#include <wifihotspot_rocketphonexs.h>

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
// const char * udpAddress = "192.168.0.255";

WiFiUDP Udp;
unsigned int localPort = 8052;      // local port to listen on
char packetBuffer[30]; //buffer to hold incoming packet,
// AsyncUDP udp;

// uint8_t* currentValUINT;
// String currentValString;

void setup(){
  // Initilize hardware serial:
  Serial.begin(115200);
  WiFi.begin(SSID.c_str(), PASS.c_str());
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("Connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("Network name: ");
  Serial.println(WiFi.SSID());

}

void loop()
{
// if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
//    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
//                  packetSize,
//                  Udp.remoteIP().toString().c_str(), Udp.remotePort(),
//                  Udp.destinationIP().toString().c_str(), Udp.localPort(),
//                  ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer, 30);
    packetBuffer[n] = 0;
//    Serial.println("Contents:");
    Serial.println(packetBuffer);

  }
}