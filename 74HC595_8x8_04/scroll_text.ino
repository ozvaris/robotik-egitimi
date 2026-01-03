/*
  scroll_text.ino - NON-BLOCKING scroller (8x8)

  Gereksinim:
  - Ana .ino içinde refreshOnce(const byte glyph[8]) var.
  - Ana .ino içinde BIT_LEFT_IS_BIT7 var.
  - Ana .ino içinde GLYPH_* (en azından bu örnekler) var.

  Kullanım:
    startScroll("AHMET", 60);
    loop() içinde sürekli scrollUpdate();
    if (scrollIsDone()) ... (yeniden başlat)
*/

#include <Arduino.h>

// --- Ana dosyadan gelenler ---
extern const bool BIT_LEFT_IS_BIT7;
void refreshOnce(const byte glyph[8]);


// -------------------- yardımcılar --------------------

static inline byte readGlyphBit(byte rowByte, byte visualCol /*0..7 soldan-sağa*/) {
  if (BIT_LEFT_IS_BIT7) return (rowByte >> (7 - visualCol)) & 0x01;
  return (rowByte >> visualCol) & 0x01;
}

static inline void shiftDisplayLeft(byte disp[8], const byte incomingBitPerRow[8]) {
  for (byte r = 0; r < 8; r++) {
    if (BIT_LEFT_IS_BIT7) {
      disp[r] = (byte)((disp[r] << 1) | (incomingBitPerRow[r] & 0x01));
    } else {
      disp[r] = (byte)((disp[r] >> 1) | ((incomingBitPerRow[r] & 0x01) << 7));
    }
  }
}

static inline void incomingBitsFromGlyphCol(const byte* glyph, byte visualCol, byte outBits[8]) {
  for (byte row = 0; row < 8; row++) {
    outBits[row] = readGlyphBit(glyph[row], visualCol);
  }
}

// -------------------- NON-BLOCKING SCROLL STATE --------------------

static const char* g_text = nullptr;
static uint16_t g_stepMs = 60;
static uint32_t g_nextStepAt = 0;

static byte g_disp[8] = {0,0,0,0,0,0,0,0};

// Durum makinesi
// phase 0: lead-in boşluk (8 kolon)
// phase 1: char kolonları (8 kolon) + phase 2: char arası 1 boş kolon
// phase 3: tail-out boşluk (8 kolon)
// phase 4: done
static byte g_phase = 4;
static byte g_leadRemaining = 0;
static byte g_tailRemaining = 0;

static const char* g_p = nullptr;     // text pointer
static const byte* g_glyph = nullptr; // current glyph
static byte g_col = 0;                // current glyph column 0..7

static void resetDisp() {
  for (byte i = 0; i < 8; i++) g_disp[i] = 0;
}

void startScroll(const char* text, uint16_t stepMs) {
  g_text = text;
  g_stepMs = stepMs;
  g_nextStepAt = millis(); // hemen başla

  resetDisp();

  g_phase = 0;
  g_leadRemaining = 8;
  g_tailRemaining = 8;

  g_p = g_text;
  g_glyph = nullptr;
  g_col = 0;
}

bool scrollIsDone() {
  return g_phase == 4;
}

static void doOneStep() {
  // 1 kolon kaydırma adımı üret
  byte incoming[8] = {0,0,0,0,0,0,0,0};

  if (g_phase == 0) {
    // lead-in: sadece boş kolonlar
    shiftDisplayLeft(g_disp, incoming);
    if (--g_leadRemaining == 0) {
      g_phase = 1;
    }
    return;
  }

  if (g_phase == 1) {
    // bir karakterin 8 kolonunu akıt
    if (!g_glyph) {
      // yeni karakter al
      if (!g_p || *g_p == '\0') {
        g_phase = 3; // metin bitti -> tail-out
        return;
      }
      g_glyph = glyphForChar(*g_p);
      g_col = 0;
    }

    incomingBitsFromGlyphCol(g_glyph, g_col, incoming);
    shiftDisplayLeft(g_disp, incoming);

    g_col++;
    if (g_col >= 8) {
      // karakter bitti
      g_glyph = nullptr;
      g_p++;          // sonraki char
      g_phase = 2;    // araya 1 boş kolon
    }
    return;
  }

  if (g_phase == 2) {
    // karakter arası 1 boş kolon
    shiftDisplayLeft(g_disp, incoming);
    g_phase = 1;
    return;
  }

  if (g_phase == 3) {
    // tail-out: 8 boş kolon
    shiftDisplayLeft(g_disp, incoming);
    if (--g_tailRemaining == 0) {
      g_phase = 4; // done
    }
    return;
  }
}

void scrollUpdate() {
  // Ekranı sürekli canlı tut (en kritik nokta)
  refreshOnce(g_disp);

  // Bitti mi? bitince sadece sabit görüntü refresh edilir (istersen boş yapabilirsin)
  if (g_phase == 4) return;

  // Zamanı geldiyse 1 adım kaydır
  uint32_t now = millis();
  if ((int32_t)(now - g_nextStepAt) >= 0) {
    doOneStep();
    g_nextStepAt = now + g_stepMs;
  }
}
