#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
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

byte rowPins[ROWS] = {9, 8, 7, 6};
byte colPins[COLS] = {5, 4, 3, 2};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// ---------------- EEPROM ----------------
const byte EEPROM_MAGIC_ADDR = 0;
const byte EEPROM_PASS_ADDR  = 1;
const byte EEPROM_MAGIC_VAL  = 0x5A;

// ---------------- MODLAR ----------------
enum Mod {
  SIFRE_GIRIS,
  DEGISTIRME_ESKI_SIFRE,
  YENI_SIFRE_1,
  YENI_SIFRE_2
};

Mod aktifMod = SIFRE_GIRIS;

// ---------------- SIFRELER ----------------
char aktifSifre[5] = "1234";   // EEPROM bossa varsayilan
char girilen[5] = "";
char yeniSifre[5] = "";
byte indexNo = 0;

bool yildizBekleniyor = false;

// ---------------- YARDIMCI ----------------
void bufferTemizle() {
  indexNo = 0;
  girilen[0] = '\0';
}

void mesajGoster(const char* satir1, const char* satir2 = "") {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(satir1);
  lcd.setCursor(0, 1);
  lcd.print(satir2);
}

bool sifreGecerliMi(const char* sifre) {
  for (byte i = 0; i < 4; i++) {
    if (sifre[i] < '0' || sifre[i] > '9') return false;
  }
  return sifre[4] == '\0';
}

void eepromSifreKaydet(const char* sifre) {
  EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VAL);

  for (byte i = 0; i < 5; i++) {
    EEPROM.update(EEPROM_PASS_ADDR + i, sifre[i]);
  }
}

void eepromSifreYukle() {
  if (EEPROM.read(EEPROM_MAGIC_ADDR) != EEPROM_MAGIC_VAL) {
    strcpy(aktifSifre, "1234");
    eepromSifreKaydet(aktifSifre);
    return;
  }

  char tmp[5];
  for (byte i = 0; i < 5; i++) {
    tmp[i] = EEPROM.read(EEPROM_PASS_ADDR + i);
  }

  if (sifreGecerliMi(tmp)) {
    strcpy(aktifSifre, tmp);
  } else {
    strcpy(aktifSifre, "1234");
    eepromSifreKaydet(aktifSifre);
  }
}

void sifreGirisEkrani() {
  aktifMod = SIFRE_GIRIS;
  yildizBekleniyor = false;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lutfen Sifre");
  lcd.setCursor(0, 1);
  lcd.print("Giriniz:");
}

void yildizUyariEkrani() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tekrar * basin");
  lcd.setCursor(0, 1);
  lcd.print("Sifre degistir");
}

void eskiSifreDogrulaEkrani() {
  aktifMod = DEGISTIRME_ESKI_SIFRE;
  yildizBekleniyor = false;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aktif sifreyi");
  lcd.setCursor(0, 1);
  lcd.print("giriniz:");
}

void yeniSifre1Ekrani() {
  aktifMod = YENI_SIFRE_1;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Yeni sifre:");
  lcd.setCursor(0, 1);
}

void yeniSifre2Ekrani() {
  aktifMod = YENI_SIFRE_2;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tekrar girin:");
  lcd.setCursor(0, 1);
}

void yildizYaz() {
  if (aktifMod == SIFRE_GIRIS) {
    lcd.setCursor(8 + indexNo - 1, 1);   // "Giriniz:" sonrasi
    lcd.print('*');
  } else if (aktifMod == DEGISTIRME_ESKI_SIFRE) {
    lcd.setCursor(8 + indexNo - 1, 1);   // "giriniz:" sonrasi
    lcd.print('*');
  } else {
    lcd.setCursor(indexNo - 1, 1);
    lcd.print('*');
  }
}

void rakamEkle(char key) {
  if (indexNo < 4) {
    girilen[indexNo] = key;
    indexNo++;
    girilen[indexNo] = '\0';
    yildizYaz();
  }
}

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  eepromSifreYukle();
  sifreGirisEkrani();
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  // ---------------- NORMAL GIRIS ----------------
  if (aktifMod == SIFRE_GIRIS) {

    if (key >= '0' && key <= '9') {
      if (yildizBekleniyor) {
        sifreGirisEkrani();
      }

      rakamEkle(key);

      if (indexNo == 4) {
        delay(150);

        if (strcmp(girilen, aktifSifre) == 0) {
          mesajGoster("Kapi acildi");
        } else {
          mesajGoster("Hatali sifre");
        }

        delay(2000);
        sifreGirisEkrani();
      }
    }
    else if (key == '*') {
      if (indexNo == 0) {
        if (!yildizBekleniyor) {
          yildizBekleniyor = true;
          yildizUyariEkrani();
        } else {
          eskiSifreDogrulaEkrani();
        }
      } else {
        sifreGirisEkrani();
      }
    }
  }

  // ---------------- AKTIF SIFRE DOGRULAMA ----------------
  else if (aktifMod == DEGISTIRME_ESKI_SIFRE) {

    if (key >= '0' && key <= '9') {
      rakamEkle(key);

      if (indexNo == 4) {
        delay(150);

        if (strcmp(girilen, aktifSifre) == 0) {
          mesajGoster("Dogrulandi");
          delay(1000);
          yeniSifre1Ekrani();
        } else {
          mesajGoster("Hatali aktif", "sifre");
          delay(2000);
          sifreGirisEkrani();
        }
      }
    }
    else if (key == '*') {
      eskiSifreDogrulaEkrani();
    }
  }

  // ---------------- YENI SIFRE 1 ----------------
  else if (aktifMod == YENI_SIFRE_1) {

    if (key >= '0' && key <= '9') {
      rakamEkle(key);

      if (indexNo == 4) {
        strcpy(yeniSifre, girilen);
        delay(200);
        yeniSifre2Ekrani();
      }
    }
    else if (key == '*') {
      yeniSifre1Ekrani();
    }
  }

  // ---------------- YENI SIFRE 2 ----------------
  else if (aktifMod == YENI_SIFRE_2) {

    if (key >= '0' && key <= '9') {
      rakamEkle(key);

      if (indexNo == 4) {
        delay(150);

        if (strcmp(girilen, yeniSifre) == 0) {
          strcpy(aktifSifre, yeniSifre);
          eepromSifreKaydet(aktifSifre);   // KALICI KAYIT
          mesajGoster("Sifre degisti");
        } else {
          mesajGoster("Sifreler farkli");
        }

        delay(2000);
        sifreGirisEkrani();
      }
    }
    else if (key == '*') {
      yeniSifre2Ekrani();
    }
  }
}