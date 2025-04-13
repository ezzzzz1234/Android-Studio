#include <WiFi.h>
#include <HTTPClient.h>

// =======================
// WiFi Credentials
// =======================
const char* ssid = "H";
const char* password = "123456789";

// =======================
// Google Apps Script URL
// =======================
const char* googleScriptURL = "https://script.google.com/macros/s/AKfycbyZH1uKoq0HVURSnJzCqXQDSor6DEX2VCPoJJEMgc7TsmzAfobuk1IuEXvDgETPcNmQ/exec";

// =======================
// Soil Moisture Sensor Settings
// =======================
#define SOIL_MOISTURE_PIN 34
const int dryValue = 0;
const int wetValue = 4095;
const int SIGNIFICANT_CHANGE = 15;
int lastMoistureValue = -1;
String lastMoistureStatus = "";

// =======================
// PIR Motion Sensor Settings
// =======================
const int pirSensorPin = 13;
int lastPIRState = -1;

// =======================
// Setup
// =======================
void setup() {
  Serial.begin(115200);
  analogReadResolution(12);

  pinMode(pirSensorPin, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to WiFi");
  Serial.println("Sensors Initialized.");
}

// =======================
// Main Loop
// =======================
void loop() {
  // === Read Soil Moisture ===
  int rawValue = analogRead(SOIL_MOISTURE_PIN);
  int moisturePercent = map(rawValue, dryValue, wetValue, 0, 100);
  moisturePercent = constrain(moisturePercent, 0, 100);

  String moistureStatus;
  if (moisturePercent > 50) {
    moistureStatus = "Too Wet";
  } else if (moisturePercent < 20) {
    moistureStatus = "Too Dry";
  } else {
    moistureStatus = "Normal";
  }

  Serial.print("Soil Moisture: ");
  Serial.print(moisturePercent);
  Serial.print("% - Status: ");
  Serial.println(moistureStatus);

  // === Read PIR Motion Sensor ===
  int pirState = digitalRead(pirSensorPin);
  String pirStatus = (pirState == HIGH) ? "Motion Detected" : "No Motion";
  Serial.print("Motion Sensor: ");
  Serial.println(pirStatus);

  // === Determine if we need to send data ===
  bool sendMoisture = false;
  if (moistureStatus != "Normal") {
    if (lastMoistureValue < 0 || lastMoistureStatus != moistureStatus ||
        abs(moisturePercent - lastMoistureValue) >= SIGNIFICANT_CHANGE) {
      sendMoisture = true;
    }
  }

  bool sendMotion = false;
  if (pirState != lastPIRState) {
    sendMotion = true;
  }

  // === Send to Google Sheets if needed ===
  if ((sendMoisture || sendMotion) && WiFi.status() == WL_CONNECTED) {
    HTTPClient http;

    // Build parameters separately to avoid type mismatch issues
    String moistureParam = sendMoisture ? String(moisturePercent) : "";
    String moistureStatusParam = sendMoisture ? moistureStatus : "";
    String motionStatusParam = sendMotion ? pirStatus : "";

    String url = String(googleScriptURL) +
                 "?moisture=" + moistureParam +
                 "&moistureStatus=" + moistureStatusParam +
                 "&motionStatus=" + motionStatusParam;

    // URL-encode spaces (simple encoding; for production more characters may need encoding)
    url.replace(" ", "%20");

    http.begin(url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      //Serial.print("Google Sheets Response: ");
      //Serial.println(http.getString());
    } else {
      //Serial.print("HTTP Error: ");
      //Serial.println(http.errorToString(httpCode));
    }
    http.end();

    // Update the last sent values if sent successfully
    if (sendMoisture) {
      lastMoistureValue = moisturePercent;
      lastMoistureStatus = moistureStatus;
    }
    if (sendMotion) {
      lastPIRState = pirState;
    }
  }

  delay(10000); // Delay 10 seconds before the next reading
}
