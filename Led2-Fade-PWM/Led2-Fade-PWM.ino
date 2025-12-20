const int ledPin = 3;        // LED pin

void setup() {
  pinMode(ledPin, OUTPUT); // Pin 9'u çıkış olarak ayarla
}

void loop() {
  // LED'in parlaklığını artır (fade in)
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    analogWrite(ledPin, brightness); // PWM ile parlaklığı ayarla
    delay(30); // 30 milisaniye bekle
  }

  // LED'in parlaklığını azalt (fade out)
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    analogWrite(ledPin, brightness);
    delay(30); 
  }
}
