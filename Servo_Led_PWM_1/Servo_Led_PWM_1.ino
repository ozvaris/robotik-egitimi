#include <Servo.h>

const int ledPin = 3;
const int buttonPin = 4;
const int servoPin = 5;

Servo myservo;

void setup() {
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  myservo.attach(servoPin);  // Servo pinini bağla
}

void loop() {
  if (digitalRead(buttonPin) == LOW) {
    // LED fade in + servo ileri
    for (int i = 0; i <= 255; i += 3) {
      analogWrite(ledPin, i);
      int angle = map(i, 0, 255, 0, 180); // LED parlaklığına göre açı belirle
      myservo.write(angle);
      delay(10);
    }

    // LED fade out + servo geri
    for (int i = 255; i >= 0; i -= 3) {
      analogWrite(ledPin, i);
      int angle = map(i, 0, 255, 0, 180);
      myservo.write(angle);
      delay(10);
    }
  }
}
