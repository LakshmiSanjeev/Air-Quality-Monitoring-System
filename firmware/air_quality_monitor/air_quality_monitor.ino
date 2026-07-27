/*
 * Air Quality Monitoring System
 * -----------------------------------------------------------------------
 * Board: NodeMCU (ESP8266)
 * Sensors: DHT11 (temperature + humidity), MQ-series gas sensor (analog)
 *
 * Required libraries (install via Arduino IDE Library Manager):
 *   - DHT sensor library (Adafruit)
 *   - Adafruit Unified Sensor (dependency of the above)
 *   - ESP8266WiFi (bundled with the ESP8266 board package)
 *   - ESP8266HTTPClient (bundled with the ESP8266 board package)
 *
 * Board package: install "esp8266" via Boards Manager, select "NodeMCU 1.0 (ESP-12E Module)" as the board.
 * -----------------------------------------------------------------------
 */

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <DHT.h>

#include "config.h"

DHT dht(DHT_PIN, DHT_TYPE);

unsigned long lastSampleTime = 0;

void setup() {
  Serial.begin(115200);
  delay(100);

  dht.begin();

  if (SEND_TO_CLOUD) {
    connectToWiFi();
  } 
  else {
    Serial.println();
    Serial.println("SEND_TO_CLOUD is false — running in Serial-only mode.");
    Serial.println("Set SEND_TO_CLOUD to true in config.h once your WiFi and endpoint are set.");
  }

  Serial.println("Air Quality Monitoring System starting...");
}

void loop() {
  if (millis() - lastSampleTime >= SAMPLE_INTERVAL_MS) {
    lastSampleTime = millis();
    takeReadingAndReport();
  }
}

void connectToWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(WIFI_SSID);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startAttempt = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startAttempt < 15000) {
    delay(300);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("Connected. IP address: ");
    Serial.println(WiFi.localIP());
  } 
  else {
    Serial.println();
    Serial.println("WiFi connection failed — continuing in offline/Serial mode.");
  }
}

void takeReadingAndReport() {
  float humidity = dht.readHumidity();
  float temperatureC = dht.readTemperature();
  int mqRaw = analogRead(MQ_PIN);

  if (isnan(humidity) || isnan(temperatureC)) {
    Serial.println("Failed to read from DHT sensor — check wiring on DHT_PIN.");
    return;
  }

  const char* airQuality = classifyAirQuality(mqRaw);

  Serial.println("- RESULTS -");
  Serial.print("Temperature: "); Serial.print(temperatureC); Serial.println(" C");
  Serial.print("Humidity: "); Serial.print(humidity); Serial.println(" %");
  Serial.print("MQ raw ADC: "); Serial.println(mqRaw);
  Serial.print("Air Quality: "); Serial.println(airQuality);

  if (SEND_TO_CLOUD && WiFi.status() == WL_CONNECTED) {
    sendReadingToCloud(temperatureC, humidity, mqRaw, airQuality);
  }
}

const char* classifyAirQuality(int mqRaw) {
  if (mqRaw >= AQ_THRESHOLD_HAZARDOUS) return "Hazardous";
  if (mqRaw >= AQ_THRESHOLD_POOR) return "Poor";
  if (mqRaw >= AQ_THRESHOLD_MODERATE) return "Moderate";
  return "Good";
}

void sendReadingToCloud(float temperatureC, float humidity, int mqRaw, const char* airQuality) {
  WiFiClient client;
  HTTPClient http;

  http.begin(client, API_ENDPOINT);
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", String("Bearer ") + API_KEY);

  String payload = String("{") + "\"temperature\":" + String(temperatureC, 2) + "," + "\"humidity\":" + String(humidity, 2) + ","
  + "\"mq_raw\":" + String(mqRaw) + "," + "\"air_quality\":\"" + airQuality + "\"" + "}";

  int statusCode = http.POST(payload);

  if (statusCode > 0) {
    Serial.print("Cloud POST status: ");
    Serial.println(statusCode);
  } 
  else {
    Serial.print("Cloud POST failed: ");
    Serial.println(http.errorToString(statusCode));
  }

  http.end();
}
