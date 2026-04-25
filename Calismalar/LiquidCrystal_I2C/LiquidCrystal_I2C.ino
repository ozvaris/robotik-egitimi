#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

byte c_cedilla[8] = {
  B00000,
  B00000,
  B01110,
  B00001,
  B01111,
  B10001,
  B01111,
  B00100
};


void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  lcd.init();
  lcd.backlight();

  // 1. Satır Merhaba yaz
  lcd.setCursor(0, 0);
  lcd.print("Merhaba!");
  delay(5 * 1000);  

  // 2. Satır I2C LCD Hazir yaz
  lcd.setCursor(0, 1);
  lcd.print("I2C LCD Hazir");
  delay(5 * 1000); 

  // Ekranı Temizle
  lcd.setCursor(0, 0);
  lcd.print("  ");
  lcd.setCursor(0, 1);
  lcd.print("             ");
  delay(5 * 1000); 

  // 1. satır 5 kolon aaa yaz
  lcd.setCursor(5, 0);
  lcd.print("aaa");


}

void loop() {
  // put your main code here, to run repeatedly:

}
