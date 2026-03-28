#include <ThreeWire.h>
#include <RtcDS1302.h>

// HEM OKUYOR HEM SET ile update ediyor
// Kütüphane: “Rtc by Makuna” (Library Manager’dan bu isimle bulup kurabilirsin)
// Serial Monitor’da “Newline” veya “Both NL & CR” seçili olsun.

ThreeWire myWire(4, 5, 2);  // DAT, CLK, RST  (resimdeki pinlere göre)
RtcDS1302<ThreeWire> Rtc(myWire);

String line;

void setup() {
  Serial.begin(9600);
  delay(200);

  Rtc.Begin();
  Rtc.SetIsWriteProtected(false);
  Rtc.SetIsRunning(true);

  Serial.println("DS1302 RTC Hazir.");
  Serial.println("Komutlar:");
  Serial.println("  GET");
  Serial.println("  SET YYYY-MM-DD HH:MM:SS   (ornek: SET 2026-01-02 14:45:30)");
  Serial.println("  HELP");
}

void loop() {
  // Seri satir oku
  while (Serial.available()) {
    char c = (char)Serial.read();
    if (c == '\n' || c == '\r') {
      if (line.length() > 0) {
        handleCommand(line);
        line = "";
      }
    } else {
      line += c;
      if (line.length() > 80) line.remove(0, line.length());  // taşmayı önle
    }
  }
}

void handleCommand(String cmd) {
  cmd.trim();
  if (cmd.length() == 0) return;

  // Büyük/küçük harf toleransı
  String up = cmd;
  up.toUpperCase();

  if (up == "HELP") {
    Serial.println("Komutlar:");
    Serial.println("  GET");
    Serial.println("  SET YYYY-MM-DD HH:MM:SS   (ornek: SET 2026-01-02 14:45:30)");
    return;
  }

  if (up == "GET") {
    printNow();
    return;
  }

  if (up.startsWith("SET ")) {
    String rest = cmd.substring(4);
    rest.trim();

    int Y, Mo, D, h, m, s;
    if (!parseDateTime(rest, Y, Mo, D, h, m, s)) {
      Serial.println("HATA: Format yanlis. Ornek:");
      Serial.println("  SET 2026-01-02 14:45:30");
      return;
    }

    if (Y < 2000 || Y > 2099 || Mo < 1 || Mo > 12 || D < 1 || D > 31 || h < 0 || h > 23 || m < 0 || m > 59 || s < 0 || s > 59) {
      Serial.println("HATA: Tarih/saat araligi gecersiz.");
      return;
    }

    // Yaz
    Rtc.SetIsWriteProtected(false);
    Rtc.SetIsRunning(true);
    Rtc.SetDateTime(RtcDateTime((uint16_t)Y, (uint8_t)Mo, (uint8_t)D,
                                (uint8_t)h, (uint8_t)m, (uint8_t)s));

    Serial.println("OK: Saat ayarlandi. (GET ile kontrol et)");
    return;
  }


  Serial.println("Bilinmeyen komut. HELP yaz.");
}

bool parseDateTime(const String& str, int& Y, int& Mo, int& D, int& h, int& m, int& s) {
  // Beklenen: "YYYY-MM-DD HH:MM:SS"
  // Sabit pozisyonlardan parse edeceğiz (hızlı + güvenli)
  if (str.length() < 19) return false;
  if (str.charAt(4) != '-' || str.charAt(7) != '-' || str.charAt(10) != ' ' || str.charAt(13) != ':' || str.charAt(16) != ':') return false;

  Y = str.substring(0, 4).toInt();
  Mo = str.substring(5, 7).toInt();
  D = str.substring(8, 10).toInt();
  h = str.substring(11, 13).toInt();
  m = str.substring(14, 16).toInt();
  s = str.substring(17, 19).toInt();

  // toInt() başarısızsa 0 döner; ay=00 vb yakalanacak, ama burada ekstra kontrol ekleyelim
  if (Y == 0 && str.substring(0, 4) != "0000") return false;
  return true;
}

void printNow() {
  RtcDateTime now = Rtc.GetDateTime();

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
