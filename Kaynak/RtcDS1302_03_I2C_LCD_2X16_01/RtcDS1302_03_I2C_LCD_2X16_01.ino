#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// ================== DONANIM ==================
// DS1302 pinleri (Arduino UNO örneği)
// DAT -> D4, CLK -> D5, RST/CE -> D2
ThreeWire myWire(4, 5, 2);  // DAT, CLK, RST
RtcDS1302<ThreeWire> Rtc(myWire);

// I2C LCD (senin scanner çıktın: 0x27)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ================== SERIAL KOMUT OKUMA ==================
String line;

// ================== LCD GÜNCELLEME ==================
unsigned long lastLcdMs = 0;
const unsigned long LCD_PERIOD_MS = 1000;

// Son geçerli değerleri tut (RTC bazen tek okumada "invalid" dönebiliyor)
char lastTbuf[9]  = "--:--:--";     // "HH:MM:SS"
char lastDbuf[11] = "---- -- --";   // "YYYY-MM-DD" yerine placeholder
bool hasLastGood = false;

RtcDateTime now = RtcDateTime(0);

void updateLcdClock() {
  // 1) RTC oku (gerekirse 2 kez dene)
  // RtcDateTime now = Rtc.GetDateTime();
  delay(100);

  char tbuf[9];
  char dbuf[11];

  if (!now.IsValid()) {
    // RTC invalid ise:
    // - Daha önce geçerli değer okuduysak onu göstermeye devam et
    // - Hiç yoksa placeholder bas
    if (hasLastGood) {
      snprintf(tbuf, sizeof(tbuf), "%s", lastTbuf);
      snprintf(dbuf, sizeof(dbuf), "%s", lastDbuf);
    } else {
      snprintf(tbuf, sizeof(tbuf), "--:--:--");
      snprintf(dbuf, sizeof(dbuf), "---- -- --");
    }
  } else {
    snprintf(tbuf, sizeof(tbuf), "%02u:%02u:%02u", now.Hour(), now.Minute(), now.Second());
    snprintf(dbuf, sizeof(dbuf), "%04u-%02u-%02u", now.Year(), now.Month(), now.Day());

    // son iyi değerleri sakla
    snprintf(lastTbuf, sizeof(lastTbuf), "%s", tbuf);
    snprintf(lastDbuf, sizeof(lastDbuf), "%s", dbuf);
    hasLastGood = true;
  }

  // 2) LCD yaz
  lcd.setCursor(0, 0);
  lcd.print("Saat: ");
  lcd.print(tbuf);
  // satır sonunu temizle
  int used0 = 6 + 8; // "Saat: " (6) + HH:MM:SS (8)
  for (int i = used0; i < 16; i++) lcd.print(' ');

  lcd.setCursor(0, 1);
  lcd.print("Tarih:");
  lcd.print(dbuf); // 10 karakter hedef
  int used1 = 6 + 10; // "Tarih:" (6) + YYYY-MM-DD (10)
  for (int i = used1; i < 16; i++) lcd.print(' ');
}

// ================== KOMUTLAR ==================
void printNow() {
  // RtcDateTime now = Rtc.GetDateTime();
  now = Rtc.GetDateTime();

  if (!now.IsValid()) {
    Serial.println("RTC zamani gecersiz (invalid).");
    return;
  }

  char buf[24];
  snprintf(buf, sizeof(buf), "%04u-%02u-%02u %02u:%02u:%02u",
           now.Year(), now.Month(), now.Day(),
           now.Hour(), now.Minute(), now.Second());
  Serial.print("RTC: ");
  Serial.println(buf);
}

bool parseDateTime(const String& str, int& Y, int& Mo, int& D, int& h, int& m, int& s) {
  // "YYYY-MM-DD HH:MM:SS"
  if (str.length() < 19) return false;
  if (str.charAt(4) != '-' || str.charAt(7) != '-' ||
      str.charAt(10) != ' ' ||
      str.charAt(13) != ':' || str.charAt(16) != ':') return false;

  Y  = str.substring(0, 4).toInt();
  Mo = str.substring(5, 7).toInt();
  D  = str.substring(8, 10).toInt();
  h  = str.substring(11, 13).toInt();
  m  = str.substring(14, 16).toInt();
  s  = str.substring(17, 19).toInt();

  if (Y < 2000 || Y > 2099) return false;
  if (Mo < 1 || Mo > 12) return false;
  if (D < 1 || D > 31) return false;
  if (h < 0 || h > 23) return false;
  if (m < 0 || m > 59) return false;
  if (s < 0 || s > 59) return false;

  return true;
}

void handleCommand(String cmd) {
  cmd.trim();

  if (cmd.equalsIgnoreCase("HELP")) {
    Serial.println("Komutlar:");
    Serial.println("  GET");
    Serial.println("  SET YYYY-MM-DD HH:MM:SS   (ornek: SET 2026-01-02 14:45:30)");
    Serial.println("  HELP");
    return;
  }

  if (cmd.equalsIgnoreCase("GET")) {
    printNow();
    updateLcdClock(); // GET sonrası LCD'yi de güncelle
    return;
  }

  if (cmd.startsWith("SET")) {
    String rest = cmd.substring(3);
    rest.trim();

    int Y, Mo, D, h, m, s;
    if (!parseDateTime(rest, Y, Mo, D, h, m, s)) {
      Serial.println("HATA: Format: SET YYYY-MM-DD HH:MM:SS");
      return;
    }

    RtcDateTime dt(Y, Mo, D, h, m, s);
    Rtc.SetDateTime(dt);

    Serial.println("OK: Saat ayarlandi. (GET ile kontrol et)");
    updateLcdClock(); // SET sonrası LCD'yi de güncelle
    return;
  }

  Serial.println("Bilinmeyen komut. HELP yaz.");
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  // RTC
  Rtc.Begin();
  Rtc.SetIsWriteProtected(false);
  if (!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }

  // LCD
  Wire.begin();
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("I2C LCD Hazir");
  lcd.setCursor(0, 1);
  lcd.print("Saat yukleniyor");
  delay(600);

  Serial.println("DS1302 RTC Hazir.");
  Serial.println("Komutlar:");
  Serial.println("  GET");
  Serial.println("  SET YYYY-MM-DD HH:MM:SS   (ornek: SET 2026-01-02 14:45:30)");
  Serial.println("  HELP");

  updateLcdClock();
  lastLcdMs = millis();
}

void loop() {
  // 1) LCD'yi her 1 saniyede bir güncelle (bu blok senin dosyada yorum satırıydı)
  unsigned long nowMs = millis();
  if (nowMs - lastLcdMs >= LCD_PERIOD_MS) {
    lastLcdMs += LCD_PERIOD_MS; // drift azaltır
    // updateLcdClock();
    // handleCommand("GET");
    printNow();
    updateLcdClock();
  }

  // 2) Serial komutlarını oku
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (line.length() > 0) {
        handleCommand(line);
        line = "";
      }
    } else {
      line += c;
      if (line.length() > 80) line.remove(0, line.length());
    }
  }
}
