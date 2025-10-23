#include "LCDScreen.h"
#include 
LiquidCrystal_I2C lcd(0x27, 16, 2);

void LCD_init(){
  Wire.begin(I2C_MOSI, I2C_MISO);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("LCD Ready);");
}

void LCD_Display(){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Hello!");
}