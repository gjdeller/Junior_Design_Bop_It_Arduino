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

// 0..100 maps to 135°..405° (total 270°)
static float valueToDeg(float v) {
  if (v < 0) v = 0;
  if (v > 100) v = 100;
  return 135.0f + (v / 100.0f) * 270.0f;
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
  tft.fillCircle(CX, CY, 3, GC9A01A_WHITE); // hub
}

void Gauge_DrawFace() {
  tft.fillScreen(GC9A01A_BLUE);

  // rings
  tft.drawCircle(CX, CY, R_OUT+4, 0x7BEF); // gray
  tft.drawCircle(CX, CY, R_OUT+2, 0x39E7); // darker gray

  // ticks: minor every 5, major every 25
  for (int v = 0; v <= 100; v += 5) {
    float deg = valueToDeg(v);
    int x1,y1,x2,y2;
    bool major = (v % 25 == 0);
    polar(CX, CY, R_OUT, deg, x1, y1);
    polar(CX, CY, major ? R_IN_MAJOR : R_IN_MINOR, deg, x2, y2);
    uint16_t col = major ? GC9A01A_WHITE : 0xC618; // white / light gray
    tft.drawLine(x1, y1, x2, y2, col);
  }

  // labels 0,25,50,75,100
  tft.setTextColor(GC9A01A_WHITE);
  tft.setTextSize(2);
  auto labelAt = [&](int v){
    float deg = valueToDeg(v);
    int x,y;
    polar(CX, CY, R_IN_MAJOR-16, deg, x, y);
    char buf[8]; snprintf(buf, sizeof(buf), "%d", v);
    int16_t bx, by; uint16_t bw, bh;
    tft.getTextBounds(buf, x, y, &bx, &by, &bw, &bh);
    tft.setCursor(x - bw/2, y - bh/2);
    tft.print(buf);
  };
  labelAt(25); labelAt(25); labelAt(50); labelAt(75); labelAt(100);

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
  tft.begin();                            // you can pass 40000000 for 40 MHz if stable
  tft.setRotation(0);

  Gauge_DrawFace();
  lastDeg = NAN; // reset needle state
}

void Gauge_SetValue(float value) {
  float deg = valueToDeg(value);

  // erase old needle
  if (!isnanf(lastDeg)) {
    drawNeedle(lastDeg, GC9A01A_BLUE);
  }
  // draw new needle
  drawNeedle(deg, GC9A01A_WHITE);
  lastDeg = deg;

  // numeric readout
  tft.fillRect(60, 150, 120, 32, GC9A01A_BLUE);
  tft.setTextSize(3);
  tft.setTextColor(GC9A01A_WHITE);
  tft.setCursor(70, 154);
  char buf[16];
  snprintf(buf, sizeof(buf), "%3d", (int)roundf(value));
  tft.print(buf);
}
