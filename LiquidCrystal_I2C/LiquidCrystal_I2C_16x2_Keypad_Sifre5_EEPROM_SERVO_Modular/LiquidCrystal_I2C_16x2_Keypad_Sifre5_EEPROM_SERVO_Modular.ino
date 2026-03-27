#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>
#include <Servo.h>
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

// ---------------- SERVO ----------------
Servo kapiServo;
const byte SERVO_PIN = 10;
const int SERVO_KAPALI_ACI = 0;
const int SERVO_ACIK_ACI   = 180;

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
char aktifSifre[5] = "1234";
char girilen[5] = "";
char yeniSifre[5] = "";
byte indexNo = 0;

bool yildizBekleniyor = false;

// ---------------- PROTOTYPES ----------------
void handleSifreGiris(char key);
void handleDegistirmeEskiSifre(char key);
void handleYeniSifre1(char key);
void handleYeniSifre2(char key);

void bufferTemizle();
void mesajGoster(const char* satir1, const char* satir2 = "");
bool sifreGecerliMi(const char* sifre);
void eepromSifreKaydet(const char* sifre);
void eepromSifreYukle();
void sifreGirisEkrani();
void yildizUyariEkrani();
void eskiSifreDogrulaEkrani();
void yeniSifre1Ekrani();
void yeniSifre2Ekrani();
void yildizYaz();
void rakamEkle(char key);
void kapiAc();

// ---------------- YARDIMCI ----------------
void bufferTemizle() {
  indexNo = 0;
  girilen[0] = '\0';
}

void mesajGoster(const char* satir1, const char* satir2) {
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

void yildizYaz() {
  if (aktifMod == SIFRE_GIRIS) {
    lcd.setCursor(8 + indexNo - 1, 1);
    lcd.print('*');
  } else if (aktifMod == DEGISTIRME_ESKI_SIFRE) {
    lcd.setCursor(8 + indexNo - 1, 1);
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

void kapiAc() {
  kapiServo.write(SERVO_ACIK_ACI);
  delay(5000);
  kapiServo.write(SERVO_KAPALI_ACI);
}

void setup() {
  Wire.begin();
  lcd.init();
  lcd.backlight();

  kapiServo.attach(SERVO_PIN);
  kapiServo.write(SERVO_KAPALI_ACI);

  eepromSifreYukle();
  sifreGirisEkrani();
}

void loop() {
  char key = keypad.getKey();
  if (!key) return;

  if (aktifMod == SIFRE_GIRIS) {
    handleSifreGiris(key);
  }
  else if (aktifMod == DEGISTIRME_ESKI_SIFRE) {
    handleDegistirmeEskiSifre(key);
  }
  else if (aktifMod == YENI_SIFRE_1) {
    handleYeniSifre1(key);
  }
  else if (aktifMod == YENI_SIFRE_2) {
    handleYeniSifre2(key);
  }
}