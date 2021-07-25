#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WebSerial.h>
#include "secret.h"
#include "HX711.h"

AsyncWebServer server(80);

const char *ssid = WIFI_SSID;         // Your WiFi SSID
const char *password = WIFI_PASSWORD; // Your WiFi Password

#define DOUT 0
#define CLK 2
HX711 scale;

int cursor = 0;
int ch_f = 0;
float caliblation_factor = 0;

void recvMsg(uint8_t *data, size_t len) {
  WebSerial.println("Received Data...");
  String d = "";
  for (int i = 0; i < len; i++) {
    d += char(data[i]);
  }
  if (data[0] == 'n') {
    ch_f = 1;
    cursor++;
  } else if (data[0] == 'p') {
    ch_f = 1;
    cursor--;
  }
  WebSerial.println(d);
}

void setup() {
  delay(3000);
  Serial.begin(115200);
  Serial.println("Hello world!!");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
  // WebSerial is accessible at "<IP Address>/webserial" in browser
  WebSerial.begin(&server);
  WebSerial.msgCallback(recvMsg);
  server.begin();

  scale.begin(DOUT, CLK);
}

void loop() {
  if (ch_f == 1) {
    switch (cursor) {
    case 1:
      WebSerial.println("hello webSerial");
      ch_f = 0;
      break;

    case 2:
      WebSerial.println("hx711 reset");
      scale.set_scale();
      scale.tare();
      WebSerial.println("done. you can set load");
      ch_f = 0;
      break;

    case 3:
      WebSerial.print(scale.get_units(10));
      WebSerial.print("kg @ ");
      WebSerial.println(caliblation_factor);
      break;

    case 4:
      caliblation_factor = scale.get_units(10) / 12.0f;
      scale.set_scale(caliblation_factor);
      WebSerial.print("done. caliblation_factor is ");
      WebSerial.println(caliblation_factor);
      ch_f = 0;
      break;

    case 5:
      WebSerial.println("reset cursor");
      cursor = 0;
      ch_f = 0;
      break;

    default:
      break;
    }
  }
}