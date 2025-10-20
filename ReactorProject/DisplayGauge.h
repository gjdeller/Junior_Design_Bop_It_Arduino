#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// --- Pins (same as your working sketch; change if your wiring changes) ---
#define LCD_MOSI 35
#define LCD_MISO -1
#define LCD_CLK  36
#define LCD_CS   13
#define LCD_DC   14
#define LCD_RST  15
#define BACKLIGHT_PIN 7

// Initialize SPI + display and draw the gauge face once
void Gauge_Init();

// Redraw the static face (only if you need to refresh everything)
void Gauge_DrawFace();

// Update the gauge needle & numeric readout with a value in [0..100]
void Gauge_SetValue(float value);
