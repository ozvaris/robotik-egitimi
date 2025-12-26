// 74HC595 + 1x 7-segment 0-9 counter
// Varsayım: 7-segment COMMON CATHODE (COM -> GND)
// Segment mapping: QA..QH = A,B,C,D,E,F,G,DP (DP kullanılmıyor)

const int DATA_PIN = 12;   // DS  (pin 14)
const int CLOCK_PIN = 13;  // SH_CP (pin 11)
const int LATCH_PIN = 11;   // ST_CP (pin 12)


// 2 - R2
// 1 - R1
// 3 - R3
// 0 - R4


// 7 - C1
// 5 - C2
// 6 - C3
// 4 - C4 


const byte singlebyte   = 0b00101110;





void shiftWrite(byte value) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, value);  // A=bit0 ... DP=bit7
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);

  shiftWrite(0);  // başlangıçta kapat
}

void loop() {

  shiftWrite(singlebyte);
  delay(1000);
  

}
