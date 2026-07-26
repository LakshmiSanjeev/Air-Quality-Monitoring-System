/*
 * config.h
 * -----------------------------------------------------------------------
 * Project-specific settings for the Air Quality Monitoring System.
 * Fill in your own WiFi credentials and endpoint before flashing.
 *
 * This file is listed in .gitignore-friendly form via config.example.h —
 * copy config.example.h to config.h locally and never commit real
 * credentials. (A placeholder copy is committed here for convenience;
 * replace the values before use.)
 * -----------------------------------------------------------------------
 */

#ifndef CONFIG_H
#define CONFIG_H

// ---------------------------------------------------------------------
// WiFi credentials
// ---------------------------------------------------------------------
#define WIFI_SSID     "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"

// ---------------------------------------------------------------------
// Cloud endpoint (generic HTTP JSON POST)
// Works with ThingSpeak, a custom REST API, Firebase Functions, etc.
// Leave SEND_TO_CLOUD as false to run sensor-only / Serial Monitor mode.
// ---------------------------------------------------------------------
#define SEND_TO_CLOUD   false
#define API_ENDPOINT    "http://your-server-or-cloud-endpoint/api/readings"
#define API_KEY         "your_api_key_here"

// ---------------------------------------------------------------------
// Pin mapping (NodeMCU / ESP8266, matches the wiring in images/circuit-diagram.png)
// ---------------------------------------------------------------------
#define DHT_PIN   D4    // DHT11 data pin
#define DHT_TYPE  DHT11
#define MQ_PIN    A0    // MQ-series gas sensor analog output (NodeMCU has a single ADC pin)

// ---------------------------------------------------------------------
// Sampling interval (ms)
// ---------------------------------------------------------------------
#define SAMPLE_INTERVAL_MS 5000

// ---------------------------------------------------------------------
// MQ sensor thresholds (raw ADC, 0-1023)
// These are placeholder values — calibrate against a known-clean-air
// baseline and your specific MQ variant (MQ135/MQ2/MQ9) before relying
// on them for anything beyond a class demo.
// ---------------------------------------------------------------------
#define AQ_THRESHOLD_MODERATE  300
#define AQ_THRESHOLD_POOR      500
#define AQ_THRESHOLD_HAZARDOUS 700

#endif // CONFIG_H
