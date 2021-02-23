/*
  This software, the ideas and concepts is Copyright (c) David Bird 2021 and beyond.
  All rights to this software are reserved.
  It is prohibited to redistribute or reproduce of any part or all of the software contents in any form other than the following:
  1. You may print or download to a local hard disk extracts for your personal and non-commercial use only.
  2. You may copy the content to individual third parties for their personal use, but only if you acknowledge the author David Bird as the source of the material.
  3. You may not, except with my express written permission, distribute or commercially exploit the content.
  4. You may not transmit it or store it in any other website or other form of electronic retrieval system for commercial purposes.
  5. You MUST include all of this copyright and permission notice ('as annotated') and this shall be included in all copies
  or substantial portions of the software and where the software use is visible to an end-user.

  THE SOFTWARE IS PROVIDED "AS IS" FOR PRIVATE USE ONLY, IT IS NOT FOR COMMERCIAL USE IN WHOLE OR PART OR CONCEPT.

  FOR PERSONAL USE IT IS SUPPLIED WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.

  IN NO EVENT SHALL THE AUTHOR OR COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "TinyGPS++.h"
#include <SoftwareSerial.h>
TinyGPSPlus gps;

#ifdef ESP32
SoftwareSerial ss(34, 12); //tx,rx for TTGO T-BEAM
#else
SoftwareSerial ss(D1, D2); //tx,rx for Wemos D1 Mini
#endif

#include <SPI.h>
#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"

// For the ESP8266 D1 Mini use these connections
#define TFT_DC    0 // D3
#define TFT_CS   15 // D8
#define TFT_MOSI 13 // D7
#define TFT_RST   2 // D4
#define TFT_CLK  14 // D5
#define TFT_LED  3.3v
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST); // Using only hardware SPI for speed

// Assign names to common 16-bit color values:
#define   BLACK  0x0000
#define   BLUE   0x001F
#define   RED    0xF800
#define   GREEN  0x07E0
#define   CYAN   0x07FF
#define   YELLOW 0xFFE0
#define   ORANGE 0xFD20
#define   WHITE  0xFFFF

String    Time, Date;
float     NumberSats, Latitude, Longitude, Bearing;
float     AltitudeMETRES, AltitudeMILES, AltitudeKM, AltitudeFEET;
float     SpeedKPH, SpeedMPH, SpeedKNOTS, SpeedMPS;

const int centreX  = 230; // Location of the compass display on screen
const int centreY  = 120;
const int diameter = 70; // Size of the compass
int       dx = centreX, dy = centreY;
int       last_dx = centreX, last_dy = centreY - diameter * 0.85;

void setup() {
  Serial.begin(115200);
  ss.begin(9600);         // This opens up communications to the GPS
  tft.begin();            // Start the TFT display
  tft.setRotation(3);     // Rotate screen by 90°
  tft.setTextSize(2);     // Set medium text size
  tft.setTextColor(YELLOW);
  tft.fillScreen(BLUE);
}

void loop() {
  Latitude       = gps.location.lat();
  Longitude      = gps.location.lng();
  Date           = String(gps.date.day() < 10 ? "0" : "") + String(gps.date.day()) + "/" + String(gps.date.month() < 10 ? "0" : "") + String(gps.date.month()) + "/" + String(gps.date.year());
  //Date           = String(gps.date.month()<10?"0":"") + String(gps.date.month()) + "/" + String(gps.date.day()<10?"0":"") + String(gps.date.day()) + "/" + String(gps.date.year());
  Time           = String(gps.time.hour() < 10 ? "0" : "")   + String(gps.time.hour())   + ":" +
                   String(gps.time.minute() < 10 ? "0" : "") + String(gps.time.minute()) + ":" +  String(gps.time.hour() < 10 ? "0" : "") + String(gps.time.second() < 10 ? "0" : "") + String(gps.time.second());
  Bearing        = gps.course.deg();
  SpeedKPH       = gps.speed.kmph();
  SpeedMPH       = gps.speed.mph();
  SpeedKNOTS     = gps.speed.knots();
  SpeedMPS       = gps.speed.mps();
  NumberSats     = gps.satellites.value();
  AltitudeMETRES = gps.altitude.meters();
  AltitudeKM     = gps.altitude.kilometers();
  AltitudeMILES  = gps.altitude.miles();
  AltitudeFEET   = gps.altitude.feet();
  Serial.println("Time\t\tDate\t\tLAT\tLON\tSATS\tAlt\tBearing\tSpeed(KPH)");
  Serial.println("----------------------------------------------------------------------------------");
  Serial.print(Time                   + "\t");
  Serial.print(Date                   + "\t");
  Serial.print(String(Latitude, 3)    + "\t");
  Serial.print(String(Longitude, 3)   + "\t");
  Serial.print(String(NumberSats)     + "\t");
  Serial.print(String(AltitudeMETRES) + "\t"); // Select as required
  Serial.print(String(Bearing)        + "\t");
  Serial.print(String(SpeedKPH)       + "\t"); // Select as required
  //Serial.print(String(SpeedMPH)     + "\t"); // Select as required
  Serial.println("\n");
  DisplayGPSdata(NumberSats, Latitude, Longitude, AltitudeMETRES, SpeedKPH, Bearing); // Select units as required
  smartDelay(1000);
  if (millis() > 5000 && gps.charsProcessed() < 10)  Serial.println(F("No GPS data received: check wiring"));
}
//#####################################################################
void DisplayGPSdata(float dNumberSats, float dLatitude, float dLongitude, float dAltitude, float dSpeed, float dBearing) {
  PrintText(60, 0, "G6EJD GPS Compass", CYAN, 2);
  tft.fillRect(45, 40, 90, 19 * 4, BLUE);
  PrintText(0, 45, "LAT:" + String(dLatitude), YELLOW, 2);
  PrintText(0, 63, "LON:" + String(dLongitude), YELLOW, 2);
  PrintText(0, 81, "ALT:" + String(dAltitude, 1) + "M", YELLOW, 2);
  PrintText(0, 99, "SAT:" + String(dNumberSats, 0), YELLOW, 2);
  tft.fillRect(80, 220, 120, 18, BLUE);
  PrintText(10, 220, "Speed:" + String(dSpeed) + "kph", YELLOW, 2);
  tft.fillRect(240, 220, 120, 18, BLUE);
  tft.setCursor(200, 220);
  tft.print("Azi: " + Bearing_to_Ordinal(dBearing));
  Display_Compass(dBearing);
  Display_Date_Time();
}
//#####################################################################
void Display_Compass(float dBearing) {
  int dxo, dyo, dxi, dyi;
  tft.setCursor(0, 0);
  tft.drawCircle(centreX, centreY, diameter, WHITE); // Draw compass circle
  for (float i = 0; i < 360; i = i + 22.5) {
    dxo = diameter * cos((i - 90) * 3.14 / 180);
    dyo = diameter * sin((i - 90) * 3.14 / 180);
    dxi = dxo * 0.9;
    dyi = dyo * 0.9;
    tft.drawLine(dxo + centreX, dyo + centreY, dxi + centreX, dyi + centreY, WHITE);
  }
  PrintText((centreX - 5), (centreY - diameter - 18), "N", GREEN, 2);
  PrintText((centreX - 5), (centreY + diameter + 5) , "S", GREEN, 2);
  PrintText((centreX + diameter + 5),  (centreY - 5), "E", GREEN, 2);
  PrintText((centreX - diameter - 15), (centreY - 5), "W", GREEN, 2);
  dx = (0.85 * diameter * cos((dBearing - 90) * 3.14 / 180)) + centreX; // calculate X position
  dy = (0.85 * diameter * sin((dBearing - 90) * 3.14 / 180)) + centreY; // calculate Y position
  draw_arrow(last_dx, last_dy, centreX, centreY, 5, 5, BLUE);   // Erase last arrow
  draw_arrow(dx, dy, centreX, centreY, 5, 5, YELLOW);           // Draw arrow in new position
  last_dx = dx;
  last_dy = dy;
}
//#####################################################################
void draw_arrow(int x2, int y2, int x1, int y1, int alength, int awidth, int colour) {
  float distance;
  int dx, dy, x2o, y2o, x3, y3, x4, y4, k;
  distance = sqrt(pow((x1 - x2), 2) + pow((y1 - y2), 2));
  dx = x2 + (x1 - x2) * alength / distance;
  dy = y2 + (y1 - y2) * alength / distance;
  k = awidth / alength;
  x2o = x2 - dx;
  y2o = dy - y2;
  x3 = y2o * k + dx;
  y3 = x2o * k + dy;
  x4 = dx - y2o * k;
  y4 = dy - x2o * k;
  tft.drawLine(x1, y1, x2, y2, colour);
  tft.drawLine(x1, y1, dx, dy, colour);
  tft.drawLine(x3, y3, x4, y4, colour);
  tft.drawLine(x3, y3, x2, y2, colour);
  tft.drawLine(x2, y2, x4, y4, colour);
}
//#####################################################################
void Display_Date_Time() {
  PrintText(0, 150, "Date/Time:", CYAN, 2);
  tft.fillRect(0, 165, 130, 19 * 2, BLUE);
  PrintText(0, 168, Time, GREEN, 2);
  PrintText(0, 188, Date, GREEN, 2);
}
//#####################################################################
String Bearing_to_Ordinal(float bearing) {
  if (bearing >= 348.75 || bearing < 11.25)  return "N";
  if (bearing >=  11.25 && bearing < 33.75)  return "NNE";
  if (bearing >=  33.75 && bearing < 56.25)  return "NE";
  if (bearing >=  56.25 && bearing < 78.75)  return "ENE";
  if (bearing >=  78.75 && bearing < 101.25) return "E";
  if (bearing >= 101.25 && bearing < 123.75) return "ESE";
  if (bearing >= 123.75 && bearing < 146.25) return "SE";
  if (bearing >= 146.25 && bearing < 168.75) return "SSE";
  if (bearing >= 168.75 && bearing < 191.25) return "S";
  if (bearing >= 191.25 && bearing < 213.75) return "SSW";
  if (bearing >= 213.75 && bearing < 236.25) return "SW";
  if (bearing >= 236.25 && bearing < 258.75) return "WSW";
  if (bearing >= 258.75 && bearing < 281.25) return "W";
  if (bearing >= 281.25 && bearing < 303.75) return "WNW";
  if (bearing >= 303.75 && bearing < 326.25) return "NW";
  if (bearing >= 326.25 && bearing < 348.75) return "NNW";
  return "?";
}
//#####################################################################
void PrintText(int x, int y, String text, int colour, byte text_size) {
  tft.setCursor(x, y);
  tft.setTextColor(colour);
  tft.setTextSize(text_size);
  tft.print(text);
  tft.setTextColor(YELLOW); // Default colour
  tft.setTextSize(2);       // Default Text Size
}
//#####################################################################
static void smartDelay(unsigned long ms) {
  unsigned long start = millis();
  do {
    while (ss.available()) gps.encode(ss.read());
  } while (millis() - start < ms);
}
