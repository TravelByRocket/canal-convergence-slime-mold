// send high/low input status over wifi with UDP
// 04 Aug 2019 at 13:55 — test logic converter and show LED high/low
// ESP8266 w/ red & green : MAC 24:62:AB:0B:5C:D4, IP 192.168.87.50 reserved
// ESP8266 w/ red & blue  : MAC 84:F3:EB:EE:11:92, IP 192.168.87.51 reserved
// ESP8266 w/ red & yellow: MAC 24:62:AB:09:50:CE, IP 192.168.87.52 reserved

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <wifitechwrangler.h> // imports String vars for SSID and PASS

unsigned int localPort = 8052;      // local port to listen on
WiFiUDP Udp;
char messageBuffer[3];
IPAddress server(192, 168, 87, 52);

//int LED = 2;
int SIGNAL = 15;

void setup() {
  Serial.begin(115200);
  Serial.println("");
  WiFi.mode(WIFI_STA); // station mode
  WiFi.begin(SSID, PASS);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());
  Serial.printf("UDP server on port %d\n", localPort);
  Udp.begin(localPort);
  
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SIGNAL, INPUT);
  
}

void loop() {
  
  
//  if(digitalRead(SIGNAL) == HIGH){
  if(digitalRead(SIGNAL) == HIGH){
    digitalWrite(LED_BUILTIN, LOW); // LED is on a Pull-up so turns on with LOW
    strcpy(messageBuffer,"a1"); //one button is being pressed on unit a
  } else if (digitalRead(SIGNAL) == LOW){
    digitalWrite(LED_BUILTIN, HIGH);
    strcpy(messageBuffer,"a0"); //no button is being pressed on unit a
  }
  
  if (millis() % 2000 < 50){
    digitalWrite(LED_BUILTIN, LOW); // LED is on a Pull-up so turns on with LOW
  }

  Serial.print("sending ");
  Serial.println(messageBuffer);
    
  Udp.beginPacket(server, localPort);
  Udp.write(messageBuffer);
  Udp.endPacket();
  
  delay(25);
}
