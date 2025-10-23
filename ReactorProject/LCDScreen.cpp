#include "LCDScreen.h"
#include "UserCommands.h"
#include "TaskChecker.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);

void LCD_init(){
  Wire.begin(I2C_MOSI, I2C_MISO);
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Reactors Powered ON");
}

void LCD_Display(String command, float score){
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(command);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(score);
}