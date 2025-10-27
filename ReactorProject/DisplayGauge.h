#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// --- Pins
//#define LCD_MOSI 35 // SDA
//#define LCD_MISO -1 // 
//#define LCD_CLK  36 // SCK

// PINS for OLED 1
//#define LCD_CS   13 // CS
//#define LCD_DC   14 // DC
//#define LCD_RST  15 //RST
//#define BACKLIGHT_PIN 7 //BLK


// Initialize SPI + display and draw the gauge face once
void Gauge_Init();

void Gauge_DrawFace();

// This is used to update the gauge to display the current fission rate. Some issues fix later. 
void Gauge_SetValue(float value);
