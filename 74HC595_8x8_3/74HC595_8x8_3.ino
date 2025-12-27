/*
  74HC595 ile 4x4 (R1..R4, C1..C4) tarama örneği

  Donanım NOTLARI
  - Kolonlar ACTIVE-LOW: Kolon biti 0 olunca GND olur ve LED yanar.
  - Satırlar ACTIVE-HIGH varsayımı: Satır biti 1 olunca satır aktif olur.

  INPUT MASK (kolay kullanım)
  - makeByteFromMask(mask) fonksiyonuna verdiğin mask formatı:
      b7 b6 b5 b4  b3 b2 b1 b0
      R1 R2 R3 R4  C1 C2 C3 C4
    Yani:
      b7=R1, b6=R2, b5=R3, b4=R4, b3=C1, b2=C2, b1=C3, b0=C4

  OUTPUT (74HC595'e giden byte)
  - Senin kablolama/mapping'ine göre yeniden yerleştiriliyor:
      out b0=R4, out b1=R2, out b2=R1, out b3=R3,
      out b4=C4, out b5=C2, out b6=C3, out b7=C1
*/

// 4x4 frame: her satır için "ON pattern" (C1C2C3C4) 1=yanık.
// (Biz bunu active-low kolon nibble'a çeviriyoruz.)
struct Frame {
  const char* name;
  byte rowOn[4];  // nibble: b3=C1 .. b0=C4 (1=ON)
};

const int DATA_PIN = 12;   // 74HC595 DS   (pin 14)
const int CLOCK_PIN = 13;  // 74HC595 SHCP (pin 11)
const int LATCH_PIN = 11;  // 74HC595 STCP (pin 12)

// 74HC595'e 1 byte yaz
void shiftWrite(byte value) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);
  digitalWrite(LATCH_PIN, HIGH);
}

/*
  makeByte:
  - Ayrı ayrı R/C bitlerini alır (0/1)
  - Senin donanım bit sıralamana göre tek bir "output byte" üretir
*/
byte makeByte(
  byte R1, byte R2, byte R3, byte R4,
  byte C1, byte C2, byte C3, byte C4) {
  byte v = 0;

  // Rows -> output bitleri
  if (R4) v |= (1 << 2);
  if (R2) v |= (1 << 0);
  if (R1) v |= (1 << 3);
  if (R3) v |= (1 << 7);

  // Cols -> output bitleri
  if (C4) v |= (1 << 4);
  if (C2) v |= (1 << 5);
  if (C3) v |= (1 << 6);
  if (C1) v |= (1 << 1);

  return v;
}

/*
  makeByteFromMask:
  - Tek bir "input mask" alır (b7..b0 = R1 R2 R3 R4 C1 C2 C3 C4)
  - Mask içinden R/C bitlerini ayırır
  - makeByte(...) çağırarak doğru output byte'ı döndürür
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

// satır başı bekleme (parlaklık/flicker ayarı)
const uint16_t ROW_HOLD_US = 1200;

// Row mask (b7..b4)
const byte ROW_MASKS[4] = {
  0b10000000,  // R1
  0b01000000,  // R2
  0b00100000,  // R3
  0b00010000   // R4
};

// 16x16 -> tam dolu
const Frame F_FULL = { "16x16(full)", { 0b1111, 0b1111, 0b1111, 0b1111 } };
// 8x8 -> çerçeve
const Frame F_BORDER = { "8x8(border)", { 0b1111, 0b1001, 0b1001, 0b1111 } };
// 4x4 -> ortada 2x2 dolu (C2,C3 ve R2,R3)
const Frame F_CENTER2 = { "4x4(center2x2)", { 0b0000, 0b0110, 0b0110, 0b0000 } };
// 1x1 -> tek nokta (R2,C2)
const Frame F_DOT = { "1x1(dot)", { 0b0000, 0b0100, 0b0000, 0b0000 } };

// 1 frame refresh (R1..R4 hızlı tarama)
void refreshOnceFrame(const Frame& f) {
  for (byte r = 0; r < 4; r++) {
    byte onNibble = f.rowOn[r] & 0x0F;        // C1..C4 (1=ON)
    byte colActiveLow = (~onNibble) & 0x0F;   // active-low: ON=0
    byte mask = ROW_MASKS[r] | colActiveLow;  // b7..b4 row + b3..b0 col
    shiftWrite(makeByteFromMask(mask));
    delayMicroseconds(ROW_HOLD_US);
  }
}

// Belirli süre boyunca frame göster (içeride hızlı tarama var!)
void showFrame(const Frame& f, uint16_t durationMs) {
  unsigned long t0 = millis();
  while (millis() - t0 < durationMs) {
    refreshOnceFrame(f);
  }
  shiftWrite(0);  // temizle
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  shiftWrite(0);
  Serial.begin(9600);
}

void loop() {
  // for (byte i = 0; i < 7; i++) {
  //   Serial.println(SEQ_NAME[i]);
  //   showGlyph(SEQ[i], 1000);                        // 1 saniye göster
  //   showGlyph((const byte[4]){ 0, 0, 0, 0 }, 120);  // küçük boşluk (istersen kaldır)
  // }

  // küçül
  showFrame(F_FULL, 1000);
  showFrame(F_BORDER, 1000);
  showFrame(F_CENTER2, 1000);
  showFrame(F_DOT, 1000);

  // büyü
  showFrame(F_CENTER2, 1000);
  showFrame(F_BORDER, 1000);
  showFrame(F_FULL, 1000);
}
