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
  lcd.print("Sure: 00:00:00");

  Serial.begin(115200);
  while (!Serial) {}
}

void printTwoDigits(int value) {
  if (value < 10) {
    lcd.print("0");
  }

  lcd.print(value);

}

void loop() {

  static unsigned long last = 0;
  static unsigned long totalSeconds;

  if(millis() - last >= 1000) {
     last = millis();

     totalSeconds++;
     // totalSeconds = totalSeconds + 1;

     unsigned long hours = totalSeconds / 3600 + 15;
     unsigned long minutes = (totalSeconds % 3600) / 60 + 51;
     unsigned long seconds = totalSeconds % 60;

     Serial.println(seconds);

     lcd.setCursor(6,1);
     printTwoDigits(hours);
     lcd.print(":");
     printTwoDigits(minutes);
     lcd.print(":");
     printTwoDigits(seconds);


  }


  
}
