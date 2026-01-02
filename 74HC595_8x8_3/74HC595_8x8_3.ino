/*
  74HC595 ile 4x4 (R1..R4, C1..C4) tarama
  - Kolonlar ACTIVE-LOW (0 -> ON)
  - Satırlar ACTIVE-HIGH (1 -> aktif)

  GLYPH formatı (harflerdeki gibi):
    Her satır için 1 byte:
      b7 b6 b5 b4  b3 b2 b1 b0
      R1 R2 R3 R4  C1 C2 C3 C4   (C'ler ACTIVE-LOW)
*/

const int DATA_PIN = 9;    // 74HC595 DS   (pin 14)
const int CLOCK_PIN = 10;  // 74HC595 SHCP (pin 11)
const int LATCH_PIN = 8;   // 74HC595 STCP (pin 12)

const uint16_t ROW_HOLD_US = 1200;  // satır tutma süresi (parlaklık/titreme ayarı)

// -------------------- 74HC595 yazma --------------------
void shiftWrite(byte value) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);
  digitalWrite(LATCH_PIN, HIGH);
}

/*
  makeByte:
  - Ayrı ayrı R/C bitlerini alır (0/1)
  - Senin kablolama/mapping'ine göre tek bir output byte üretir
  (Bu kısım "map()" değil: bu bir BIT PERMÜTASYONU)
*/
byte makeByte(
  byte R1, byte R2, byte R3, byte R4,
  byte C1, byte C2, byte C3, byte C4) {
  byte v = 0;

  // Rows -> output bitleri (senin kablolamana göre)
  if (R4) v |= (1 << 2);
  if (R2) v |= (1 << 0);
  if (R1) v |= (1 << 3);
  if (R3) v |= (1 << 7);

  // Cols -> output bitleri (senin kablolamana göre)
  if (C4) v |= (1 << 4);
  if (C2) v |= (1 << 5);
  if (C3) v |= (1 << 6);
  if (C1) v |= (1 << 1);

  return v;
}

/*
  INPUT MASK -> R/C bitlerine ayırıp makeByte(...) çağırır
  mask formatı:
    b7=R1, b6=R2, b5=R3, b4=R4, b3=C1, b2=C2, b1=C3, b0=C4
*/
byte makeByteFromMask(byte mask) {
  byte R1 = (mask >> 7) & 0x01;
  byte R2 = (mask >> 6) & 0x01;
  byte R3 = (mask >> 5) & 0x01;
  byte R4 = (mask >> 4) & 0x01;

  byte C1 = (mask >> 3) & 0x01;
  byte C2 = (mask >> 2) & 0x01;
  byte C3 = (mask >> 1) & 0x01;
  byte C4 = (mask >> 0) & 0x01;

  return makeByte(R1, R2, R3, R4, C1, C2, C3, C4);
}

// -------------------- GLYPH tarama --------------------
void refreshOnce(const byte glyph[4]) {
  for (byte r = 0; r < 4; r++) {
    shiftWrite(makeByteFromMask(glyph[r]));
    delayMicroseconds(ROW_HOLD_US);
  }
}

void showGlyph(const byte glyph[4], uint16_t durationMs) {
  unsigned long t0 = millis();
  while (millis() - t0 < durationMs) {
    refreshOnce(glyph);
  }
  shiftWrite(0);  // temizle
}

const byte BLANK = 0b00001111; // rows off + cols off

const byte GLYPH_CLEAR[4] = {
  0b00000000,  // R1 + C1111(ON) -> C nibble 0000
  0b00000000,  // R2
  0b00000000,  // R3
  0b00000000   // R4
};

// -------------------- Kare -> Dot animasyon glyph'leri --------------------
// FULL: tüm 4x4 dolu (kolonlar ON => C nibble = 0000)
const byte GLYPH_FULL[4] = {
  0b10000000,  // R1 + C1111(ON) -> C nibble 0000
  0b01000000,  // R2
  0b00100000,  // R3
  0b00010000   // R4
};

// BORDER: çerçeve (R1/R4 full, R2/R3 sadece C1 ve C4 ON)
// C1 & C4 ON => C nibble (C1 C2 C3 C4) = 0 1 1 0 = 0b0110
const byte GLYPH_BORDER[4] = {
  0b10000000,  // R1: ████  (C nibble 0000)
  0b01000110,  // R2: █··█
  0b00100110,  // R3: █··█
  0b00010000   // R4: ████
};

// CENTER2: ortada 2x2 (R2/R3: C2,C3 ON)
// C2 & C3 ON => C nibble = 1 0 0 1 = 0b1001
const byte GLYPH_CENTER2[4] = {
  BLANK,  // boş (satır seçmiyoruz)
  0b01001001,  // R2: ·██·
  0b00101001,  // R3: ·██·
  BLANK
};

// DOT: tek nokta (R2, C2 ON)
// sadece C2 ON => C nibble = 1 0 1 1 = 0b1011
const byte GLYPH_DOT[4] = {
  BLANK,
  0b01001011,  // R2: ·█··  (C2 ON)
  BLANK,
  BLANK
};


void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  Serial.begin(115200);
  Serial.println("4x4 GLYPH animasyon basladi.");
}

void loop() {
  // küçül
  showGlyph(GLYPH_FULL, 1000);
  showGlyph((const byte[4]){BLANK,BLANK,BLANK,BLANK}, 120);  // küçük boşluk (istersen kaldır)
  showGlyph(GLYPH_BORDER,  1000);
  showGlyph((const byte[4]){BLANK,BLANK,BLANK,BLANK}, 120);  // küçük boşluk (istersen kaldır)
  showGlyph(GLYPH_CENTER2, 1000);
  showGlyph((const byte[4]){BLANK,BLANK,BLANK,BLANK}, 120);  // küçük boşluk (istersen kaldır)
  showGlyph(GLYPH_DOT, 1000);
  showGlyph((const byte[4]){BLANK,BLANK,BLANK,BLANK}, 120);  // küçük boşluk (istersen kaldır)

  // // büyü
  showGlyph(GLYPH_CENTER2, 1000);
  showGlyph((const byte[4]){BLANK,BLANK,BLANK,BLANK}, 120);  // küçük boşluk (istersen kaldır)
  showGlyph(GLYPH_BORDER,  1000);
  showGlyph((const byte[4]){BLANK,BLANK,BLANK,BLANK}, 120);  // küçük boşluk (istersen kaldır)
  showGlyph(GLYPH_FULL,    1000);
  showGlyph((const byte[4]){BLANK,BLANK,BLANK,BLANK}, 120);  // küçük boşluk (istersen kaldır)
}
