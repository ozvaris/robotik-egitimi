int potPin = A0;      // Potansiyometre A0'da
int redPin = 9;       // Kırmızı PWM pini
int greenPin = 10;    // Yeşil PWM pini
int bluePin = 11;     // Mavi PWM pini

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  Serial.begin(9600);   // Serial haberleşme başlat
  Serial.println("Potansiyometre PWM Kontrol Basladi");
}

void loop() {
  int potValue = analogRead(potPin);               // 0 - 1023
  int pwmValue = map(potValue, 0, 1023, 0, 255);   // 0 - 255

  analogWrite(redPin, pwmValue);
  analogWrite(greenPin, pwmValue);
  analogWrite(bluePin, pwmValue);

  // Serial çıktılar
  Serial.print("Pot: ");
  Serial.print(potValue);
  Serial.print("  |  PWM: ");
  Serial.println(pwmValue);

  delay(100); // Serial Monitor'u boğmamak için
}

