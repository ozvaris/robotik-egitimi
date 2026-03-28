#include <ThreeWire.h>
#include <RtcDS1302.h>
// Sadece Okuyor
// Kütüphane: “Rtc by Makuna” (Library Manager’dan bu isimle bulup kurabilirsin)

// Resimdeki bağlantı:
// CLK -> D5, DAT -> D4, RST -> D2
ThreeWire myWire(4, 5, 2); // (DAT, CLK, RST)
RtcDS1302<ThreeWire> Rtc(myWire);

void setup() {
  Serial.begin(9600);
  delay(200);

  Serial.println("DS1302 baslatiliyor...");
  Rtc.Begin();

  // Yazma korumasını kapat (bazı modüllerde açık gelebiliyor)
  Rtc.SetIsWriteProtected(false);

  // Saat çalışmıyorsa çalıştır
  if (!Rtc.GetIsRunning()) {
    Serial.println("RTC duruyordu, baslatiliyor...");
    Rtc.SetIsRunning(true);
  }

  // RTC zamanı geçerli değilse derleme zamanına ayarla
  RtcDateTime now = Rtc.GetDateTime();
  if (!now.IsValid()) {
    Serial.println("RTC zamani gecersiz! Derleme zamani ayarlaniyor...");
    Rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
  }

  // İlk anda zamanı yazdır
  printDateTime(Rtc.GetDateTime());
}

void loop() {
  RtcDateTime now = Rtc.GetDateTime();
  printDateTime(now);
  delay(1000);
}

void printDateTime(const RtcDateTime& dt) {
  // Format: YYYY-MM-DD HH:MM:SS
  char datestring[20];
  snprintf_P(datestring,
             sizeof(datestring),
             PSTR("%04u-%02u-%02u %02u:%02u:%02u"),
             dt.Year(),
             dt.Month(),
             dt.Day(),
             dt.Hour(),
             dt.Minute(),
             dt.Second());

  Serial.println(datestring);
}
