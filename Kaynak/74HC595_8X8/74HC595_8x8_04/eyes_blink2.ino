/*
  eyes_blink2.ino - NON-BLOCKING göz bebeği kaydırma (sol/orta/sağ) + blink

  Gereksinim:
  - Ana .ino içinde refreshOnce(const byte glyph[8]) var.
*/

#include <Arduino.h>
void refreshOnce(const byte glyph[8]);

// ------------------------------------------------------------
// Frame üretimi: 8x8 içinde iki göz (sclera + pupil)
// Pupilleri 3 pozisyon: LEFT / CENTER / RIGHT
// Blink fazları: OPEN -> HALF -> CLOSED -> HALF -> OPEN
// ------------------------------------------------------------

enum EyeLook : byte { LOOK_LEFT = 0,
                      LOOK_CENTER = 1,
                      LOOK_RIGHT = 2 };

static byte g_frame[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
static EyeLook g_look = LOOK_CENTER;

// blink state
static bool g_blinking = false;
static byte g_blinkPhase = 0;  // 0 open,1 half,2 closed,3 half->open
static uint32_t g_nextAt = 0;
static uint32_t g_endAt_eye2 = 0;
static bool g_running_eye2 = false;

// Timing (ms) — istediğin gibi oynat
static uint16_t T_LOOK_HOLD_MIN = 350;  // bir bakışın en az süresi
static uint16_t T_LOOK_HOLD_MAX = 950;  // bir bakışın en çok süresi

static uint16_t T_HALF_eye2 = 60;
static uint16_t T_CLOSED = 120;

// Blink aralığı (ms) (rastgele)
static uint16_t T_BLINK_MIN = 1200;
static uint16_t T_BLINK_MAX = 3200;

static uint32_t g_nextBlinkAt = 0;

// ------------------------------------------------------------
// Basit yardımcı: random aralık
// ------------------------------------------------------------
static inline uint16_t rnd16(uint16_t a, uint16_t b) {
  // [a, b]
  if (b <= a) return a;
  return (uint16_t)(a + (uint16_t)random((long)(b - a + 1)));
}

// ------------------------------------------------------------
// Frame builder: göz şekli + pupil pozisyonu + blink maskesi
// 1 = ON varsayımı (refreshOnce içinde senin polarite çözülüyor)
// ------------------------------------------------------------
static void buildEyesFrame(EyeLook look, byte blinkLevel /*0=open 1=half 2=closed*/) {
  // temizle
  for (byte r = 0; r < 8; r++) g_frame[r] = 0;

  // col: 0..7 (0=sol), row: 0..7 (0=üst)
  auto setPix = [&](byte row, byte col) {
    if (row > 7 || col > 7) return;
    g_frame[row] |= (byte)(1 << (7 - col));  // col0 -> bit7
  };

  auto drawHLine = [&](byte row, byte c0, byte c1) {
    for (byte c = c0; c <= c1; c++) setPix(row, c);
  };

  // Göz bölgemiz: satır 2..5
  const byte rTop = 2;
  const byte rMid1 = 3;
  const byte rMid2 = 4;
  const byte rBot = 5;

  // Sol göz: col 0..3  |  Sağ göz: col 4..7
  const byte L0 = 0, L1 = 3;
  const byte R0 = 4, R1 = 7;

  if (blinkLevel == 2) {
    // CLOSED: iki göz için tek çizgi (rMid2)
    drawHLine(rMid2, L0, L1);
    drawHLine(rMid2, R0, R1);
    return;
  }

  if (blinkLevel == 1) {
    // HALF: iki göz için iki satır çizgi (rMid1 ve rMid2)
    drawHLine(rMid1, L0, L1);
    drawHLine(rMid2, L0, L1);
    drawHLine(rMid1, R0, R1);
    drawHLine(rMid2, R0, R1);

    // Pupilleri half'ta da çiz (isteğe bağlı)
  } else {
    // OPEN: outline (oval gibi)
    // üst/alt çizgi (orta kısım dolu)
    drawHLine(rTop, L0 + 1, L1 - 1);  // sol göz üst
    drawHLine(rBot, L0 + 1, L1 - 1);  // sol göz alt
    drawHLine(rTop, R0 + 1, R1 - 1);  // sağ göz üst
    drawHLine(rBot, R0 + 1, R1 - 1);  // sağ göz alt

    // yan kenarlar
    setPix(rMid1, L0);
    setPix(rMid2, L0);
    setPix(rMid1, L1);
    setPix(rMid2, L1);
    setPix(rMid1, R0);
    setPix(rMid2, R0);
    setPix(rMid1, R1);
    setPix(rMid2, R1);
  }

  // Pupiller (blinkLevel 0 veya 1 iken)
  // 4 kolonluk göz içinde pupil 3 pozisyon:
  // LEFT:  sol göz col1, sağ göz col5
  // CENTER: sol göz col2, sağ göz col6
  // RIGHT: sol göz col3, sağ göz col7 (ama outline ile çakışmasın diye sınırları dikkat)
  byte pL, pR;
  if (look == LOOK_LEFT) {
    pL = 1;  // sol göz içi
    pR = 5;  // sağ göz içi
  } else if (look == LOOK_RIGHT) {
    // RIGHT'ta pupil'i outline'a bindirmemek için içte tutalım:
    pL = 2;  // (3 yerine 2 daha güvenli)
    pR = 6;
  } else {
    pL = 2;
    pR = 6;
  }

  // pupil satırı (biraz aşağı iyi durur)
  byte pr = (blinkLevel == 1) ? rMid2 : rMid2;
  setPix(pr, pL);
  setPix(pr, pR);
}


// ------------------------------------------------------------
// Dış API
// ------------------------------------------------------------

// totalMs kadar “göz animasyonu” çalışsın.
// totalMs=0 verirsen sürekli çalışır (bitmez)
void startEyes2(uint16_t totalMs) {
  g_running_eye2 = true;
  g_blinking = false;
  g_blinkPhase = 0;
  g_look = LOOK_CENTER;

  uint32_t now = millis();
  g_nextAt = now + rnd16(T_LOOK_HOLD_MIN, T_LOOK_HOLD_MAX);

  if (totalMs == 0) g_endAt_eye2 = 0;
  else g_endAt_eye2 = now + totalMs;

  g_nextBlinkAt = now + rnd16(T_BLINK_MIN, T_BLINK_MAX);

  // rastgele için seed iyi olsun (istersen analogRead(A0) vs ile seed ver)
  buildEyesFrame(g_look, 0);
}

bool eyes2IsDone() {
  return !g_running_eye2;
}

static void beginBlink(uint32_t now) {
  g_blinking = true;
  g_blinkPhase = 1;  // half
  g_nextAt = now + T_HALF_eye2;
  buildEyesFrame(g_look, 1);
}

static void advanceBlink(uint32_t now) {
  // phase: 1 half -> 2 closed -> 3 half -> 0 open (stop blink)
  if (g_blinkPhase == 1) {
    g_blinkPhase = 2;
    g_nextAt = now + T_CLOSED;
    buildEyesFrame(g_look, 2);
  } else if (g_blinkPhase == 2) {
    g_blinkPhase = 3;
    g_nextAt = now + T_HALF_eye2;
    buildEyesFrame(g_look, 1);
  } else {
    // back to open
    g_blinkPhase = 0;
    g_blinking = false;
    g_nextAt = now + rnd16(T_LOOK_HOLD_MIN, T_LOOK_HOLD_MAX);
    buildEyesFrame(g_look, 0);
  }
}

static void changeLook(uint32_t now) {
  // sol/orta/sağ arasında rastgele değiştir (aynısı gelirse bir daha dene)
  EyeLook prev = g_look;
  EyeLook next = (EyeLook)random(0, 3);
  if (next == prev) next = (EyeLook)((prev + 1) % 3);
  g_look = next;

  buildEyesFrame(g_look, 0);
  g_nextAt = now + rnd16(T_LOOK_HOLD_MIN, T_LOOK_HOLD_MAX);
}

void eyes2Update() {
  // Görüntüyü sürekli canlı tut
  refreshOnce(g_frame);

  if (!g_running_eye2) return;

  uint32_t now = millis();

  // süre doldu mu? (0 ise sonsuz)
  if (g_endAt_eye2 != 0 && (int32_t)(now - g_endAt_eye2) >= 0) {
    g_running_eye2 = false;
    return;
  }

  // Blink zamanı geldi mi? (blink başlamadıysa başlat)
  if (!g_blinking && (int32_t)(now - g_nextBlinkAt) >= 0) {
    beginBlink(now);
    // bir sonraki blink planı
    g_nextBlinkAt = now + rnd16(T_BLINK_MIN, T_BLINK_MAX);
    return;
  }

  // Genel “step” zamanı geldi mi?
  if ((int32_t)(now - g_nextAt) < 0) return;

  if (g_blinking) {
    advanceBlink(now);
  } else {
    changeLook(now);
  }
}
