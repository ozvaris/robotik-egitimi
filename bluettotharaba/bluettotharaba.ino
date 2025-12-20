// Bluetooth komutları
#define FORWARD 'F'
#define BACKWARD 'B'
#define LEFT 'L'
#define RIGHT 'R'
#define START 'A'
#define PAUSE 'P'

// Motor A (Sol)
const int enA = 5;
const int in1 = 6;
const int in2 = 7;

// Motor B (Sağ)
const int enB = 10;
const int in3 = 8;
const int in4 = 9;

// Kontrol değişkeni
bool motorAktif = false;

void setup() {
  Serial.begin(9600);

  // Motor pinleri çıkış olarak ayarlanıyor
  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  
  pinMode(enB, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  // Başlangıçta motorlar kapalı
  tumMotorlariDur();
}

void loop() {
  if (Serial.available()) {
    char command = Serial.read();
    executeCommand(command);
  }
}

// ------------------------------
// Gelen komutları işleyici
// ------------------------------
void executeCommand(char command) {
  switch (command) {
    case START:
      motorAktif = true;
      break;

    case PAUSE:
      motorAktif = false;
      tumMotorlariDur();
      break;

    case FORWARD:
      if (motorAktif) {
        solMotorIleri(200);
        sagMotorIleri(200);
      }
      break;

    case BACKWARD:
      if (motorAktif) {
        solMotorGeri(200);
        sagMotorGeri(200);
      }
      break;

    case LEFT:
      if (motorAktif) {
        solMotorGeri(150);
        sagMotorIleri(150);
      }
      break;

    case RIGHT:
      if (motorAktif) {
        solMotorIleri(150);
        sagMotorGeri(150);
      }
      break;
  }
}

// ------------------------------
// Motor Fonksiyonları
// ------------------------------
void solMotorIleri(int hiz) {
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, hiz);
}

void solMotorGeri(int hiz) {
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, hiz);
}

void solMotorDur() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
}

void sagMotorIleri(int hiz) {
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
  analogWrite(enB, hiz);
}

void sagMotorGeri(int hiz) {
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
  analogWrite(enB, hiz);
}

void sagMotorDur() {
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
  analogWrite(enB, 0);
}

void tumMotorlariDur() {
  solMotorDur();
  sagMotorDur();
}
