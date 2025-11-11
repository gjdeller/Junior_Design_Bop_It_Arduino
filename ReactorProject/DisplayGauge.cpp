#include "DisplayGauge.h"
#include <SPI.h>
#include "Pins.h"

static Adafruit_GC9A01A tft(&SPI, LCD_DC, LCD_CS, LCD_RST);

static const int CX = 120;
static const int CY = 120;
static const int R_OUT = 110;
static const int R_IN_MINOR = 98;
static const int R_IN_MAJOR = 92;
static const int R_NEEDLE = 80;

// last needle angle to erase cleanly
static float lastDeg = NAN;

// ----- PHYSICAL RANGE (W) -----
static double gMin = 0.0;
static double gMax = 1e6;     // set full-scale watts here

// ===== Helpers: map physical watts to gauge percent and place ticks/labels =====
static inline float wattsToPercent(double watts) {
  if (gMax <= gMin) return 0.0f;
  double t = (watts - gMin) / (gMax - gMin);   // 0..1
  if (t < 0.0) t = 0.0;
  if (t > 1.0) t = 1.0;
  return (float)(t * 100.0);                   // 0..100
}

static float valueToDeg(float percent) {
  // percent is 0..100 → 135°..405°
  if (percent < 0)   percent = 0;
  if (percent > 100) percent = 100;
  return 135.0f + (percent / 100.0f) * 270.0f;
}

// convert degrees to screen coords (y grows down)
static void polar(int cx, int cy, int r, float deg, int &x, int &y) {
  float rad = deg * PI / 180.0f;
  float s = sinf(rad);
  float c = cosf(rad);
  x = cx + (int)roundf(r * s);
  y = cy - (int)roundf(r * c);
}

static void drawNeedle(float deg, uint16_t color) {
  int x,y;
  polar(CX, CY, R_NEEDLE, deg, x, y);
  tft.drawLine(CX, CY, x, y, color);
  tft.fillCircle(CX, CY, 3, GC9A01A_RED); // hub
}

// Draw a tick at a specific W position
static void tickAtWatts(double watts, bool major=true) {
  float p   = wattsToPercent(watts);
  float deg = valueToDeg(p);
  int x1,y1,x2,y2;
  polar(CX, CY, R_OUT, deg, x1, y1);
  polar(CX, CY, major ? R_IN_MAJOR : R_IN_MINOR, deg, x2, y2);
  uint16_t col = GC9A01A_BLACK;                // visible on white face
  tft.drawLine(x1, y1, x2, y2, col);
}

// Place a text label at a specific W position, auto-formatting units
static void labelAtWatts(double watts) {
  float p   = wattsToPercent(watts);
  float deg = valueToDeg(p);

  int x, y;
  polar(CX, CY, R_IN_MAJOR - 16, deg, x, y);

  // Build "nice" W/kW/MW text
  char buf[20];
  if (watts >= 1e6) {
    double MW = watts / 1e6;
    if (MW >= 100.0) snprintf(buf, sizeof(buf), "%.0f MW", MW);
    else             snprintf(buf, sizeof(buf), "%.1f MW", MW);
  } else if (watts >= 1e3) {
    double kW = watts / 1e3;
    if (kW >= 100.0) snprintf(buf, sizeof(buf), "%.0f kW", kW);
    else             snprintf(buf, sizeof(buf), "%.1f kW", kW);
  } else {
    snprintf(buf, sizeof(buf), "%.0f W", watts);
  }

  int16_t bx, by; uint16_t bw, bh;
  tft.getTextBounds(buf, x, y, &bx, &by, &bw, &bh);
  tft.setCursor(x - bw/2, y - bh/2);
  tft.print(buf);
}

