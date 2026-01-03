/*
  2x 74HC595 ile 8x8 LED Matrix (Row scan)
  - Senin altyapı korunur: shiftWrite2, makeByte_0/1, mapMasksToOutputs, writeMasks, ROW_MASKS, COL_MASKS
  - Üstüne: refreshOnce, showGlyph, GLYPH set (0-9 + A-Z), SEQ/SEQ_NAME

  Varsayım:
  - Row seçimi: ACTIVE-HIGH  (1 => satır aktif)
  - Col seçimi: ACTIVE-LOW   (0 => kolon aktif)
*/

// ------------------------------------------------------------
// PINLER (senin dosyandan aynen)
// ------------------------------------------------------------
const int DATA_PIN_0 = 9;    // register1 (COL) 74HC595 DS
const int CLOCK_PIN_0 = 10;  // register1 (COL) 74HC595 SHCP
const int LATCH_PIN_0 = 8;   // register1 (COL) 74HC595 STCP

const int DATA_PIN_1 = 12;   // register2 (ROW) 74HC595 DS
const int CLOCK_PIN_1 = 13;  // register2 (ROW) 74HC595 SHCP
const int LATCH_PIN_1 = 11;  // register2 (ROW) 74HC595 STCP

// ------------------------------------------------------------
// AYARLAR (TERS/AYNA gibi durumlarda burayı değiştir)
// ------------------------------------------------------------
// GLYPH satır byte'ında "sol kolon = bit7" ise true, "sol kolon = bit0" ise false
const bool BIT_LEFT_IS_BIT7 = true;

// Eğer “1=ON” yerine “0=ON” gibi bir font setin varsa true yap
const bool GLYPH_INVERT = false;

// Satır başına bekleme (multiplex parlaklık/hız ayarı)
const uint16_t ROW_HOLD_US = 900;  // 600-1500 arası deneyebilirsin

// ------------------------------------------------------------
// 74HC595’e yazma (iki latch beraber)
// ------------------------------------------------------------
void shiftWrite2(byte register1Value, byte register2Value) {
  digitalWrite(LATCH_PIN_0, LOW);
  digitalWrite(LATCH_PIN_1, LOW);

  shiftOut(DATA_PIN_0, CLOCK_PIN_0, MSBFIRST, register1Value);
  shiftOut(DATA_PIN_1, CLOCK_PIN_1, MSBFIRST, register2Value);

  digitalWrite(LATCH_PIN_0, HIGH);
  digitalWrite(LATCH_PIN_1, HIGH);
}

void writeSingle(byte rowMask, byte colMask);

// ------------------------------------------------------------
// MAPPING (senin dosyandaki haliyle)
// ------------------------------------------------------------

// register2 (ROW) byte üretimi: B_0..B_7
byte makeByte_0(
  byte B_0, byte B_1, byte B_2, byte B_3,
  byte B_4, byte B_5, byte B_6, byte B_7) {
  byte v = 0;

  // identity: bit0->B_0 ... bit7->B_7
  if (B_0) v |= (1 << 0);
  if (B_1) v |= (1 << 1);
  if (B_2) v |= (1 << 2);
  if (B_3) v |= (1 << 3);
  if (B_4) v |= (1 << 4);
  if (B_5) v |= (1 << 5);
  if (B_6) v |= (1 << 6);
  if (B_7) v |= (1 << 7);

  return v;
}

// register1 (COL) byte üretimi: B_0..B_7
byte makeByte_1(
  byte B_0, byte B_1, byte B_2, byte B_3,
  byte B_4, byte B_5, byte B_6, byte B_7) {
  byte v = 0;

  // identity: bit0->B_0 ... bit7->B_7
  if (B_0) v |= (1 << 0);
  if (B_1) v |= (1 << 1);
  if (B_2) v |= (1 << 2);
  if (B_3) v |= (1 << 3);
  if (B_4) v |= (1 << 4);
  if (B_5) v |= (1 << 5);
  if (B_6) v |= (1 << 6);
  if (B_7) v |= (1 << 7);

  return v;
}

