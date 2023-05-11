#include <ArduinoHttpClient.h>
#include "certs.h"
#include <Arduino_JSON.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

const char* ssid = "P110B";
const char* password = "tumotdenchin";

//Your Domain name with URL path or IP address with path
// String serverName = "https://garbagecanserver.onrender.com";
String serverName = "http://192.168.1.21:3000";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
int text;

void sendData(bool state);
typedef struct {
  bool full_state;
} Payload;
void setup() {
  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
  delay(500);
  Serial.print('r');
}

void loop() {
  // sCmd.readSerial();
  if (Serial.available() > 0) {
    // đọc chữ liệu
    text = Serial.read();

    if (char(text) == 'f') {
      Serial.println("da send thanh cong trang thai day");
      sendData(true, serverName + "/smartcan?id=640d3b665c50b0cfb0a0cd3c");
    }
    if (char(text) == 'e') {
      Serial.println("da send thanh cong trang thai trong");
      sendData(false, serverName + "/smartcan?id=640d3b665c50b0cfb0a0cd3c");
    }
  }
}

void sendData(bool state, String domain) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;
    String serverPath = domain;
    // client.setFingerprint(fingerprint_sni_cloudflaressl_com);
    // Your Domain name with URL path or IP address with path
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.setRedirectLimit(100);

    http.begin(client, serverPath.c_str());
    http.addHeader("Content-Type", "application/json; charset=utf-8");
    http.addHeader("Accept", "*/*");
    http.addHeader("Connection", "keep-alive");
    // If you need Node-RED/server authentication, insert user and password below
    //http.setAuthorization("REPLACE_WITH_SERVER_USERNAME", "REPLACE_WITH_SERVER_PASSWORD");

    String data = state ? "{\"full_state\":true}" : "{\"full_state\":false}";

    // Send HTTP GET request
    int httpResponseCode = http.PUT(data);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      if (httpResponseCode == 302 || httpResponseCode == 307) {
        Serial.print("location: ");
        Serial.println(http.getLocation());
        sendData(state, http.getLocation());
      }
      String payload = http.getString();
      Serial.println(payload);
    } else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      Serial.println(http.getLocation());
      String l = "location";

      if (httpResponseCode == 302 || httpResponseCode == 307) {
        // http.responseStatusCode();
        // Serial.println(http.headers());
        // for(int i = 0;i<http.headers();i++){
        //   Serial.println(http.headerName(i));
        // }
      }
    }
    // Free resources
    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}