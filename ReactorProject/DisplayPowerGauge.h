#pragma once
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_GC9A01A.h>

// --- Pins
#define LCD_MOSI 35 // SDA
#define LCD_MISO -1 // 
#define LCD_CLK  36 // SCK

// PINS FOR OLED 2
#define LCDB_CS 16 // CS for OLED 2
#define LCDB_DC 17 // DC for OLED 2
#define LCDB_RST 18 // RST for OLED 2
#define BACKLIGHT_PIN 7// BLK for OLED


// Initialize SPI + display and draw the gauge face once
void PowerGauge_Init();

void PowerGauge_DrawFace();

// This is used to update the gauge to display the current fission rate. Some issues fix later. 
void PowerGauge_SetValue(float value);
