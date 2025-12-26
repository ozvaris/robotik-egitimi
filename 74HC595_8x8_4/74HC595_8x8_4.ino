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

struct Frame {
  const char* name;
  byte rowOn[4];  // b3=C1 .. b0=C4 (1=ON)
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

// Row seçme maskeleri (b7..b4): R1,R2,R3,R4
const byte ROW_MASKS[4] = {
  0b10000000, // R1
  0b01000000, // R2
  0b00100000, // R3
  0b00010000  // R4
};

// ===== GÖZ FRAME'LERİ (4x4) =====
const Frame EYES_OPEN  = { "OPEN",  { 
  0b0000,  // R1
  0b1001,  // R2  (C1 ve C4 yanık)
  0b1001,  // R3  (C1 ve C4 yanık)
  0b0000   // R4
}};

// “yarım kapanma” gibi: sadece tek satır iki nokta
const Frame EYES_HALF  = { "HALF",  { 
  0b0000,
  0b1001,  // R2
  0b0000,  // R3
  0b0000
}};

// kapalı göz: yatay çizgi
const Frame EYES_CLOSED = { "CLOSE", { 
  0b0000,
  0b0000,
  0b1111,  // R3 (tam çizgi)
  0b0000
}};

// Parlaklık / titreme ayarı (satır başına bekleme)
const uint16_t ROW_HOLD_US = 1200;

// ===== GÖZ KIRPMA ANİMASYONU =====
void blinkOnce() {
  showFrame(EYES_OPEN,   700);  // açık bekle
  showFrame(EYES_HALF,    80);  // kapanmaya başla
  showFrame(EYES_CLOSED, 120);  // kapalı
  showFrame(EYES_HALF,    80);  // açılmaya başla
  showFrame(EYES_OPEN,   700);  // tekrar açık
}

void showFrame(const Frame& f, uint16_t durationMs) {
  unsigned long t0 = millis();
  while (millis() - t0 < durationMs) {
    refreshOnce(f);
  }
  shiftWrite(0);
}

// 1 frame refresh (R1..R4 tarama)
void refreshOnce(const Frame& f) {
  for (byte r = 0; r < 4; r++) {
    byte onNibble    = f.rowOn[r] & 0x0F;    // 1=yanık
    byte colActiveLo = (~onNibble) & 0x0F;   // kolon active-low => yanık=0

    byte mask = ROW_MASKS[r] | colActiveLo; // b7..b4 row + b3..b0 col
    shiftWrite(makeByteFromMask(mask));

    delayMicroseconds(ROW_HOLD_US);         // sende zaten var
  }
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  shiftWrite(0);
}

void loop() {
  blinkOnce();
  // istersen arada “çift kırpma”:
  // showFrame(EYES_OPEN, 200);
  // showFrame(EYES_CLOSED, 100);
  // showFrame(EYES_OPEN, 400);
}
