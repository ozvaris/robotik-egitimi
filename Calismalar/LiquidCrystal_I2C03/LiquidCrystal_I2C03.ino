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

  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("\I2C LCD Hazir");

}

void loop() {
  
  static unsigned long last = 0;
  static int sec = 0;  

  // Serial.println(millis());

  if (millis() - last >= 1000) {
    last = millis();
    sec++;

    Serial.println(sec);
    lcd.setCursor(12, 0);
    if(sec < 10) lcd.print("     ");
    lcd.setCursor(12, 0);
    lcd.print(sec);

  }
  
}

