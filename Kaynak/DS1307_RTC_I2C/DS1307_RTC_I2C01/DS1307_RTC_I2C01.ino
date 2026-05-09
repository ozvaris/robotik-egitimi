#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

void setup() {
  Wire.begin();

  lcd.init();
  lcd.backlight();

  if (!rtc.begin()) {
    lcd.print("RTC bulunamadi");
    while (1);
  }

  // if (!rtc.isrunning()) {
  //   lcd.clear();
  //   lcd.print("RTC ayarlaniyor");

  //   // Bilgisayardaki derleme zamanini RTC'ye yazar
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  // }
}

void loop() {
  DateTime now = rtc.now();

  lcd.setCursor(0, 0);
  lcd.print("Saat: ");

  if (now.hour() < 10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");

  if (now.minute() < 10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");

  if (now.second() < 10) lcd.print("0");
  lcd.print(now.second());

  lcd.setCursor(0, 1);
  lcd.print("Tarih:");

  if (now.day() < 10) lcd.print("0");
  lcd.print(now.day());
  lcd.print("/");

  if (now.month() < 10) lcd.print("0");
  lcd.print(now.month());
  lcd.print("/");

  lcd.print(now.year());

  delay(1000);
}