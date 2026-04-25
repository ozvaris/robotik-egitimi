#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  lcd.init();
  lcd.backlight();

  // 1. Satır Merhaba yaz
  lcd.setCursor(0, 0);
  lcd.print("Merhaba!");

  lcd.setCursor(0, 1);


  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(13, OUTPUT);




}

void loop() {
  // put your main code here, to run repeatedly:
  delay(1000);
  digitalWrite(LED_BUILTIN, HIGH);  // turn the LED on (HIGH is the voltage level)
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);  // turn the LED on (HIGH is the voltage level)

  for (int pos = 0; pos <= 16; pos++)
  {
    lcd.setCursor(0, 0);
    lcd.clear();

    lcd.setCursor(pos, 0);
    lcd.print("Merhaba");

    delay(500); //500ms bekle

  }




}
