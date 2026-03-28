// 74HC595 + 1x 7-segment 0-9 counter
// Varsayım: 7-segment COMMON CATHODE (COM -> GND)
// Segment mapping: QA..QH = A,B,C,D,E,F,G,DP (DP kullanılmıyor)

const int DATA_PIN = 12;   // DS  (pin 14)
const int CLOCK_PIN = 13;  // SH_CP (pin 11)
const int LATCH_PIN = 11;   // ST_CP (pin 12)

// Bit sırası: 0=A,1=B,2=C,3=D,4=E,5=F,6=G,7=DP
// COMMON CATHODE için: 1 = segment yanar
const byte digitsCC[10] = {
  0b00111111,  // 0 -> A B C D E F
  0b00000110,  // 1 -> B C
  0b01011011,  // 2 -> A B D E G
  0b01001111,  // 3 -> A B C D G
  0b01100110,  // 4 -> B C F G
  0b01101101,  // 5 -> A C D F G
  0b01111101,  // 6 -> A C D E F G
  0b00000111,  // 7 -> A B C
  0b01111111,  // 8 -> A B C D E F G
  0b01101111   // 9 -> A B C D F G
};


// Her eleman SADECE 1 segmenti yakar
const byte segmentsCC[8] = {
  0b00000001,  // A
  0b00000010,  // B
  0b00000100,  // C
  0b00001000,  // D
  0b00010000,  // E
  0b00100000,  // F
  0b01000000,  // G
  0b10000000   // DP
};



void shiftWrite(byte value) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, MSBFIRST, value);  // A=bit0 ... DP=bit7
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  shiftWrite(0);  // başlangıçta kapat
}

void loop() {
  for (int n = 0; n <= 9; n++) {
    shiftWrite(digitsCC[n]);
    delay(1000);
  }

  // for (int i = 0; i < 8; i++) {
  //   shiftWrite(segmentsCC[i]);
  //   delay(1000);
  // }
}
