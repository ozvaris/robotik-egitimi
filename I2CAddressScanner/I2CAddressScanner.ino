#include <Wire.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) {}

  Wire.begin();
  Serial.println("\nI2C Scanner basladi...");

  byte count = 0;
  for (byte address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    byte error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("I2C cihaz bulundu: 0x");
      if (address < 16) Serial.print("0");
      Serial.println(address, HEX);
      count++;
    }
  }

  if (count == 0) Serial.println("Hic I2C cihaz bulunamadi.");
  else {
    Serial.print("Toplam cihaz: ");
    Serial.println(count);
  }
}

void loop() {}
