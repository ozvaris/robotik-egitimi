const int ledPin = 3;        // PWM destekli LED pini
const int buttonPin = 4;     // Buton pini

bool fadeOutNext = false;    // Sıradaki fade ne olacak?

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    // Butona basıldıysa sadece o an çalış

    // Fade in (aç)
    for (int i = 0; i <= 255; i += 3) {
      analogWrite(ledPin, i);
      delay(10);
    }
    // Fade out (kapat)
    for (int i = 255; i >= 0; i -= 3) {
      analogWrite(ledPin, i);
      delay(10);
    }

  }
}
