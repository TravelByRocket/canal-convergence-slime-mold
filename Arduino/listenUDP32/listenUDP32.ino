/*
 *  This sketch sends random data over UDP on a ESP32 device
 *
 */
#include <WiFi.h>
#include "AsyncUDP.h"
// WiFi network name and password:
// const char * networkName = "your-ssid";
// const char * networkPswd = "your-password";
#include <wifihotspot_rocketphonexs.h>

//IP address to send UDP data to:
// either use the ip address of the server or 
// a network broadcast address
// const char * udpAddress = "192.168.0.255";
const int udpPort = 8052;

AsyncUDP udp;

uint8_t* currentValUINT;
String currentValString;

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

  if(udp.listen(udpPort)) {
    Serial.print("UDP Listening on IP: ");
    Serial.println(WiFi.localIP());
    udp.onPacket([](AsyncUDPPacket packet) {
        Serial.write(packet.data(), packet.length());
        Serial.println();
        // currentVal = toCharArray
        // byte byteArray[packet.length()];
        // strcpy((char *)byteArray,packet.data());
        // String myString = String((char *)byteArray);

        String testString = packet.data();
        // currentValUINT = packet.data();
        // currentValString = String((char *)currentValUINT);
        // currentVal = strcpy(currentVal,packet.data().c_str());
        // Serial.println("the board got "+currentValString);
        Serial.println("the board got "+myString);
    });
  }

}

void loop()
{
    delay(1000);
    //Send broadcast
    udp.broadcast("Anyone here?");
}