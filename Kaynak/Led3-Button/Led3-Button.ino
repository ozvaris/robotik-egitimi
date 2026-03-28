const int ledPin = 3;        // LED pin
const int buttonPin = 4;     // Buton pin

void setup() {
  pinMode(ledPin, OUTPUT);             // LED pini çıkış
  pinMode(buttonPin, INPUT_PULLUP);    // Buton pini pull-up ile giriş
}

void loop() {
  int buttonState = digitalRead(buttonPin); // Buton durumu okunur

  if (buttonState == LOW) { // Butona basıldıysa (LOW = basılı)
    digitalWrite(ledPin, LOW); // LED yanar
  } else {
    digitalWrite(ledPin, HIGH);  // LED söner
  }
}
