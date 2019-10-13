/*
  UDPSendReceive.pde:
  This sketch receives UDP message strings, prints them to the serial port
  and sends an "acknowledge" string back to the sender

  A Processing sketch is included at the end of file that can be used to send
  and received messages for testing with a computer.

  created 21 Aug 2010
  by Michael Margolis

  This code is in the public domain.

  adapted from Ethernet library examples
*/


#include <WiFi.h>
#include <WiFiUdp.h>
#include <bryanwifinetworks.h>

unsigned int localPort = 8052;      // local port to listen on

// buffers for receiving and sending data
char packetBuffer[3]; //buffer to hold incoming packet,

WiFiUDP Udp;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  WiFi.mode(WIFI_STA);
  setupWiFiMulti();

  Udp.begin(localPort);

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // if there's data available, read a packet
  int packetSize = Udp.parsePacket();
  if (packetSize) {
//    Serial.printf("Received packet of size %d from %s:%d\n    (to %s:%d, free heap = %d B)\n",
//                  packetSize,
//                  Udp.remoteIP().toString().c_str(), Udp.remotePort(),
//                  Udp.destinationIP().toString().c_str(), Udp.localPort(),
//                  ESP.getFreeHeap());

    // read the packet into packetBufffer
    int n = Udp.read(packetBuffer,3);
    packetBuffer[n] = 0;
//    Serial.println("Contents:");
    Serial.println(packetBuffer);

  }

//  String mess = packetBuffer.ToString
  if(packetBuffer[1] == '1' || millis() % 3000 < 50){
    digitalWrite(LED_BUILTIN, HIGH); // LED is on a Pull-up so turns on with LOW
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

}

/*
  test (shell/netcat):
  --------------------
	  nc -u 192.168.esp.address 8888
*/
