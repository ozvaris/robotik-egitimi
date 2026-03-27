#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <string.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- KEYMAP ----------------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Keypad baglantilari
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- SIFRE ----------------
const char dogruSifre[] = "2345";   // Burayi istedigin sifreyle degistirebilirsin
char girilenSifre[5];               // 4 hane + sonlandirici
byte sifreIndex = 0;

void sifreEkraniGoster() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lutfen Sifre");

  lcd.setCursor(0, 1);
  lcd.print("Giriniz: ");

  sifreIndex = 0;
  girilenSifre[0] = '\0';
}

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  sifreEkraniGoster();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    // Sadece rakam kabul et
    if (key >= '0' && key <= '9') {
      if (sifreIndex < 4) {
        girilenSifre[sifreIndex] = key;
        sifreIndex++;
        girilenSifre[sifreIndex] = '\0';

        // Girilen karakteri ekranda goster
        lcd.setCursor(9 + sifreIndex - 1, 1);
        lcd.print('*');   // Istersen '*' yerine key yazabilirsin
      }

      // 4 hane tamamlaninca kontrol et
      if (sifreIndex == 4) {
        delay(200);

        if (strcmp(girilenSifre, dogruSifre) == 0) {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Kapi acildi");
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Hatali sifre");
        }

        delay(2000);
        sifreEkraniGoster();
      }
    }

    // * tusu ile temizle
    else if (key == '*') {
      sifreEkraniGoster();
    }
  }
}