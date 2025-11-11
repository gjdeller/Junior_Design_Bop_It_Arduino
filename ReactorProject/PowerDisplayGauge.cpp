#include "DisplayPowerGauge.h"
#include <SPI.h>
#include "Pins.h"

static Adafruit_GC9A01A tft2(&SPI, LCDB_DC, LCDB_CS, LCDB_RST);


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
  tft2.drawLine(CX, CY, x, y, color);
  tft2.fillCircle(CX, CY, 3, GC9A01A_RED); // hub
}

void ControlRodGauge_DrawFace() {
  tft2.fillScreen(GC9A01A_WHITE);

  // rings
  tft2.drawCircle(CX, CY, R_OUT+4, 0x7BEF); // gray
  tft2.drawCircle(CX, CY, R_OUT+2, 0x39E7); // darker gray

  // ticks: minor every 5, major every 25
  for (int v = 0; v <= 100; v += 5) {
    float deg = valueToDeg(v);
    int x1,y1,x2,y2;
    bool major = (v % 25 == 0);
    polar(CX, CY, R_OUT, deg, x1, y1);
    polar(CX, CY, major ? R_IN_MAJOR : R_IN_MINOR, deg, x2, y2);
    uint16_t col = major ? GC9A01A_WHITE : 0xC618; // white / light gray
    tft2.drawLine(x1, y1, x2, y2, col);
  }

  // labels 0,25,50,75,100
  tft2.setTextColor(GC9A01A_BLACK);
  tft2.setTextSize(2);
  auto labelAt = [&](int v){
    float deg = valueToDeg(v);
    int x,y;
    polar(CX, CY, R_IN_MAJOR-16, deg, x, y);
    char buf[8]; snprintf(buf, sizeof(buf), "%d", v);
    int16_t bx, by; uint16_t bw, bh;
    tft2.getTextBounds(buf, x, y, &bx, &by, &bw, &bh);
    tft2.setCursor(x - bw/2, y - bh/2);
    tft2.print(buf);
  };
  labelAt(0); labelAt(25); labelAt(50); labelAt(75); labelAt(100);

  // title
  tft2.setTextSize(1);
  tft2.setCursor(CX - 60, 220);
  tft2.setTextColor(0xC618);
  tft2.print("Power Output: ");
}

void ControlRodGauge_Init() {
  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, HIGH);      // simple ON; (PWM dimming optional)

  SPI.begin(LCD_CLK, LCD_MISO, LCD_MOSI); // (SCLK, MISO, MOSI)
  tft2.begin(4000000);
  tft2.setRotation(0);
  delay(10);

  ControlRodGauge_DrawFace();
  lastDeg = NAN; // reset needle state
}

void ControlRodGauge_SetValue(float value) {
  float deg = valueToDeg(value);

  // erase old needle
  if (!isnanf(lastDeg)) {
    drawNeedle(lastDeg, GC9A01A_WHITE);
  }
  // draw new needle
  drawNeedle(deg, GC9A01A_RED);
  lastDeg = deg;

  // numeric readout
  tft2.fillRect(60, 150, 120, 32, GC9A01A_WHITE);
  tft2.setTextSize(3);
  tft2.setTextColor(GC9A01A_BLACK);
  tft2.setCursor(70, 154);
  char buf[16];
  snprintf(buf, sizeof(buf), "%3d", (int)roundf(value));
  tft2.print(buf);
  tft2.print(" Rod %");
}
