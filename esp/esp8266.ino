#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define DEBUG 0    // set to 0 to remove Debug Prints

#if DEBUG
#define D_SerialBegin(...) Serial.begin(__VA_ARGS__)
#define D_println(...)  Serial.println(__VA_ARGS__)
#else
#define D_SerialBegin(...)
#define D_println(...)
#endif


const char* wifi_ssid = "<wifi-ssid>";
const char* wifi_password = "<wifi-password>";

const char* pb_user = "<pocketbase-username>";
const char* pb_password = "<pocketbase-password>";
const char* pb_serverUrl = "<pocketbase-baseurl>";


Adafruit_BME280 bme;
String authToken = ""; // leave empty
String userId = ""; // leave empty

void setup() {
  D_SerialBegin(115200);
  D_println();
  connectToWifi(wifi_ssid, wifi_password);
  connectToSensor();
}

void loop() {
  float readings[3];
  readSensor(readings);

  if(loginPocketBase(pb_user, pb_password)){
    sendDataToPocketBase(readings[0], readings[1], readings[2]);
    delay(100);
  }

  ESP.deepSleep(60e6); // deep sleep 60 seconds
}

void connectToWifi(const char* ssid, const char* password){
  WiFi.begin(ssid, password);
  D_println("connecting to wifi: " + String(ssid));
  while (WiFi.status() != WL_CONNECTED) {
        D_println("connecting...");
        delay(1000);
    }
    D_println("connected to WiFi");
}

void connectToSensor(){
  if (!bme.begin(0x76)) {
    D_println(F("BME280 not found!"));
    while (1) yield();
  }
  D_println(F("BME280 initialized successfully."));
}

void readSensor(float* output){
  float temp[3];

  output[0] = bme.readTemperature();
  output[1] = bme.readHumidity();
  output[2] = bme.readPressure()/100.0;
}

bool loginPocketBase(const char* username, const char* password) {
  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    String loginUrl = String(pb_serverUrl) + "/api/collections/iot_user/auth-with-password";
    if (http.begin(client, loginUrl)) {
      // Set headers
      http.addHeader("Content-Type", "application/json");

      // Create JSON payload
      String payload = "{";
      payload += "\"identity\":\"" + String(username) + "\",";
      payload += "\"password\":\"" + String(password) + "\"";
      payload += "}";

      D_println("sending payload: " + payload);

      // Send POST request
      int httpResponseCode = http.POST(payload);

      if (httpResponseCode == 200) {
        String response = http.getString();
        D_println("login successful: " + response);

        // Extract token from response
        int tokenStart = response.indexOf("\"token\":\"") + 9;
        int tokenEnd = response.indexOf("\"", tokenStart);
        authToken = response.substring(tokenStart, tokenEnd);
        D_println("extracted token: " + authToken);

        // Extract user ID from response
        int idStart = response.indexOf("\"id\":\"") + 6;
        int idEnd = response.indexOf("\"", idStart);
        userId = response.substring(idStart, idEnd);
        D_println("extracted UserId: " + userId);

        http.end();
        return true;
      } else {
        D_println("login failed. response code: " + httpResponseCode);
        D_println(http.getString());
      }

      http.end();
    } else {
      D_println("unable to connect to pocketbase server");
    }
  } else {
    D_println("wifi not connected");
  }
  return false;
}

void sendDataToPocketBase(float temperature, float humidity, float air_pressure) {
  if (WiFi.status() == WL_CONNECTED && authToken != "") {
    WiFiClient client;
    HTTPClient http;

    String recordsUrl = String(pb_serverUrl) + "/api/collections/data/records";
    if (http.begin(client, recordsUrl)) {
      // Set headers
      http.addHeader("Content-Type", "application/json");
      http.addHeader("Authorization", "Bearer " + authToken);

      // Create JSON payload
      String payload = "{";
      payload += "\"temperature\":" + String(temperature, 2) + ",";
      payload += "\"humidity\":" + String(humidity, 2) + ",";
      payload += "\"air_pressure\":" + String(air_pressure, 2) + ",";
      payload += "\"iot_user\":\"" + String(userId) + "\"";
      payload += "}";

      D_println("sending payload: " + payload);

      // Send POST request
      int httpResponseCode = http.POST(payload);

      // Print response
      if (httpResponseCode > 0) {
        D_println("response code: " + httpResponseCode);
        String response = http.getString();
        D_println("response body: " + response);
      } else {
        D_println("error on sending POST: " + httpResponseCode);
      }

      http.end();
    } else {
      D_println("unable to connect to pocketbase server");
    }
  } else {
    if (authToken == "") {
      D_println("authorization token is missing. Login might have failed.");
    } else {
      D_println("wifi not connected");
    }
  }
}
