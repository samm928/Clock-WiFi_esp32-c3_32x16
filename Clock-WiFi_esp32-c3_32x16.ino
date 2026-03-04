// Created using Google AI
// samm928@gmail.com
// ESP32-C3 Mini from Waveshare

#include <WiFi.h>
#include <time.h>
#include <FastLED.h>
#include <FastLED_NeoMatrix.h>
#include <Adafruit_GFX.h>
#include <Adafruit_MPU6050.h>
#include "credentials.h"

// --- Hardware Configuration ---
#define DATA_PIN     4
#define MW          32
#define MH          16
#define BRIGHTNESS  20

CRGB leds[MW * MH];

// --- Matrix Setup ---
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, MW, MH, 
  NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG);

// --- WiFi & NTP Settings ---
const char* ssid = SECRET_SSID;
const char* password =  SECRET_PASS;
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = -18000;      // Adjust for your timezone
const int   daylightOffset_sec = 3600;    

// Scrolling Variables
int scrollX = MW; 
uint8_t hue = 0;

void setup() {
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, MW * MH);

  matrix->begin();
  matrix->setTextWrap(false);
  matrix->setBrightness(30);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

void loop() {
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){ return; }

  matrix->fillScreen(0);

  // --- 1. STATIC TIME (RED) ---
  // Color alternates between Red and Amber every 2 seconds
  if ((millis() / 2000) % 2 == 0) matrix->setTextColor(matrix->Color(255, 0, 0));
  else                           matrix->setTextColor(matrix->Color(255, 150, 0));
  
  matrix->setCursor(2, 1);
  char hourStr[3], minStr[3];
  strftime(hourStr, 3, "%H", &timeinfo);
  strftime(minStr, 3, "%M", &timeinfo);
  
  matrix->print(hourStr);
  if (millis() % 1000 < 500) { matrix->print(":"); } else { matrix->print(" "); }
  matrix->print(minStr);

  // --- 2. SCROLLING DATE (BLUE) ---
  // Format: "Day, Month Date" (e.g., "Monday, March 03")
  char dateFull[40];
  strftime(dateFull, 40, "%A, %B %d", &timeinfo);

  CRGB rgbColor = CHSV(hue++, 255, 255);
  matrix->setTextColor(matrix->Color(rgbColor.r, rgbColor.g, rgbColor.b));
//  matrix->setTextColor(matrix->Color(0, 0, 255));

  matrix->setCursor(scrollX, 9);
  matrix->print(dateFull);

  // Handle Scroll Logic
  // Calculate text width: approx 6 pixels per character
  int textWidth = strlen(dateFull) * 6; 
  if (--scrollX < -textWidth) {
    scrollX = MW;
  }

  matrix->show();
  delay(50); // Controls scroll speed
}
