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

const int DATA_PIN  = 12;  // 74HC595 DS   (pin 14)
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
  byte C1, byte C2, byte C3, byte C4
) {
  byte v = 0;

  // Rows -> output bitleri
  if (R4) v |= (1 << 0);
  if (R2) v |= (1 << 1);
  if (R1) v |= (1 << 2);
  if (R3) v |= (1 << 3);

  // Cols -> output bitleri
  if (C4) v |= (1 << 4);
  if (C2) v |= (1 << 5);
  if (C3) v |= (1 << 6);
  if (C1) v |= (1 << 7);

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

/*
  4x4 tarama:
  - Her adımda 1 satır + 1 kolon seçilir.
  - Kolonlar active-LOW olduğu için seçilen kolon biti 0 yapılır.
*/
void scanAllSegments(uint16_t holdMs) {
  // Rows: R1..R4 (active-HIGH)
  const byte ROW_MASKS[4] = {
    0b10000000, // R1
    0b01000000, // R2
    0b00100000, // R3
    0b00010000  // R4
  };

  // Cols: C1..C4 (active-LOW: seçilen kolon 0)
  // Alt nibble = C1 C2 C3 C4 (b3..b0)
  const byte COL_MASKS[4] = {
    0b00000111, // C1 seçili: 0 1 1 1
    0b00001011, // C2 seçili: 1 0 1 1
    0b00001101, // C3 seçili: 1 1 0 1
    0b00001110  // C4 seçili: 1 1 1 0
  };

  for (byte r = 0; r < 4; r++) {
    for (byte c = 0; c < 4; c++) {
      byte mask = ROW_MASKS[r] | COL_MASKS[c];
      shiftWrite(makeByteFromMask(mask));
      delay(holdMs);
    }
  }

  shiftWrite(0); // tarama bitti -> kapat
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  shiftWrite(0);
  Serial.begin(9600);
}

void loop() {
  scanAllSegments(500);
}
