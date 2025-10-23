#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define I2C_MOSI 8 // SDA
#define I2C_MISO 9 // SCL


void LCD_init();

void LCD_Display();