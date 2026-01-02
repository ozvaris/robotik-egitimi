/*
  scroll_text.ino
  - 8x8 matrix için kayan yazı
  - Ana .ino'daki showGlyph/refreshOnce/GLYPH_* tanımlarını kullanır.
*/

// Ana dosyadan gelen ayar ve fonksiyonlar (Arduino otomatik prototip çıkarır)
extern const bool BIT_LEFT_IS_BIT7;
void showGlyph(const byte *glyph, uint16_t ms);

// Ana dosyadan gelen glyph'ler (kullandıklarımız)
extern const byte GLYPH_SPACE[8];
extern const byte GLYPH_A[8];
extern const byte GLYPH_H[8];
extern const byte GLYPH_M[8];
extern const byte GLYPH_E[8];
extern const byte GLYPH_T[8];

// -------------------- yardımcılar --------------------

static inline byte readGlyphBit(byte rowByte, byte visualCol /*0..7 soldan-sağa*/) {
  // BIT_LEFT_IS_BIT7: sol=bit7, sağ=bit0
  if (BIT_LEFT_IS_BIT7) {
    return (rowByte >> (7 - visualCol)) & 0x01;
  } else {
    return (rowByte >> visualCol) & 0x01;
  }
}

static inline void shiftDisplayLeft(byte disp[8], const byte incomingBitPerRow[8]) {
  // incomingBitPerRow[row] = 0/1 (sağdan eklenecek bit)
  for (byte r = 0; r < 8; r++) {
    if (BIT_LEFT_IS_BIT7) {
      // sol=MSB(bit7) => sola kaydır = <<1, sağdan bit0'a ekle
      disp[r] = (byte)((disp[r] << 1) | (incomingBitPerRow[r] & 0x01));
    } else {
      // sol=LSB(bit0) => sola kaydır = >>1, sağdan bit7'ye ekle
      disp[r] = (byte)((disp[r] >> 1) | ((incomingBitPerRow[r] & 0x01) << 7));
    }
  }
}

static const byte* glyphForChar(char c) {
  switch (c) {
    case 'A': return GLYPH_A;
    case 'B': return GLYPH_B;   // eğer ana dosyada yoksa sil
    case 'C': return GLYPH_C;   // eğer ana dosyada yoksa sil
    case 'D': return GLYPH_D;   // eğer ana dosyada yoksa sil
    case 'E': return GLYPH_E;
    case 'G': return GLYPH_G;   // eğer ana dosyada yoksa sil
    case 'H': return GLYPH_H;
    case 'I': return GLYPH_I;   // eğer ana dosyada yoksa sil
    case 'M': return GLYPH_M;
    case 'N': return GLYPH_N;   // eğer ana dosyada yoksa sil
    case 'U': return GLYPH_U;   // eğer ana dosyada yoksa sil
    case 'V': return GLYPH_V;   // eğer ana dosyada yoksa sil
    case 'T': return GLYPH_T;
    case ' ': return GLYPH_SPACE;

    case '0': return GLYPH_0;
    case '1': return GLYPH_1;
    case '2': return GLYPH_2;
    case '3': return GLYPH_3;
    case '4': return GLYPH_4;
    case '5': return GLYPH_5;
    case '6': return GLYPH_6;
    case '7': return GLYPH_7;
    case '8': return GLYPH_8;
    case '9': return GLYPH_9;

    // Derece işareti için özel karakter:
    // Metinde bunu kullanmak için '^' yazacağız (çünkü C string içinde '°' bazen sorun çıkarıyor)
    case '^': return GLYPH_DEGREE;

    default:  return GLYPH_SPACE;
  }
}


// Bir glyph’in belirli bir kolonunu (soldan-sağa visualCol) display’e “sağdan” enjekte edecek bitleri üret
static inline void buildIncomingBitsFromGlyphColumn(const byte* glyph, byte visualCol, byte incomingBits[8]) {
  for (byte row = 0; row < 8; row++) {
    incomingBits[row] = readGlyphBit(glyph[row], visualCol);
  }
}

// -------------------- ana fonksiyon: scrollText --------------------
// speedMs: her kolon kaydırma adımı arası süre (ms)
void scrollText(const char* text, uint16_t speedMs) {
  byte disp[8] = {0,0,0,0,0,0,0,0}; // ekranda görünen 8x8 (row byte)

  // Baştaki boşluk: yazı sağdan gelsin diye 8 kolon boş kaydır
  for (byte i = 0; i < 8; i++) {
    byte zeros[8] = {0,0,0,0,0,0,0,0};
    shiftDisplayLeft(disp, zeros);
    showGlyph(disp, speedMs);
  }

  // Metindeki her karakteri 8 kolon olarak akıt
  for (const char* p = text; *p; p++) {
    const byte* g = glyphForChar(*p);

    // 8 kolon: 0..7 soldan sağa
    for (byte col = 0; col < 8; col++) {
      byte incoming[8];
      buildIncomingBitsFromGlyphColumn(g, col, incoming);
      shiftDisplayLeft(disp, incoming);
      showGlyph(disp, speedMs);
    }

    // karakterler arası 1 kolon boşluk
    {
      byte zeros[8] = {0,0,0,0,0,0,0,0};
      shiftDisplayLeft(disp, zeros);
      showGlyph(disp, speedMs);
    }
  }

  // Sonda yazı tamamen çıksın diye 8 kolon daha boş kaydır
  for (byte i = 0; i < 8; i++) {
    byte zeros[8] = {0,0,0,0,0,0,0,0};
    shiftDisplayLeft(disp, zeros);
    showGlyph(disp, speedMs);
  }
}