// İKİ BYTE AL -> İÇERDE MAPPING -> İKİ REGISTER BYTE ÇIKAR
void mapMasksToOutputs(byte rowMask, byte colMask, byte& register1Out, byte& register2Out) {
  // !!! DİKKAT !!!
  // Bu mapping kısmı senin önceki konuşmadan gelen “karışık eşleştirme” halin.
  // Aynen korudum. Eğer sen daha sonra mapping’i düzelttiysen burası değişebilir.

  // rowMask/colMask bitleri -> rB/cB ara bitlerine dağıtım
  byte rB0 = (colMask >> 7) & 0x01;  // C8
  byte rB1 = (colMask >> 6) & 0x01;  // C7
  byte rB2 = (rowMask >> 1) & 0x01;  // R2
  byte rB3 = (colMask >> 0) & 0x01;  // C1
  byte rB4 = (rowMask >> 3) & 0x01;  // R4
  byte rB5 = (colMask >> 5) & 0x01;  // C6
  byte rB6 = (colMask >> 3) & 0x01;  // C4
  byte rB7 = (rowMask >> 0) & 0x01;  // R1

  byte cB0 = (rowMask >> 4) & 0x01;  // R5
  byte cB1 = (rowMask >> 6) & 0x01;  // R7
  byte cB2 = (colMask >> 1) & 0x01;  // C2
  byte cB3 = (colMask >> 2) & 0x01;  // C3
  byte cB4 = (rowMask >> 7) & 0x01;  // R8
  byte cB5 = (colMask >> 4) & 0x01;  // C5
  byte cB6 = (rowMask >> 5) & 0x01;  // R6
  byte cB7 = (rowMask >> 2) & 0x01;  // R3

  // register2 = satırlar, register1 = kolonlar varsayımı
  register1Out = makeByte_0(rB0, rB1, rB2, rB3, rB4, rB5, rB6, rB7);
  register2Out = makeByte_1(cB0, cB1, cB2, cB3, cB4, cB5, cB6, cB7);
}

void writeMasks(byte rowMask, byte colMask) {
  byte reg1, reg2;
  mapMasksToOutputs(rowMask, colMask, reg1, reg2);
  shiftWrite2(reg1, reg2);
}

void allOff() {
  // satırları kapat + kolonları pasif yap (active-low => 1 pasif)
  writeMasks(0x00, 0xFF);
}

void writeSingle(byte rowMask, byte colMask) {
  byte reg1, reg2;
  mapMasksToOutputs(rowMask, colMask, reg1, reg2);
  shiftWrite2(reg1, reg2);
}

// ------------------------------------------------------------
// ROW/COL mask dizileri (senin dosyandan aynen)
// ------------------------------------------------------------
const byte ROW_MASKS[8] = {
  0b00000001,
  0b00000010,
  0b00000100,
  0b00001000,
  0b00010000,
  0b00100000,
  0b01000000,
  0b10000000
};

// Active-low: her eleman tek bir kolonu 0 yapar
const byte COL_MASKS[8] = {
  0b11111110,
  0b11111101,
  0b11111011,
  0b11110111,
  0b11101111,
  0b11011111,
  0b10111111,
  0b01111111
};

// ------------------------------------------------------------
// GLYPH yardımcıları
// ------------------------------------------------------------
static inline byte readBitForCol(byte rowByte, byte colIndex0to7) {
  // colIndex: 0 soldan, 7 sağdan gibi düşünelim
  if (BIT_LEFT_IS_BIT7) {
    // sol = bit7, sağ = bit0
    return (rowByte >> (7 - colIndex0to7)) & 0x01;
  } else {
    // sol = bit0, sağ = bit7
    return (rowByte >> (colIndex0to7)) & 0x01;
  }
}

byte buildColMaskFromRowByte(byte rowByte) {
  if (GLYPH_INVERT) rowByte = ~rowByte;

  // active-low kolon mantığı: ON olan kolonlar 0'a çekilecek
  byte colMask = 0xFF;  // hepsi pasif

  for (byte col = 0; col < 8; col++) {
    if (readBitForCol(rowByte, col)) {
      colMask &= COL_MASKS[col];  // o kolonu 0 yap
    }
  }
  return colMask;
}

