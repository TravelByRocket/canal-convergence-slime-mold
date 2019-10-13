#include <WiFi.h>
#include <wifiucbwireless.h> // imports String vars for SSID and PASS

void setup() {
  Serial.begin(115200);
  while(!Serial){
    // wait for serial connection
  }
  Serial.println("");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID.c_str(), PASS.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(500);
  }

  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  Serial.print("MAC: ");
  Serial.println(WiFi.macAddress());
}

void loop() {
  
}