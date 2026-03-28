void setup() {
  pinMode(3, OUTPUT); // Dijital pin 2 çıkış olarak ayarlanır
}

void loop() {
  digitalWrite(3, HIGH); // LED'i yak
  delay(1000);            // 500 ms bekle
  digitalWrite(3, LOW);  // LED'i söndür
  delay(1000);            // 500 ms bekle
}