// ------------------------------------------------------------
// refreshOnce / showGlyph
// ------------------------------------------------------------
void refreshOnce(const byte glyph[8]) {
  // 8 satırı hızlı tarar
  for (byte r = 0; r < 8; r++) {
    byte rowMask = ROW_MASKS[r];                       // satırı seç (active-high)
    byte colMask = buildColMaskFromRowByte(glyph[r]);  // o satırdaki kolonları yak
    writeMasks(rowMask, colMask);
    delayMicroseconds(ROW_HOLD_US);
  }
  // çok kısa kapatmak ghosting'i azaltır
  allOff();
}

void showGlyph(const byte* glyph, uint16_t ms) {
  uint32_t endAt = millis() + ms;
  while ((int32_t)(millis() - endAt) < 0) {
    refreshOnce(glyph);
  }
}


// ------------------------------------------------------------
// GLYPH SET (0-9 + A-Z)
// 8 satır, her satır 8 bit. 1 = ON varsayımı.
// ------------------------------------------------------------
const byte GLYPH_SPACE[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

const byte GLYPH_0[8] = { 0b00000000, 0b00111100, 0b01000010, 0b01000110, 0b01001010, 0b01100010, 0b01000010, 0b00111100 };
const byte GLYPH_1[8] = { 0b00000000, 0b00011000, 0b00101000, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00111110 };
const byte GLYPH_2[8] = { 0b00000000, 0b00111100, 0b01000010, 0b00000010, 0b00001100, 0b00110000, 0b01000000, 0b01111110 };
const byte GLYPH_3[8] = { 0b00000000, 0b00111100, 0b01000010, 0b00000010, 0b00011100, 0b00000010, 0b01000010, 0b00111100 };
const byte GLYPH_4[8] = { 0b00000000, 0b00000100, 0b00001100, 0b00010100, 0b00100100, 0b01111110, 0b00000100, 0b00000100 };
const byte GLYPH_5[8] = { 0b00000000, 0b01111110, 0b01000000, 0b01111100, 0b00000010, 0b00000010, 0b01000010, 0b00111100 };
const byte GLYPH_6[8] = { 0b00000000, 0b00111100, 0b01000000, 0b01111100, 0b01000010, 0b01000010, 0b01000010, 0b00111100 };
const byte GLYPH_7[8] = { 0b00000000, 0b01111110, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b00100000 };
const byte GLYPH_8[8] = { 0b00000000, 0b00111100, 0b01000010, 0b01000010, 0b00111100, 0b01000010, 0b01000010, 0b00111100 };
const byte GLYPH_9[8] = { 0b00000000, 0b00111100, 0b01000010, 0b01000010, 0b00111110, 0b00000010, 0b01000010, 0b00111100 };

const byte GLYPH_A[8] = { 0b00000000, 0b00011000, 0b00100100, 0b01000010, 0b01000010, 0b01111110, 0b01000010, 0b01000010 };
const byte GLYPH_B[8] = { 0b00000000, 0b01111100, 0b01000010, 0b01000010, 0b01111100, 0b01000010, 0b01000010, 0b01111100 };
const byte GLYPH_C[8] = { 0b00000000, 0b00111100, 0b01000010, 0b01000000, 0b01000000, 0b01000000, 0b01000010, 0b00111100 };
const byte GLYPH_D[8] = { 0b00000000, 0b01111000, 0b01000100, 0b01000010, 0b01000010, 0b01000010, 0b01000100, 0b01111000 };
const byte GLYPH_E[8] = { 0b00000000, 0b01111110, 0b01000000, 0b01000000, 0b01111100, 0b01000000, 0b01000000, 0b01111110 };
const byte GLYPH_F[8] = { 0b00000000, 0b01111110, 0b01000000, 0b01000000, 0b01111100, 0b01000000, 0b01000000, 0b01000000 };
const byte GLYPH_G[8] = { 0b00000000, 0b00111100, 0b01000010, 0b01000000, 0b01001110, 0b01000010, 0b01000010, 0b00111100 };
const byte GLYPH_H[8] = { 0b00000000, 0b01000010, 0b01000010, 0b01000010, 0b01111110, 0b01000010, 0b01000010, 0b01000010 };
const byte GLYPH_I[8] = { 0b00000000, 0b00111110, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00111110 };
const byte GLYPH_J[8] = { 0b00000000, 0b00011110, 0b00000100, 0b00000100, 0b00000100, 0b01000100, 0b01000100, 0b00111000 };
const byte GLYPH_K[8] = { 0b00000000, 0b01000010, 0b01000100, 0b01001000, 0b01110000, 0b01001000, 0b01000100, 0b01000010 };
const byte GLYPH_L[8] = { 0b00000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01000000, 0b01111110 };
const byte GLYPH_M[8] = { 0b00000000, 0b01000010, 0b01100110, 0b01011010, 0b01000010, 0b01000010, 0b01000010, 0b01000010 };
const byte GLYPH_N[8] = { 0b00000000, 0b01000010, 0b01100010, 0b01010010, 0b01001010, 0b01000110, 0b01000010, 0b01000010 };
const byte GLYPH_O[8] = { 0b00000000, 0b00111100, 0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b00111100 };
const byte GLYPH_P[8] = { 0b00000000, 0b01111100, 0b01000010, 0b01000010, 0b01111100, 0b01000000, 0b01000000, 0b01000000 };
const byte GLYPH_Q[8] = { 0b00000000, 0b00111100, 0b01000010, 0b01000010, 0b01000010, 0b01001010, 0b01000100, 0b00111010 };
const byte GLYPH_R[8] = { 0b00000000, 0b01111100, 0b01000010, 0b01000010, 0b01111100, 0b01001000, 0b01000100, 0b01000010 };
const byte GLYPH_S[8] = { 0b00000000, 0b00111110, 0b01000000, 0b01000000, 0b00111100, 0b00000010, 0b00000010, 0b01111100 };
const byte GLYPH_T[8] = { 0b00000000, 0b01111110, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000, 0b00001000 };
const byte GLYPH_U[8] = { 0b00000000, 0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b00111100 };
const byte GLYPH_V[8] = { 0b00000000, 0b01000010, 0b01000010, 0b01000010, 0b01000010, 0b00100100, 0b00100100, 0b00011000 };
const byte GLYPH_W[8] = { 0b00000000, 0b01000010, 0b01000010, 0b01000010, 0b01011010, 0b01011010, 0b01100110, 0b01000010 };
const byte GLYPH_X[8] = { 0b00000000, 0b01000010, 0b00100100, 0b00011000, 0b00011000, 0b00011000, 0b00100100, 0b01000010 };
const byte GLYPH_Y[8] = { 0b00000000, 0b01000010, 0b00100100, 0b00011000, 0b00001000, 0b00001000, 0b00001000, 0b00001000 };
const byte GLYPH_Z[8] = { 0b00000000, 0b01111110, 0b00000010, 0b00000100, 0b00001000, 0b00010000, 0b00100000, 0b01111110 };
const byte GLYPH_DEGREE[8] = { 0b00000000, 0b00011000, 0b00011000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000 };


static const byte* glyphForChar(char c) {
  switch (c) {
    case 'A': return GLYPH_A;
    case 'B': return GLYPH_B;  // eğer ana dosyada yoksa sil
    case 'C': return GLYPH_C;  // eğer ana dosyada yoksa sil
    case 'D': return GLYPH_D;  // eğer ana dosyada yoksa sil
    case 'E': return GLYPH_E;
    case 'G': return GLYPH_G;  // eğer ana dosyada yoksa sil
    case 'H': return GLYPH_H;
    case 'I': return GLYPH_I;  // eğer ana dosyada yoksa sil
    case 'J': return GLYPH_J;
    case 'K': return GLYPH_K;
    case 'L': return GLYPH_L;
    case 'M': return GLYPH_M;
    case 'N': return GLYPH_N;  // eğer ana dosyada yoksa sil
    case 'O': return GLYPH_O;
    case 'P': return GLYPH_P;
    case 'R': return GLYPH_R;
    case 'S': return GLYPH_S;
    case 'T': return GLYPH_T;
    case 'U': return GLYPH_U;  // eğer ana dosyada yoksa sil
    case 'V': return GLYPH_V;  // eğer ana dosyada yoksa sil
    case 'Y': return GLYPH_Y;
    case 'Z': return GLYPH_Z;
    case 'X': return GLYPH_X;
    case ' ': return GLYPH_SPACE;

    case '0': return GLYPH_0;
    case '1': return GLYPH_1;
    case '2': return GLYPH_2;
    case '3': return GLYPH_3;
    case '4': return GLYPH_4;
    case '5': return GLYPH_5;
    case '6': return GLYPH_6;
    case '7': return GLYPH_7;
    case '8': return GLYPH_8;
    case '9': return GLYPH_9;

    // Derece işareti için özel karakter:
    // Metinde bunu kullanmak için '^' yazacağız (çünkü C string içinde '°' bazen sorun çıkarıyor)
    case '^': return GLYPH_DEGREE;

    default: return GLYPH_SPACE;
  }
}


// ------------------------------------------------------------
// SEQ / SEQ_NAME
// ------------------------------------------------------------
const char* SEQ_NAME[] = {
  "0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
  "A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
  "N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z"
};

const byte* const SEQ[] = {
  GLYPH_0, GLYPH_1, GLYPH_2, GLYPH_3, GLYPH_4, GLYPH_5, GLYPH_6, GLYPH_7, GLYPH_8, GLYPH_9,
  GLYPH_A, GLYPH_B, GLYPH_C, GLYPH_D, GLYPH_E, GLYPH_F, GLYPH_G, GLYPH_H, GLYPH_I, GLYPH_J, GLYPH_K, GLYPH_L, GLYPH_M,
  GLYPH_N, GLYPH_O, GLYPH_P, GLYPH_Q, GLYPH_R, GLYPH_S, GLYPH_T, GLYPH_U, GLYPH_V, GLYPH_W, GLYPH_X, GLYPH_Y, GLYPH_Z
};

const byte SEQ_LEN = sizeof(SEQ) / sizeof(SEQ[0]);

// ------------------------------------------------------------
// SETUP / LOOP
// ------------------------------------------------------------
void setup() {
  pinMode(DATA_PIN_0, OUTPUT);
  pinMode(CLOCK_PIN_0, OUTPUT);
  pinMode(LATCH_PIN_0, OUTPUT);

  pinMode(DATA_PIN_1, OUTPUT);
  pinMode(CLOCK_PIN_1, OUTPUT);
  pinMode(LATCH_PIN_1, OUTPUT);

  Serial.begin(115200);
  allOff();

  Serial.println("8x8 Matrix ready. Showing 0-9, A-Z...");
}

void loop() {
  static byte mode = 0;
  static bool inited = false;

  // Hem göz hem 2 yazı

  if (!inited) {
    startEyes2(5000);  // 5 saniye göz + pupil hareketi
    inited = true;
  }

  if (mode == 0) {
    eyes2Update();
    if (eyes2IsDone()) {
      startScroll("AHMET", 60);
      mode = 1;
    }
    return;
  }

  if (mode == 1) {
    scrollUpdate();
    if (scrollIsDone()) {
      startScroll("BU GUN HAVA 25^C", 60);
      mode = 2;
    }
    return;
  }

  scrollUpdate();
  if (scrollIsDone()) {
    startEyes2(5000);
    mode = 0;
  }

  //Yanliz yazi

  // if (mode == 0) {
  //   scrollUpdate();
  //   if (scrollIsDone()) {
  //     startScroll("AHMET", 60);
  //     mode = 1;
  //   }
  //   return;
  // }

  // if (mode == 1) {
  //   scrollUpdate();
  //   if (scrollIsDone()) {
  //     startScroll("BU GUN HAVA 25^C", 60);
  //     mode = 0;
  //   }
  //   return;
  // }

  // Yanliz eye2

  // if (!inited) {
  //   startEyes2(5000);  // 5 saniye
  //   inited = true;
  // }

  // eyes2Update();

  // if (!inited) {
  //   startEyes2(5000); // 5 saniye
  //   inited = true;
  // }

  // eyes2Update();

  // Yanliz eye

  // if (!inited) {
  //   startBlinkEyes(5000);  // 5 saniye
  //   inited = true;
  // }

  // blinkEyesUpdate();

  // bittiğinde istersen burada başka şey yaparsın
  // if (eyes2IsDone()) { ... }
}
