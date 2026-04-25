#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);


void setup() {
  // put your setup code here, to run once:
  Wire.begin();

  lcd.init();
  lcd.backlight();

  // 1. Satır Merhaba yaz
  lcd.setCursor(0, 0);
  lcd.print("Merhaba!");

  lcd.setCursor(0, 1);


  pinMode(LED_BUILTIN, OUTPUT);
  // pinMode(13, OUTPUT);

  Serial.begin(115200);
  while (!Serial) {}

  Serial.println("\Setup Basladi");

}

void loop() {
  int pos = 0;
  int stepCount = 0;
  int ledState = LOW;

  Serial.println("\loop Basladi");


  while (pos <= 15) {

    Serial.println("\while1 Basladi");

    lcd.clear();
    lcd.setCursor(pos, 0);
    lcd.print("Merhaba");

    delay(500);

    pos++;
    stepCount++;

    if (stepCount == 2) {
      if (ledState == LOW) {
        ledState = HIGH;
      } else {
        ledState = LOW;
      }

      
      digitalWrite(LED_BUILTIN, ledState);
      stepCount = 0;
    }


    // delay(1000); 
    // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level) 
    // delay(1000); 
    // digitalWrite(LED_BUILTIN, LOW); // turn the LED on (HIGH is the voltage level)

  }

  Serial.println("\loop Bitti");
  
}

