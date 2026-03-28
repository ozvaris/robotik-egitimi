void handleSifreGiris(char key) {
  if (key >= '0' && key <= '9') {
    if (yildizBekleniyor) {
      sifreGirisEkrani();
    }

    rakamEkle(key);

    if (indexNo == 4) {
      delay(150);

      if (strcmp(girilen, aktifSifre) == 0) {
        mesajGoster("Kapi acildi");
        kapiAc();
      } else {
        mesajGoster("Hatali sifre");
      }

      delay(2000);
      sifreGirisEkrani();
    }
  }
  else if (key == '*') {
    if (indexNo == 0) {
      if (!yildizBekleniyor) {
        yildizBekleniyor = true;
        yildizUyariEkrani();
      } else {
        eskiSifreDogrulaEkrani();
      }
    } else {
      sifreGirisEkrani();
    }
  }
}

void handleDegistirmeEskiSifre(char key) {
  if (key >= '0' && key <= '9') {
    rakamEkle(key);

    if (indexNo == 4) {
      delay(150);

      if (strcmp(girilen, aktifSifre) == 0) {
        mesajGoster("Dogrulandi");
        delay(1000);
        yeniSifre1Ekrani();
      } else {
        mesajGoster("Hatali aktif", "sifre");
        delay(2000);
        sifreGirisEkrani();
      }
    }
  }
  else if (key == '*') {
    eskiSifreDogrulaEkrani();
  }
}

void handleYeniSifre1(char key) {
  if (key >= '0' && key <= '9') {
    rakamEkle(key);

    if (indexNo == 4) {
      strcpy(yeniSifre, girilen);
      delay(200);
      yeniSifre2Ekrani();
    }
  }
  else if (key == '*') {
    yeniSifre1Ekrani();
  }
}

void handleYeniSifre2(char key) {
  if (key >= '0' && key <= '9') {
    rakamEkle(key);

    if (indexNo == 4) {
      delay(150);

      if (strcmp(girilen, yeniSifre) == 0) {
        strcpy(aktifSifre, yeniSifre);
        eepromSifreKaydet(aktifSifre);
        mesajGoster("Sifre degisti");
      } else {
        mesajGoster("Sifreler farkli");
      }

      delay(2000);
      sifreGirisEkrani();
    }
  }
  else if (key == '*') {
    yeniSifre2Ekrani();
  }
}