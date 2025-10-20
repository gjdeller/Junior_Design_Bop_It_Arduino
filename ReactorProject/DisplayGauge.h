#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// --- Pins
#define LCD_MOSI 35
#define LCD_MISO -1
#define LCD_CLK  36
#define LCD_CS   13
#define LCD_DC   14
#define LCD_RST  15
#define BACKLIGHT_PIN 7

// Initialize SPI + display and draw the gauge face once
void Gauge_Init();

void Gauge_DrawFace();

// This is used to update the gauge to display the current fission rate. Some issues fix later. 
void Gauge_SetValue(float value);
