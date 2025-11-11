#ifndef PINS_H
#define PINS_H
#include <Arduino.h>

// Potentiometer Pins
const int POT_PIN = 4;

// Reactor Button Pins
const int BTN1 = 20; // Reactor 1
const int BTN2 = 10; // Reactor 2
const int BTN3 = 11; // Reactor 3

// Emergency Stop button Pin
const int BTN4 = 3; // Emergency Stop button

// Green and Red LED Pins
const int GREEN_LED_PIN = 21;
const int RED_LED_PIN = 12;

// Reactor LED Pins 
const int R1_LED_PIN = 5;
const int R2_LED_PIN = 6;
const int R3_LED_PIN = 1;

// Shared Pins for OLED1 (Fission Rate) and OLED2 (Control Rod)
#define LCD_MOSI 35 // SDA for Circular OLED
#define LCD_MISO -1 // 
#define LCD_CLK  36 // SCK for Circular OLED
#define BACKLIGHT_PIN 7// BLK for circular OLED

// PINS for OLED 1 (Fission Rate Display)
#define LCD_CS   13 // CS for OLED1
#define LCD_DC   14 // DC for OLED1
#define LCD_RST  15 //RST for OLED1

// PINS FOR OLED 2 (Control Rod Insertion % Display)
#define LCDB_CS 16 // CS for OLED 2
#define LCDB_DC 17 // DC for OLED 2
#define LCDB_RST 18 // RST for OLED 2

// Pins for the LCD Commands, Score, and Countdown Display
#define I2C_MOSI 8 // SDA for the LCD
#define I2C_MISO 9 // SCL for the LCD 

// Pins for the Touch Sensor (For Call Manager Input):
#define ECHO_PIN 39 // ultrasonic sensor echo pin
#define TRIG_PIN 38 // Ultrasonic Snesor Trigger Pin

// DFPlayer
#define RXD2 45 // DFPlayer RX
#define TXD2 48 // DFPlayer TX




#endif