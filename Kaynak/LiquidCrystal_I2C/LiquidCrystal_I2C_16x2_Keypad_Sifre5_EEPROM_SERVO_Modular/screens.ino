void sifreGirisEkrani() {
  aktifMod = SIFRE_GIRIS;
  yildizBekleniyor = false;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Lutfen Sifre");
  lcd.setCursor(0, 1);
  lcd.print("Giriniz:");
}

void yildizUyariEkrani() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tekrar * basin");
  lcd.setCursor(0, 1);
  lcd.print("Sifre degistir");
}

void eskiSifreDogrulaEkrani() {
  aktifMod = DEGISTIRME_ESKI_SIFRE;
  yildizBekleniyor = false;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Aktif sifreyi");
  lcd.setCursor(0, 1);
  lcd.print("giriniz:");
}

void yeniSifre1Ekrani() {
  aktifMod = YENI_SIFRE_1;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Yeni sifre:");
  lcd.setCursor(0, 1);
}

void yeniSifre2Ekrani() {
  aktifMod = YENI_SIFRE_2;
  bufferTemizle();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Tekrar girin:");
  lcd.setCursor(0, 1);
}