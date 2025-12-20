// Common Cathode 7-seg segment test (tek tek yak/söndür)
// Senin pinlerin:
const int a  = 7;
const int b  = 6;
const int c  = 5;
const int d  = 11;
const int e  = 10;
const int f  = 8;
const int g  = 9;
const int dp = 4;

// Sıra: a,b,c,d,e,f,g,dp
const int segPins[8]   = {a, b, c, d, e, f, g, dp};
const char segNames[8] = {'a','b','c','d','e','f','g','p'}; // p = dp

void allOff() {
  for (int i = 0; i < 8; i++) digitalWrite(segPins[i], LOW);  // CC => LOW = OFF
}

void setup() {
  for (int i = 0; i < 8; i++) pinMode(segPins[i], OUTPUT);
  allOff();
  Serial.begin(9600);
}

void loop() {
  allOff();

  for (int i = 0; i < 8; i++) {
    Serial.print("Segment ON: ");
    Serial.println(segNames[i]);

    digitalWrite(segPins[i], HIGH); // CC => HIGH = ON
    delay(600);

    digitalWrite(segPins[i], LOW);
    delay(200);
  }

  delay(800); // tur arası küçük bekleme
}
