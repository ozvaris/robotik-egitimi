/*
  eyes_blink.ino - NON-BLOCKING göz kırpma animasyonu (8x8)

  Gereksinim:
  - Ana .ino içinde refreshOnce(const byte glyph[8]) var.
  - Ana .ino içinde BIT_LEFT_IS_BIT7 var. (burada direkt gerek yok ama projende duruyor)
*/

#include <Arduino.h>

// Ana dosyadan gelen refresh
void refreshOnce(const byte glyph[8]);

// ------------------- Göz frame'leri -------------------
// 1 = ON varsayımı. (Senin sistemde kolon active-low olabilir; o zaten refreshOnce içinde çözülüyor.)
// Bu şekiller basit "iki göz" gibi görünür. İstersen daha artistik yaparız.

const byte EYES_OPEN[8] = {
  0b00000000,
  0b00100100,
  0b01111110,
  0b01011010,
  0b01111110,
  0b00100100,
  0b00000000,
  0b00000000
};

const byte EYES_HALF[8] = {
  0b00000000,
  0b00000000,
  0b01111110,
  0b01011010,
  0b01111110,
  0b00000000,
  0b00000000,
  0b00000000
};

const byte EYES_CLOSED[8] = {
  0b00000000,
  0b00000000,
  0b00000000,
  0b01100110,
  0b01100110,
  0b00000000,
  0b00000000,
  0b00000000
};

// ------------------- Non-blocking anim state -------------------

static const byte* g_eyeFrame = EYES_OPEN;

static uint32_t g_nextChangeAt = 0;
static uint32_t g_endAt = 0;
static bool g_running = false;

// blink sırası: OPEN -> HALF -> CLOSED -> HALF -> OPEN (ve aralarda bekleme)
static byte g_eye_phase = 0;

// Süreler (ms) — zevkine göre oynarsın
static uint16_t T_OPEN  = 900;
static uint16_t T_HALF  = 70;
static uint16_t T_CLOSE = 120;

// Animasyonu başlat: kaç ms boyunca gözler “yaşasın”
void startBlinkEyes(uint16_t totalMs) {
  g_running = true;
  g_eye_phase = 0;
  g_eyeFrame = EYES_OPEN;

  uint32_t now = millis();
  g_nextChangeAt = now + T_OPEN;
  g_endAt = now + totalMs;
}

// Bitti mi?
bool blinkEyesIsDone() {
  return !g_running;
}

// Her loop turunda çağır
void blinkEyesUpdate() {
  // Görüntüyü her tur refresh et (en önemli kısım)
  refreshOnce((const byte*)g_eyeFrame);

  if (!g_running) return;

  uint32_t now = millis();

  // total süre doldu mu?
  if ((int32_t)(now - g_endAt) >= 0) {
    g_running = false;
    g_eyeFrame = EYES_OPEN; // biterken açıkta bırak
    return;
  }

  // Frame değiştirme zamanı mı?
  if ((int32_t)(now - g_nextChangeAt) < 0) return;

  // Faz akışı
  switch (g_eye_phase) {
    case 0: // OPEN bekledi -> HALF
      g_eyeFrame = EYES_HALF;
      g_nextChangeAt = now + T_HALF;
      g_eye_phase = 1;
      break;

    case 1: // HALF -> CLOSED
      g_eyeFrame = EYES_CLOSED;
      g_nextChangeAt = now + T_CLOSE;
      g_eye_phase = 2;
      break;

    case 2: // CLOSED -> HALF
      g_eyeFrame = EYES_HALF;
      g_nextChangeAt = now + T_HALF;
      g_eye_phase = 3;
      break;

    default: // HALF -> OPEN (ve yeniden OPEN bekleme)
      g_eyeFrame = EYES_OPEN;
      g_nextChangeAt = now + T_OPEN;
      g_eye_phase = 0;
      break;
  }
}
