#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

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

// Keypad pinleri
byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- DURUM ----------------
bool menuAcik = false;

void anaEkranGoster() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Merhaba!");

  lcd.setCursor(0, 1);
  lcd.print("I2C LCD Hazir");
}

void menuGoster() {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Menu");

  lcd.setCursor(0, 1);
  lcd.print("1) 2) 3) 4)Cikis");
}

void menuMesajiGoster(char menuNo) {
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(menuNo);
  lcd.print(" Numarali menuye");

  lcd.setCursor(0, 1);
  lcd.print("girdiniz");
}

void setup() {
  Wire.begin();

  lcd.init();
  lcd.backlight();

  anaEkranGoster();
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    // A ile menuyu ac
    if (key == 'A') {
      menuAcik = true;
      menuGoster();
    }

    // Menu acikken secimler
    else if (menuAcik) {
      if (key == '1') {
        menuMesajiGoster('1');
      }
      else if (key == '2') {
        menuMesajiGoster('2');
      }
      else if (key == '3') {
        menuMesajiGoster('3');
      }
      else if (key == '4') {
        menuAcik = false;
        anaEkranGoster();
      }
    }
  }
}