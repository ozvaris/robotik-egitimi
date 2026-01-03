/*
  2x 74HC595 ile 8x8 segment tarama
  - scanAllRowSegments / scanAllColSegments yapısı korunur
  - mapMasksToOutputs: (rowMask, colMask) -> (register1Out, register2Out)
  - makeByte_0 / makeByte_1 parametreleri: B0..B7

  Varsayım (senin eski mantığınla uyumlu):
  - Row seçimi: ACTIVE-HIGH  (1 => satır aktif)
  - Col seçimi: ACTIVE-LOW   (0 => kolon aktif)

  register1 = 1. 74HC595 (COL tarafı varsayımı)
  register2 = 2. 74HC595 (ROW tarafı varsayımı)

  NOT: Mapping’i sen yapacağın için sadece makeByte_0 / makeByte_1 içini değiştirmen yeterli.
*/

const int DATA_PIN_0  = 9;   // register1 (COL) 74HC595 DS
const int CLOCK_PIN_0 = 10;  // register1 (COL) 74HC595 SHCP
const int LATCH_PIN_0 = 8;   // register1 (COL) 74HC595 STCP

const int DATA_PIN_1  = 12;  // register2 (ROW) 74HC595 DS
const int CLOCK_PIN_1 = 13;  // register2 (ROW) 74HC595 SHCP
const int LATCH_PIN_1 = 11;  // register2 (ROW) 74HC595 STCP

void writeSingle(byte rowMask, byte colMask) ;

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

void allOff() {
  // Mantıksal maske ile kapat (mapping’i içeride uygular)
  writeSingle(0x00, 0xFF);
}


// ------------------------------------------------------------
// MAPPING (BURAYI SEN DOLDURACAKSIN)
// ------------------------------------------------------------

// register2 (ROW) byte üretimi: B_0..B_7
byte makeByte_0(
  byte B_0, byte B_1, byte B_2, byte B_3,
  byte B_4, byte B_5, byte B_6, byte B_7
) {
  byte v = 0;

  // ŞU AN identity: bit0->B_0 ... bit7->B_7
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
  byte B_4, byte B_5, byte B_6, byte B_7
) {
  byte v = 0;

  // ŞU AN identity: bit0->B_0 ... bit7->B_7
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

// ------------------------------------------------------------
// İKİ BYTE AL -> İÇERDE MAPPING -> İKİ REGISTER BYTE ÇIKAR
// ------------------------------------------------------------
void mapMasksToOutputs(byte rowMask, byte colMask, byte &register1Out, byte &register2Out) {
  // rowMask bitleri (B0..B7)  -> satır tarafı (active-high varsayımı)
  byte rB0 = (colMask >> 7) & 0x01; // C8
  byte rB1 = (colMask >> 6) & 0x01; // C7
  byte rB2 = (rowMask >> 1) & 0x01; // R2
  byte rB3 = (colMask >> 0) & 0x01; // C1
  byte rB4 = (rowMask >> 3) & 0x01; // R4
  byte rB5 = (colMask >> 5) & 0x01; // C6
  byte rB6 = (colMask >> 3) & 0x01; // C4
  byte rB7 = (rowMask >> 0) & 0x01; // R1

  // colMask bitleri (B0..B7)  -> kolon tarafı (mask zaten active-low değerleri taşıyor)
  byte cB0 = (rowMask >> 4) & 0x01; // R5
  byte cB1 = (rowMask >> 6) & 0x01; // R7
  byte cB2 = (colMask >> 1) & 0x01; // C2
  byte cB3 = (colMask >> 2) & 0x01; // C3
  byte cB4 = (rowMask >> 7) & 0x01; // R8
  byte cB5 = (colMask >> 4) & 0x01; // C5
  byte cB6 = (rowMask >> 5) & 0x01; // R6
  byte cB7 = (rowMask >> 2) & 0x01; // R3

  // Varsayımsal yön:
  // register2 = satırlar, register1 = kolonlar
  register1Out = makeByte_0(rB0,rB1,rB2,rB3,rB4,rB5,rB6,rB7);
  register2Out = makeByte_1(cB0,cB1,cB2,cB3,cB4,cB5,cB6,cB7);
}

// Dışarıdan iki mask verip direkt yazdırmak için
void writeMasks(byte rowMask, byte colMask) {
  byte reg1, reg2;
  mapMasksToOutputs(rowMask, colMask, reg1, reg2);
  shiftWrite2(reg1, reg2);
}

const byte ROW_MASKS[8] = {
  0b00000001,  // eski [7]
  0b00000010,  // eski [6]
  0b00000100,  // eski [5]
  0b00001000,  // eski [4]
  0b00010000,  // eski [3]
  0b00100000,  // eski [2]
  0b01000000,  // eski [1]
  0b10000000   // eski [0]
};

const byte COL_MASKS[8] = {
  0b11111110,  // eski [7]
  0b11111101,  // eski [6]
  0b11111011,  // eski [5]
  0b11110111,  // eski [4]
  0b11101111,  // eski [3]
  0b11011111,  // eski [2]
  0b10111111,  // eski [1]
  0b01111111   // eski [0]
};




// ------------------------------------------------------------
// Tarama (senin yapı aynı)
// ------------------------------------------------------------
void scanAllRowSegments(uint16_t holdMs) {

  for (int r = 7; r >= 0; r--) {
    for (int c = 7; c >= 0; c--) {
      writeMasks(ROW_MASKS[r], COL_MASKS[c]);
      delay(holdMs);
    }
  }

  allOff();
}


void scanAllColSegments(uint16_t holdMs) {

  for (int c = 7; c >= 0; c--) {
    for (int r = 7; r >= 0; r--) {
      writeMasks(ROW_MASKS[r], COL_MASKS[c]);
      delay(holdMs);
    }
  }

  allOff();
}


// ------------------------------------------------------------
void setup() {
  pinMode(DATA_PIN_0, OUTPUT);
  pinMode(CLOCK_PIN_0, OUTPUT);
  pinMode(LATCH_PIN_0, OUTPUT);

  pinMode(DATA_PIN_1, OUTPUT);
  pinMode(CLOCK_PIN_1, OUTPUT);
  pinMode(LATCH_PIN_1, OUTPUT);

  allOff();
}

const byte singleRowByte   = 0b10000000;
const byte singleColByte   = 0b01111111;

void writeSingle(byte rowMask, byte colMask) {
  byte reg1, reg2;
  mapMasksToOutputs(rowMask, colMask, reg1, reg2);
  shiftWrite2(reg1, reg2);
}

void loop() {
  // Örnek: (11111111, 00000000) -> hepsi aktif gibi (demo amaçlı kısa)
  // writeMasks(0xFF, 0x00);
  // delay(150);
  allOff();
  delay(150);

  writeSingle(singleRowByte, singleColByte);
  delay(1000);

  // scanAllRowSegments(200);
  // scanAllColSegments(200);
}
