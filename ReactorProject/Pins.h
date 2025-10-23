#ifndef PINS_H
#define PINS_H
#include <Arduino.h>

// Analog Pins
const int POT_PIN = 4;

// Reactor Selection Button Inputs:
const int BTN1 = 20; // Reactor 1
const int BTN2 = 10; // Reactor 2
const int BTN3 = 11;

// LED Pins
const int GREEN_LED_PIN = 21;
const int RED_LED_PIN = 12;

// Reactor LED Pins
const int R1_LED_PIN = 5;
const int R2_LED_PIN = 6;
const int R3_LED_PIN = 1;

// OLED Screen 1: For Now lets do Fission Rate on here:
//#define LCD_MOSI 35 // OLED SDA
//#define LCD_MISO -1 // GC9A01A does not use MISO
//#define LCD_CLK 36 // OLED SCL -> SCLK
//#define LCD_CS 13 // Chip Select
//#define LCD_DC 14 // Data/Command
//#define LCD_RST 15 // Reset
//#define BACKLIGHT_PIN 7

#endif