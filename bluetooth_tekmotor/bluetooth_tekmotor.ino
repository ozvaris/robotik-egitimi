#include <SoftwareSerial.h>

// ==================================================
// DEFINES
// ==================================================
#define CMD_FORWARD   'F'
#define CMD_BACKWARD  'B'
#define CMD_STOP      'S'

// Hız komutları: '1'..'9'
#define CMD_SPEED_MIN '1'
#define CMD_SPEED_MAX '9'

// 1..9 -> PWM ölçekleme
#define PWM_MIN  60
#define PWM_MAX  255

// ==================================================
// Tek Motor pinleri
// ==================================================
const int enA = 10;  // PWM
const int in1 = 8;
const int in2 = 9;

// ==================================================
// Bluetooth Serial (HC-05/HC-06)
// UNO: BT_TX -> D2 (Arduino RX), BT_RX -> D3 (Arduino TX)
// ==================================================
const int BT_RX_PIN = 2;
const int BT_TX_PIN = 3;
SoftwareSerial BT(BT_RX_PIN, BT_TX_PIN);

// ✅ Başlangıç: Stop + hız=1
char lastMotionCmd = CMD_STOP;
int  speedMult     = 1;   // 1..9

int pwmFromMult(int mult) {
  // mult 1..9 dışına çıkarsa kıs
  if (mult < 1) mult = 1;
  if (mult > 9) mult = 9;

  // 1..9 -> PWM_MIN..PWM_MAX (lineer)
  long pwm = PWM_MIN + (long)(mult - 1) * (PWM_MAX - PWM_MIN) / 8;
  return (int)constrain(pwm, 0, 255);
}

void setup() {
  Serial.begin(115200);
  BT.begin(9600);

  pinMode(enA, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);

  motorDur(); // açılışta çalışmasın

  Serial.println("Tek Motor BT Kontrol Hazir.");
  Serial.println("Komutlar: F=Ileri, B=Geri, S=Dur, 1..9=Hiz");
  Serial.println("Baslangic: lastMotionCmd='S' (stop), hiz=1");
}

void loop() {
  if (BT.available())  handleCommand(BT.read(), "BT");

  // Serial Monitor'dan test
  if (Serial.available()) {
    char c = Serial.read();
    if (c != '\n' && c != '\r') handleCommand(c, "SERIAL");
  }
}

void handleCommand(char c, const char* src) {
  Serial.print("["); Serial.print(src); Serial.print("] '");
  Serial.print(c); Serial.println("'");

  // Hız ayarı
  if (c >= CMD_SPEED_MIN && c <= CMD_SPEED_MAX) {
    speedMult = c - '0'; // 1..9
    Serial.print("-> Hiz (1..9) = "); Serial.println(speedMult);

    // motor gidiyorsa anında uygula
    int pwm = pwmFromMult(speedMult);
    if (lastMotionCmd == CMD_FORWARD) {
      motorIleri(pwm);
      Serial.print("-> (aninda) ILERI pwm="); Serial.println(pwm);
    } else if (lastMotionCmd == CMD_BACKWARD) {
      motorGeri(pwm);
      Serial.print("-> (aninda) GERI pwm="); Serial.println(pwm);
    } else {
      Serial.println("-> Motor duruyor. Calismasi icin F veya B gonder.");
    }
    return;
  }

  // Hareket komutları
  switch (c) {
    case CMD_FORWARD: {
      lastMotionCmd = CMD_FORWARD;
      int pwm = pwmFromMult(speedMult);
      motorIleri(pwm);
      Serial.print("-> ILERI pwm="); Serial.println(pwm);
    } break;

    case CMD_BACKWARD: {
      lastMotionCmd = CMD_BACKWARD;
      int pwm = pwmFromMult(speedMult);
      motorGeri(pwm);
      Serial.print("-> GERI pwm="); Serial.println(pwm);
    } break;

    case CMD_STOP:
      lastMotionCmd = CMD_STOP;
      motorDur();
      Serial.println("-> DUR");
      break;

    default:
      Serial.println("-> Bilinmeyen. F,B,S,1..9");
      break;
  }
}

// ------------------------------
// Motor fonksiyonları
// ------------------------------
void motorIleri(int hiz) {
  hiz = constrain(hiz, 0, 255);
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  analogWrite(enA, hiz);
}

void motorGeri(int hiz) {
  hiz = constrain(hiz, 0, 255);
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  analogWrite(enA, hiz);
}

void motorDur() {
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  analogWrite(enA, 0);
}
