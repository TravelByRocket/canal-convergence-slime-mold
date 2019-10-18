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
#include <wifitechwrangler.h>

unsigned int localPort = 8052;      // local port to listen on

// buffers for receiving and sending data
const int packetSize = 20;
char packetBuffer[packetSize + 1]; //buffer to hold incoming packet,
char  ReplyBuffer[] = "acknowledged\0";       // a string to send back // 12 chars + terminator => 13
char  ReplyBuffer2[] = "heartbeat...\0";       // a string to send back // 12 chars + terminator => 13
const char * udpAddress = "192.168.87.61";
const char * udpAddressComp = "192.168.87.63";

WiFiUDP Udp;

void handleIncoming(){
  int packetSize = Udp.parsePacket();
  if (packetSize) {
    int n = Udp.read(packetBuffer, packetSize);
    Udp.read(packetBuffer, packetSize);
    packetBuffer[n] = 0;
    Serial.print("Contents: ");
    Serial.print(packetBuffer);
    Serial.print("... at millis()%10000 of ");
    Serial.print(millis()%10000); 
    Serial.println("");
  }
}

void handleResponse(){
//    send a reply, to the IP address and port that sent us the packet we received
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
//    Udp.beginPacket(udpAddress, localPort); NOTE -- this is closer to what I actually want to do
    Udp.write((const uint8_t*)ReplyBuffer, 12);
    Udp.endPacket();
}

int long lastSend = 0;
void handleHeartbeat(){
  if (millis() - lastSend > 1600){
    Serial.println(millis()-lastSend);
    // SEND TO OTHER ESP DEVICE
//    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.beginPacket(udpAddress, localPort);
    Udp.write((const uint8_t*)ReplyBuffer2, 12);
    Udp.endPacket();
    
    // SEND TO COMPUTER ON NETWORK
    Udp.beginPacket(udpAddressComp, localPort);
    Udp.write((const uint8_t*)ReplyBuffer2, 12);
    Udp.endPacket();
    
    lastSend = millis();
  }
}

void setup() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID.c_str(),PASS.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
}

void loop() {
  // if there's data available, read a packet

  handleIncoming();
//  handleResponse();
  handleHeartbeat();
}



/*
  test (shell/netcat):
  --------------------
	  nc -u 192.168.esp.address 8888
*/
