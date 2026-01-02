#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Scanner sonucunda buldugun adresi buraya yaz (en yaygin: 0x27 veya 0x3F)
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Wire.begin();

  lcd.init();          // LCD baslat
  lcd.backlight();     // Arka isik ac

  lcd.setCursor(0, 0);
  lcd.print("Merhaba!");

  lcd.setCursor(0, 1);
  lcd.print("I2C LCD Hazir");
}

void loop() {
  // Ornek: saniye sayaci
  static unsigned long last = 0;
  static int sec = 0;

  if (millis() - last >= 1000) {
    last = millis();
    sec++;

    lcd.setCursor(12, 1);     // 2. satirin sonuna yakin
    if (sec < 10) lcd.print("  ");      // temizle
    else if (sec < 100) lcd.print(" "); // temizle
    lcd.setCursor(12, 1);
    lcd.print(sec);
  }
}
