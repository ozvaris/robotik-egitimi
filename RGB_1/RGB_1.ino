int redPin = 9;
int greenPin = 10;
int bluePin = 11;

void setup() {
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);

  fullOff();
  delay(5000); 

}

void fullOn() {
  setFull(true);
}

void fullOff() {
  setFull(false);
}

void setFull(bool on) {
  int v = on ? 255 : 0;
  analogWrite(redPin, v);
  analogWrite(greenPin, v);
  analogWrite(bluePin, v);
}

enum Color { RED, GREEN, BLUE };

void fadeColor(Color c, bool increase, int stepDelay = 10) {
  int start = increase ? 0   : 255;
  int end   = increase ? 255 : 0;
  int step  = increase ? 1   : -1;

  // önce hepsini kapat
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);

  for (int v = start; v != end + step; v += step) {
    switch (c) {
      case RED:   analogWrite(redPin, v); break;
      case GREEN: analogWrite(greenPin, v); break;
      case BLUE:  analogWrite(bluePin, v); break;
    }
    delay(stepDelay);
  }
}


void fadeFullColor(bool increase, int stepDelay = 10) {
  int start = increase ? 0   : 255;
  int end   = increase ? 255 : 0;
  int step  = increase ? 1   : -1;

  // önce hepsini kapat
  analogWrite(redPin, 0);
  analogWrite(greenPin, 0);
  analogWrite(bluePin, 0);

  for (int v = start; v != end + step; v += step) {
    analogWrite(redPin, v); 
    analogWrite(greenPin, v); 
    analogWrite(bluePin, v);

    delay(stepDelay);
  }
}

void loop() {

  fadeColor(RED, true);    // kırmızı 0 → 255 yavaşça artar
  delay(1000);

  fadeColor(RED, false);    // kırmızı 0 → 255 yavaşça artar
  delay(1000);

  fadeColor(GREEN, true);    // kırmızı 0 → 255 yavaşça artar
  delay(1000);

  fadeColor(GREEN, false);    // kırmızı 0 → 255 yavaşça artar
  delay(1000);

  fadeColor(BLUE, true);    // kırmızı 0 → 255 yavaşça artar
  delay(1000);

  fadeColor(BLUE, false);    // kırmızı 0 → 255 yavaşça artar
  delay(1000);


  fadeFullColor(true);
  delay(1000);

  fadeFullColor(false);
  delay(1000);




}