void Gauge_DrawFace() {
  tft.fillScreen(GC9A01A_WHITE);

  // rings
  tft.drawCircle(CX, CY, R_OUT+4, 0x7BEF); // gray
  tft.drawCircle(CX, CY, R_OUT+2, 0x39E7); // darker gray

  // baseline ticks every 5% (minor) / 25% (major) on the 0..100 gauge space
  for (int v = 0; v <= 100; v += 5) {
    float deg = valueToDeg((float)v);
    int x1,y1,x2,y2;
    bool major = (v % 25 == 0);
    polar(CX, CY, R_OUT, deg, x1, y1);
    polar(CX, CY, major ? R_IN_MAJOR : R_IN_MINOR, deg, x2, y2);
    uint16_t col = major ? GC9A01A_BLACK : 0xC618; // black / light gray (visible on white)
    tft.drawLine(x1, y1, x2, y2, col);
  }

  // kW anchors (example: 0..10 kW markings regardless of gMax)
  // Adjust or add more label points as needed.
  tft.setTextColor(GC9A01A_BLACK);
  tft.setTextSize(2);

  tickAtWatts(0,      true);
  tickAtWatts(1e3,    true);
  tickAtWatts(2.5e3,  true);
  tickAtWatts(5e3,    true);
  tickAtWatts(7.5e3,  true);
  tickAtWatts(10e3,   true);

  labelAtWatts(0);
  labelAtWatts(1e3);
  labelAtWatts(2.5e3);
  labelAtWatts(5e3);
  labelAtWatts(7.5e3);
  labelAtWatts(10e3);

  // title
  tft.setTextSize(1);
  tft.setCursor(CX - 60, 220);
  tft.setTextColor(0xC618);
  tft.print("Fission Reaction Rate");
}

void Gauge_Init() {
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);      // simple ON; (PWM dimming optional)

  SPI.begin(LCD_CLK, LCD_MISO, LCD_MOSI); // (SCLK, MISO, MOSI)
  tft.begin(4000000);
  tft.setRotation(0);
  delay(10);

  Gauge_DrawFace();
  lastDeg = NAN; // reset needle state
}

void Gauge_SetValue(float value) {
  // Map raw watts to gauge percent then to degrees
  float percent = wattsToPercent((double)value);
  float deg = valueToDeg(percent);

  // erase old needle (with background color)
  if (!isnanf(lastDeg)) {
    drawNeedle(lastDeg, GC9A01A_WHITE);
  }
  // draw new needle
  drawNeedle(deg, GC9A01A_RED);
  lastDeg = deg;

  // numeric readout with auto units
  tft.fillRect(40, 150, 160, 40, GC9A01A_WHITE);

  // Format number + unit
  char numBuf[16];
  const char* unit;
  float shown;

  if (value >= 1e6f) {        // MW
    shown = value / 1e6f;
    unit = "MW";
    if (shown >= 100.0f) snprintf(numBuf, sizeof(numBuf), "%.0f", shown);
    else                 snprintf(numBuf, sizeof(numBuf), "%.1f", shown);
  } else if (value >= 1e3f) { // kW
    shown = value / 1e3f;
    unit = "kW";
    if (shown >= 100.0f) snprintf(numBuf, sizeof(numBuf), "%.0f", shown);
    else                 snprintf(numBuf, sizeof(numBuf), "%.1f", shown);
  } else {                    // W
    shown = value;
    unit = "W";
    snprintf(numBuf, sizeof(numBuf), "%.0f", shown);
  }

  // Center number and draw unit to the right
  int16_t bx, by; uint16_t bw, bh;
  tft.setTextSize(3);
  tft.setTextColor(GC9A01A_BLACK);
  tft.getTextBounds(numBuf, 0, 0, &bx, &by, &bw, &bh);

  int x = CX - (int)bw/2 - 18; // nudge left to make space for unit
  int y = 154;

  tft.setCursor(x, y);
  tft.print(numBuf);

  tft.setTextSize(2);
  tft.setCursor(x + bw + 6, y + 6);
  tft.print(unit);
}
