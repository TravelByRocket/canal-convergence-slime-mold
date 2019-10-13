// send messages over wifi with UDP

#include <ESP8266WiFi.h>
// #include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
// ESP8266WiFiMulti wifiMulti;
// #include <bryanwifinetworks.h>
// #include <wifitechwrangler.h>
// #include <wifiucbwireless.h>
#include <wifihotspot_rocketphonexs.h>

unsigned int localPort = 8052;
WiFiUDP Udp;
char messageBuffer[UDP_TX_PACKET_MAX_SIZE + 1];
IPAddress server(172, 20, 10, 7);
// 172.20.10.7


void setup() {
  Serial.begin(115200);
  Serial.println("");
  WiFi.mode(WIFI_STA); // station mode
  WiFi.begin(SSID, PASS);
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

  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
  
  pinMode(LED_BUILTIN, OUTPUT);
  
}

int useMessageNumber = 0;
void loop() {
  
  if (useMessageNumber == 0){
    strcpy(messageBuffer,"first message");
  } else if (useMessageNumber == 1){
    strcpy(messageBuffer,"second message");
  } else {
    strcpy(messageBuffer,"third message");
  } 

  useMessageNumber = (useMessageNumber + 1) % 3;

  Serial.print("sending ");
  Serial.println(messageBuffer);
    
  Udp.beginPacket(server, localPort);
  Udp.write(messageBuffer);
  Udp.endPacket();
  
  delay(300 + (int) random(0, 10)); // add some randomness to the send time
}
