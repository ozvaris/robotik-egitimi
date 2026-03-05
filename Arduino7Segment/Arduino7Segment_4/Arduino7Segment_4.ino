#include "SevSeg.h"

SevSeg sevseg;

void setup() {
  byte numDigits = 1;          // tek hane
  byte digitPins[] = {};       // tek hane için boş bırakılır

  // Sıra: A, B, C, D, E, F, G, DP
  byte segmentPins[] = {7, 6, 5, 11, 10, 8, 9, 4};

  byte displayType = COMMON_CATHODE;
  bool resistorsOnSegments = true; // dirençler segmentlerdeyse true

  sevseg.begin(displayType, numDigits, digitPins, segmentPins, resistorsOnSegments);
  sevseg.setBrightness(90);
}

void loop() {
  for (int i = 0; i <= 9; i++) {
    unsigned long start = millis();
    // SevSeg multiplex için refreshDisplay() sık çağrılmalı
    while (millis() - start < 1000) {  // 1 saniye göster
      sevseg.setNumber(i);
      sevseg.refreshDisplay();
    }
  }
}
