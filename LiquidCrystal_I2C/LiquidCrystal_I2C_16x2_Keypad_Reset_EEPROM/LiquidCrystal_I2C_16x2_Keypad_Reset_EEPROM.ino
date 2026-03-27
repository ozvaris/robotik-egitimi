#include <EEPROM.h>

const byte EEPROM_MAGIC_ADDR = 0;
const byte EEPROM_PASS_ADDR  = 1;
const byte EEPROM_MAGIC_VAL  = 0x5A;

const char yeniSifre[] = "1234";

void setup() {
  // Magic byte yaz
  EEPROM.update(EEPROM_MAGIC_ADDR, EEPROM_MAGIC_VAL);

  // Sifreyi yaz: '1','2','3','4','\0'
  for (byte i = 0; i < 5; i++) {
    EEPROM.update(EEPROM_PASS_ADDR + i, yeniSifre[i]);
  }

  // İstersen seri porttan bilgi ver
  Serial.begin(9600);
  Serial.println("EEPROM sifresi 1234 olarak resetlendi.");
}

void loop() {
  // Bos
}