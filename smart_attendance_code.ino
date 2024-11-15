#include <WiFi.h>         // WiFi library for ESP32; use <WiFiNINA.h> for other boards
#include <WiFiUdp.h>      // UDP library for network communication
#include "coap-simple.h"   // Include the CoAP library

// WiFi network credentials
const char* ssid = "your-ssid";           // Replace with your WiFi SSID
const char* password = "your-password";     // Replace with your WiFi password

// CoAP server details
const IPAddress serverIP(192,168,239,43);  // Replace with your CoAP server IP
const int serverPort = 5683;               // Default CoAP port (5683)

// Define sensor pins
const int IR_SENSOR_PIN = ;
const int PIR_SENSOR_PIN = ;

// Define LED pins
const int GREEN_LED_PIN = ;
const int RED_LED_PIN = ;

// State tracking variables
bool entryDetected = false;
bool exitDetected = false;
int attendanceCount = 0;

// Sensor state tracking variables
bool lastIrState = LOW;
bool lastPirState = LOW;

// Timing variables
unsigned long entryStartTime = 0;
unsigned long exitStartTime = 0;
const unsigned long detectionTimeout = 3000;
unsigned long lastDisplayTime = 0;
const unsigned long displayInterval = 3000;
unsigned long lastCoapUpdateTime = 0;
const unsigned long coapUpdateInterval = 300000; // Send update every 5 mins

// LED timing variables
unsigned long greenLedStartTime = 0;
unsigned long redLedStartTime = 0;
const unsigned long ledOnDuration = 3000;

// CoAP settings
WiFiUDP udp;
Coap coap(udp);
const char* coapUrl = "attendance";

void sendAttendance(int count) {
  const char* coapUrl = "attendance"; // Ensure this matches the endpoint on the server
  
  // Convert attendance count to string and store in payload array
  char payload[10];  
  snprintf(payload, sizeof(payload), "%d", count); // Convert `count` to a string

  // Use CoAP `send` method to make a `POST` request
  uint16_t messageID = coap.send(
    serverIP,                          // Server IP
    serverPort,                        // Server Port
    coapUrl,                           // Endpoint URL
    COAP_CON,                          // CoAP type (Confirmable)
    COAP_POST,                         // CoAP method (POST)
    NULL, 0,                           // No token
    (uint8_t*)payload,                 // Payload (attendance count)
    strlen(payload),                   // Payload length
    COAP_TEXT_PLAIN                    // Content format (text/plain)
  );

  if (messageID) {
    Serial.println("Sent attendance data successfully");
  } else {
    Serial.println("Failed to send attendance data");
  }
}


void setup() {
  Serial.begin(115200);
  
  // Initialize WiFi
  WiFi.begin(ssid, password);  // Use variables ssid and password
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  if (!coap.start(serverPort)) {
    Serial.println("Failed to start CoAP server!");
    return;
  }

  // Set up sensor and LED pins
  pinMode(IR_SENSOR_PIN, INPUT);
  pinMode(PIR_SENSOR_PIN, INPUT);
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  Serial.println("Attendance Counter Initialized");
}

void loop() {
  // Read sensor states
  bool irReading = !digitalRead(IR_SENSOR_PIN);
  bool pirReading = digitalRead(PIR_SENSOR_PIN);
  delay(100);  // Debounce delay
  
  unsigned long currentTime = millis();

  // Handle entry detection
  if (!entryDetected && pirReading == HIGH && irReading == LOW && lastPirState == LOW) {
    entryDetected = true;
    entryStartTime = currentTime;
    Serial.println("Entry Detected. Waiting for IR confirmation.");
  }
  
  if (entryDetected && irReading == HIGH && lastIrState == LOW) {
    attendanceCount++;
    Serial.print("Person entered. Attendance count: ");
    Serial.println(attendanceCount);

    digitalWrite(GREEN_LED_PIN, HIGH);
    greenLedStartTime = currentTime;

    entryDetected = false;
  }

  // Handle exit detection
  if (!exitDetected && irReading == HIGH && pirReading == LOW && lastIrState == LOW) {
    exitDetected = true;
    exitStartTime = currentTime;
    Serial.println("Exit Detected. Waiting for PIR confirmation.");
  }
  
  if (exitDetected && pirReading == HIGH && lastPirState == LOW) {
    if (attendanceCount > 0) {
      attendanceCount--;
      Serial.print("Person exited. Attendance count: ");
      Serial.println(attendanceCount);
    }

    digitalWrite(RED_LED_PIN, HIGH);
    redLedStartTime = currentTime;

    exitDetected = false;
  }

  // Reset entry and exit if timeout occurs
  if (entryDetected && (currentTime - entryStartTime > detectionTimeout)) {
    entryDetected = false;
    Serial.println("Entry detection timed out.");
  }

  if (exitDetected && (currentTime - exitStartTime > detectionTimeout)) {
    exitDetected = false;
    Serial.println("Exit detection timed out.");
  }

  // Turn off LEDs after the specified on-duration
  if (digitalRead(GREEN_LED_PIN) == HIGH && (currentTime - greenLedStartTime >= ledOnDuration)) {
    digitalWrite(GREEN_LED_PIN, LOW);
    Serial.println("Green LED turned off after entry.");
  }
  if (digitalRead(RED_LED_PIN) == HIGH && (currentTime - redLedStartTime >= ledOnDuration)) {
    digitalWrite(RED_LED_PIN, LOW);
    Serial.println("Red LED turned off after exit.");
  }

  // Send attendance count update to CoAP server at intervals
  if (currentTime - lastCoapUpdateTime >= coapUpdateInterval) {
    sendAttendance(attendanceCount);

    lastCoapUpdateTime = currentTime;
    Serial.print("Sent attendance count to CoAP server: ");
  }

  // Update last sensor states
  lastIrState = irReading;
  lastPirState = pirReading;
}
