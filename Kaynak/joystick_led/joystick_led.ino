// Joystick pinleri
#define VRX_PIN   A1   // X ekseni
#define VRY_PIN   A0   // Y ekseni
#define SW_PIN    2    // Joystick düğme pini (dijital giriş)

// LED pinleri
#define LED_PIN1  5    // X ekseni LED1 / Sol
#define LED_PIN2  6    // X ekseni LED2
#define LED_PIN3  8    // Y ekseni LED3 / Yukarı
#define LED_PIN4  9    // Y ekseni LED4
#define LED_PIN5  10   // Joystick'e basınca yanıp sönen LED

void setup() {
  pinMode(VRX_PIN, INPUT);
  pinMode(VRY_PIN, INPUT);
  pinMode(SW_PIN, INPUT_PULLUP); // Joystick SW pini, pull-up aktif

  pinMode(LED_PIN1, OUTPUT);
  pinMode(LED_PIN2, OUTPUT);
  pinMode(LED_PIN3, OUTPUT);
  pinMode(LED_PIN4, OUTPUT);
  pinMode(LED_PIN5, OUTPUT);

  Serial.begin(9600);
}

void loop() {
  // Joystick eksenlerini oku
  int xValue = analogRead(VRX_PIN);  
  int yValue = analogRead(VRY_PIN);  
  int swState = digitalRead(SW_PIN); // LOW → basılı
  xValue = 1023-xValue;
  yValue = 1023-yValue;


  // PWM parlaklık hesaplamaları
  int brightness1 = map(xValue, 0, 1023, 0, 255);
  int brightness2 = 255 - brightness1;
  int brightness3 = map(yValue, 0, 1023, 0, 255);
  int brightness4 = 255 - brightness3;

  // LED'leri ayarla
  analogWrite(LED_PIN1, brightness1);
  analogWrite(LED_PIN2, brightness2);
  analogWrite(LED_PIN3, brightness3);
  analogWrite(LED_PIN4, brightness4);

  // Switch'e basılmışsa LED5 yanıp sönsün
  if (swState == LOW) {
    digitalWrite(LED_PIN5, HIGH);
    delay(100);
    digitalWrite(LED_PIN5, LOW);
    delay(100);
  } else {
    digitalWrite(LED_PIN5, LOW); // basılmıyorsa LED5 kapalı
  }

  // Seri monitör için bilgi
  Serial.print("X: ");
  Serial.print(xValue);
  Serial.print(" Y: ");
  Serial.print(yValue);
  Serial.print(" SW: ");
  Serial.println(swState == LOW ? "Pressed" : "Released");
}