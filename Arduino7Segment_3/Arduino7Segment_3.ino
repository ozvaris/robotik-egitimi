// Common Cathode 7-seg 0-9 counter (senin pinlerinle)

// Pin mapping
const int a  = 7;
const int b  = 6;
const int c  = 5;
const int d  = 11;
const int e  = 10;
const int f  = 8;
const int g  = 9;
const int dp = 4;

// Sıra: a,b,c,d,e,f,g,dp
const int segPins[8] = {a, b, c, d, e, f, g, dp};

// CC için: 1 = ON (HIGH), 0 = OFF (LOW)
const byte digits[10][8] = {
  //a b c d e f g dp
  {1,1,1,1,1,1,0,0}, //0
  {0,1,1,0,0,0,0,0}, //1
  {1,1,0,1,1,0,1,0}, //2
  {1,1,1,1,0,0,1,0}, //3
  {0,1,1,0,0,1,1,0}, //4
  {1,0,1,1,0,1,1,0}, //5
  {1,0,1,1,1,1,1,0}, //6
  {1,1,1,0,0,0,0,0}, //7
  {1,1,1,1,1,1,1,0}, //8
  {1,1,1,1,0,1,1,0}  //9
};

void showDigit(int n) {
  for (int s = 0; s < 8; s++) {
    digitalWrite(segPins[s], digits[n][s] ? HIGH : LOW);
  }
}

void setup() {
  for (int i = 0; i < 8; i++) pinMode(segPins[i], OUTPUT);
}

void loop() {
  for (int n = 0; n <= 9; n++) {
    showDigit(n);
    delay(1000);
  }
}
