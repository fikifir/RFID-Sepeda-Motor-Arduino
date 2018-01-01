#include <EEPROM.h>                         // Baca dan Tulis PICC's UIDs dari/ke EEPROM
#include <SPI.h>                            // Modul RC522 menggunakan SPI protocol
#include <MFRC522.h>                        // Library untuk Mifare RC522


#define blueLed 2                           // Set LED Indikator Di Pin2 Arduino
#define relay1 8                            // Set Relay 1 di Pin8 Arduino
#define relay2 7                            // Set Relay 2 di Pin7 Arduino
#define relay3 6                            // Set Relay 3 di Pin6 Arduino

 
boolean match = false;                      // menginisialisasi kartu yang cocok dengan yang salah
boolean programMode = false;                // menginisialisasi mode pemrograman ke 'false'

int successRead;                            // Variabel integer untuk disimpan jika kita memiliki SuccessRead dari Reader

byte storedCard[4];                         // Menyimpan ID yang ter-scan dari EEPROM
byte readCard[4];                           // Menyimpan ID yang ter-scan dari Modul RFID
byte masterCard[4];                         // Menyimpan ID master card dari EEPROM

                                            /*
                                            Koneksi antara RFIDRC522 ke pin Arduino Uno/Arduino Nano:
                                            MOSI: Pin 11 / ICSP-4
                                            MISO: Pin 12 / ICSP-1
                                            SCK : Pin 13 / ICSP-3
                                            SS  : Pin 10 (bisa di-konfigurasi ulang)
                                            RST : Pin 9 (bisa di-konfigurasi ulang)
                                            lihat MFRC522 Library untuk menggunakan pin Arduino yg berbeda
                                            */

#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);           // Membuat Contoh MFCR522

///////////////////////////////////////// Setup ///////////////////////////////////
void setup() {
  pinMode(blueLed, OUTPUT);                 // Setup Pin "blueled" sebagai output
  pinMode(relay1, OUTPUT);                  // Setup Pin "relay1" sebagai output
  pinMode(relay2, OUTPUT);                  // Setup Pin "relay2" sebagai output
  pinMode(relay3, OUTPUT);                  // Setup Pin "relay3" sebagai output
  digitalWrite(relay1, LOW);                // Memastikan Relay dalam Posisi "OFF"
  digitalWrite(relay2, LOW);                // Memastikan Relay dalam Posisi "OFF"
  digitalWrite(relay3, LOW);                // Memastikan Relay dalam Posisi "OFF"
  digitalWrite(blueLed, HIGH);              // Memastikan led dalam Keadaan "ON"
  
  Serial.begin(9600);                       // Komunikasi dengan PC menggunakan Serial Protocol
  SPI.begin();                              // MFRC522 Hardware uses SPI protocol
  mfrc522.PCD_Init();                       // Inisialisasi Hardware RFID MFRC522
  
  mfrc522.PCD_SetAntennaGain(mfrc522.RxGain_max); // Untuk memaksimalkan jarak RFID hingga 1cm.
  
  Serial.println(F("Access Control v3.3")); // Untuk proses mode "Debug"
  ShowReaderDetails();                      // Menunjukkan detail dari PCD - Detail Card reader MFRC522.

                                            // Menge-cek jika master card sudah ditentukan (terekam dalam EEPROM).
                                            // jika masih belum ada master card yang terdaftar, user bisa menentukan master card.
                                            // Ini juga sangat berguna untuk menentukan Master card
                                            // Kamu dapat menyimpan rekaman EPPROM yang lain dengan syarat harus selain dari '143' ke EEPROM address 1
                                            // EEPROM address 1 harus menyimpan angka ajaib-nya yaitu '143'
  if (EEPROM.read(1) != 143) {      
    Serial.println(F("No Master Card Defined"));
    Serial.println(F("Scan A PICC to Define as Master Card"));
    do {
      successRead = getID();                // Set successRead ke posisi '1' saat membaca kartu RFID, jika tidak cocok ke posisi '0'
      digitalWrite(blueLed, HIGH);          // Indikator LED akan berkedip jika mastercard belum di set/ di daftarkan.
      delay(200);
      digitalWrite(blueLed, LOW);
      delay(200);
      digitalWrite(blueLed, HIGH);
      delay(200);
    }
    while (!successRead);                   // Program tidak akan melanjutkan ke proses selanjutnya jika tidak dapat membaca kartu
    for ( int j = 0; j < 4; j++ ) {         // Loop 4 kali
      EEPROM.write( 2 + j, readCard[j] );   // Menulis/merekam PICC's UID ke EEPROM, dimulai dari address 3
    }
    EEPROM.write(1, 143);                   // Menulis/merekam Master card ke adress 1, disini kita menentukan Master Card.
    Serial.println(F("Master Card telah ditetapkan"));
  }
  Serial.println(F("-------------------"));
  Serial.println(F("UID Master Card"));
  for ( int i = 0; i < 4; i++ ) {           // Membaca Master Card's UID dari EEPROM
    masterCard[i] = EEPROM.read(2 + i);     // Menulis/merekam ke masterCard
    Serial.print(masterCard[i], HEX);
  }
  Serial.println("");
  Serial.println(F("-------------------"));
  Serial.println(F("Perangkat RFID Siap"));
  Serial.println(F("Menunggu PICCs untuk di scan"));
  cycleLeds();                              // Semua telah siap, dan jika PICCs di scan, LED akan memberikan sinyal feedback.
}


///////////////////////////////////////// Main Loop ///////////////////////////////////
void loop () {
  do {
    successRead = getID();                  // Set successRead ke posisi '1' saat membaca kartu RFID, jika tidak cocok ke posisi '0'
    if (programMode) {
      cycleLeds();                          // Indikator LED akan berkedip menunggu kartu baru untuk di scan
    }
    else {
      normalModeOn();                       // Normal mode, blue LED OFF,
    }
  }
  while (!successRead);                     //  Program tidak akan melanjutkan ke proses selanjutnya jika tidak dapat membaca kartu
  if (programMode) {
    if ( isMaster(readCard) ) {             // Jika Master card di scan lagi akan keluar dari "Mode Program"
      Serial.println(F("Master Card telah di Scan"));
      Serial.println(F("Keluar dari Mode Program"));
      Serial.println(F("-----------------------------"));
      programMode = false;
      return;
    }
    else {
      if ( findID(readCard) ) {             // Jika kartu yang di-scan sudah terdaftar sebelumnya, maka akan dihapus
        Serial.println(F("Saya Kenal Kartu Ini, Menghapus..."));
        deleteID(readCard);
        Serial.println("--------------Berhasil--------------");
      }
      else {                                // Jika kartu yang di-scan belum terdaftar, maka akan didaftarkan
        Serial.println(F("Saya Tidak Mengenal Kartu Ini, Mendaftarkan..."));
        writeID(readCard);
        Serial.println(F("--------------Berhasil--------------"));
      }
    }
  }
  else {
    if ( isMaster(readCard) ) {             // Jika kartu yang di scan adalah Master Card maka akan masuk 'Mode Program'.
      programMode = true;
      Serial.println(F("Selamat Datang Master - Masuk dalam 'Mode Program'"));
      int count = EEPROM.read(0);           // Read the first Byte of EEPROM that
      Serial.print(F("Saya Mempunyai "));   // Mendaftarkan/ menyimpan nomor ID di EEPROM
      Serial.print(count);
      Serial.print(F(" ID yang tersimpan di EEPROM"));
      Serial.println("");
      Serial.println(F("Scan PICC untuk 'Menambahkan/Daftarkan' Atau 'Menghapus'"));
      Serial.println(F("-----------------------------"));
    }
    else {
      if ( findID(readCard) ) {             // If not, see if the card is in the EEPROM
        Serial.println(F("Akses Diterima"));
        granted();                          // Akses Diterima
      }
      else {                                // Jika tidak cocok, maka akan menampilkan teks kalau ID tidak cocok
        Serial.println(F("Kamu Ngapain Coba-coba tempel kartu RFID? Mau Curi Motor Orang?"));
        denied();
      }
    }
  }
}

/////////////////////////////////////////  Akses Diterima    ///////////////////////////////////
void granted() {
  digitalWrite(blueLed, LOW);               // LED akan mati
  digitalWrite(relay1, HIGH);               // Relay1 hidup (kunci kontak nyala)
  delay(100);                               // tunggu 100ms sejenak
  digitalWrite(relay2, HIGH);               // Relay2 hidup (Rem belakan nyala) bersamaan dengan Relay3
  delay(2500);                              // Selama 2.5 detik
  digitalWrite(relay3, HIGH);               // Relay3 hidup (Stater nyala) bersamaan dengan Relay2
  delay(2500);                              // Selama 2.5 detik
  digitalWrite(relay2, LOW);                // Relay2 akan mati
  digitalWrite(relay3, LOW);                // Relay3 akan mati
  digitalWrite(relay1, HIGH);               // Relay1 (kontak) tetap hidup selama (43200000ms)
  delay(43200000);
}

///////////////////////////////////////// Akses Ditolak  ///////////////////////////////////
void denied() {
  digitalWrite(blueLed, HIGH);              // LED akan berkedip setiap 10ms.
  delay(10);
  digitalWrite(blueLed, LOW);
  delay(10);
  digitalWrite(blueLed, HIGH);
  delay(10);
  digitalWrite(blueLed, LOW);
  delay(10);
}


///////////////////////////////////////// Merekam PICC UID ///////////////////////////////////
int getID() {
                                            // Bersiap untuk membaca PICCs
  if ( ! mfrc522.PICC_IsNewCardPresent()) { // Jika PICC masih baru, berikan sinyal ke RFID reader untuk memproses ke tahap selanjutnya
    return 0;
  }
  if ( ! mfrc522.PICC_ReadCardSerial()) {   // Melanjutkan PICC untuk komunikasi Serial dan melanjutkan ke proses selanjutnya
    return 0;
  }
                                            // Mifare PICC dipasaran ada versi 4 dan 7 bit UID. jaga-jaga jika anda menggunakan versi 7bit
                                            // Kita beransumsi bahwa setiap PICC menggunakan 4bit UID
                                            // Hingga suatu saat mungkin akan mendukung UID 7bit
  Serial.println(F("Scanned PICC's UID:"));
  for (int i = 0; i < 4; i++) {  //
    readCard[i] = mfrc522.uid.uidByte[i];
    Serial.print(readCard[i], HEX);
  }
  Serial.println("");
  mfrc522.PICC_HaltA();                     // Berhenti Membaca
  return 1;
}

void ShowReaderDetails() {
                                            // Print Versi Software MFRC522
  byte v = mfrc522.PCD_ReadRegister(mfrc522.VersionReg);
  Serial.print(F("Versi Software MFRC522: 0x"));
  Serial.print(v, HEX);
  if (v == 0x91)
    Serial.print(F(" = v1.0"));
  else if (v == 0x92)
    Serial.print(F(" = v2.0"));
  else
    Serial.print(F(" (Tidak diketahui)"));
  Serial.println("");
                                            // Jika 0x00 atau 0xFF dikembalikan, komunikasi mungkin gagal
  if ((v == 0x00) || (v == 0xFF)) {
    Serial.println(F("PERINGATAN: Komunikasi Gagal, apakah MFRC522 terhubung dengan benar?"));
    while(true);                            // Jangan melanjutkan leih jauh
  }
}

///////////////////////////////////////// Cycle Leds (Program Mode) ///////////////////////////////////
void cycleLeds() {
  digitalWrite(blueLed, LOW);               // LED akan 'ON' dan 'OFF' setiap 500ms
  delay(500);
  digitalWrite(blueLed, HIGH);
  delay(500);
  digitalWrite(blueLed, LOW);
  delay(500);
  digitalWrite(blueLed, HIGH);
  delay(500);
}

//////////////////////////////////////// Normal Mode Led  ///////////////////////////////////
void normalModeOn() {
  digitalWrite(blueLed, HIGH);              // LED 'ON' dan siap membaca kartu
  digitalWrite(relay1, LOW);                // Pastikan relay1 mati
  digitalWrite(relay2, LOW);                // Pastikan relay2 mati 
  digitalWrite(relay3, LOW);                // Pastikan relay3 mati
}

//////////////////////////////////////// Membaca ID dari EEPROM //////////////////////////////
void readID( int number ) {
  int start = (number * 4 ) + 2;            // Figure out starting position
  for ( int i = 0; i < 4; i++ ) {           // Loop 4 times to get the 4 Bytes
    storedCard[i] = EEPROM.read(start + i); // Assign values read from EEPROM to array
  }
}

///////////////////////////////////////// Menambahkan ID ke EEPROM   ///////////////////////////////////
void writeID( byte a[] ) {
  if ( !findID( a ) ) {                     // Before we write to the EEPROM, check to see if we have seen this card before!
    int num = EEPROM.read(0);               // Get the numer of used spaces, position 0 stores the number of ID cards
    int start = ( num * 4 ) + 6;            // Figure out where the next slot starts
    num++;                                  // Increment the counter by one
    EEPROM.write( 0, num );                 // Write the new count to the counter
    for ( int j = 0; j < 4; j++ ) {         // Loop 4 times
      EEPROM.write( start + j, a[j] );      // Write the array values to EEPROM in the right position
    }
    successWrite();
  Serial.println(F("Berhasil menambahkan ID ke EEPROM"));
  }
  else {
    failedWrite();
  Serial.println(F("Gagal! Ada yang salah dengan ID atau EEPROM anda"));
  }
}

///////////////////////////////////////// Menghapus ID dari EEPROM   ///////////////////////////////////
void deleteID( byte a[] ) {
  if ( !findID( a ) ) {                      // Sebelum kita hapus dari EEPROM, periksa untuk melihat apakah kita mempunyai kartu ini
    failedWrite();                           // jika tidak
  Serial.println(F("Gagal! Ada yang salah dengan ID atau EEPROM anda"));
  }
  else {
    int num = EEPROM.read(0);                // Dapatkan jumlah spasi yang digunakan, posisi 0 menyimpan jumlah ID
    int slot;                                // Cari tahu nomor slot kartu
    int start;                               // = ( num * 4 ) + 6; // Cari tahu di mana slot berikutnya dimulai
    int looping;                             // Jumlah/Frekuensi loop yang berulang
    int j;
    int count = EEPROM.read(0);              // Baca EEPROM bit pertama yang menyimpan ID kartu
    slot = findIDSLOT( a );                  // Cari tahu nomor slot kartu yang akan dihapus
    start = (slot * 4) + 2;
    looping = ((num - slot) * 4);
    num--;                                   // Turunkan counter dengan satu
    EEPROM.write( 0, num );                  // Tulis penghitung baru ke Counter
    for ( j = 0; j < looping; j++ ) {        // Loop waktu shift kartu
      EEPROM.write( start + j, EEPROM.read(start + 4 + j));   // Pergeseran nilai array ke 4 tempat sebelumnya di EEPROM
    }
    for ( int k = 0; k < 4; k++ ) {          // Pergeseran loop
      EEPROM.write( start + j + k, 0);
    }
    successDelete();
  Serial.println(F("Berhasil menghapus ID dari EEPROM"));
  }
}

///////////////////////////////////////// Cek Bit   ///////////////////////////////////
boolean checkTwo ( byte a[], byte b[] ) {
  if ( a[0] != NULL )                       // Pertama, Pastikan ada sesuatu di dalam array
    match = true;                           // Asumsikan semua cocok pada awalnya
  for ( int k = 0; k < 4; k++ ) {           // Loop 4 kali
    if ( a[k] != b[k] )                     // Jika a != b lalu atur kecocokan = false, satu fails, semua fail
      match = false;
  }
  if ( match ) {                            // Periksa untuk melihat apakah jika kecocokan masih benar
    return true;                            // Kembali true
  }
  else  {
    return false;                           // Kembali false
  }
}

///////////////////////////////////////// Menemukan Slot   ///////////////////////////////////
int findIDSLOT( byte find[] ) {
  int count = EEPROM.read(0);               // Baca Bit pertama EEPROM yang
  for ( int i = 1; i <= count; i++ ) {      // Loop sekali untuk setiap entri EEPROM
    readID(i);                              // Baca ID dari EEPROM, disimpan di dalam storeCard [4]
    if ( checkTwo( find, storedCard ) ) {   // Periksa untuk melihat apakah penyimpanan tersimpan dibaca dari EEPROM
                                            // sama-sama dengan yang ditemukan [] ID Card lulus
      return i;                             // Nomor slot kartu
      break;                                // Berhenti Mencari, kita menemukannya
    }
  }
}

///////////////////////////////////////// Mencari ID dari EEPROM   ///////////////////////////////////
boolean findID( byte find[] ) {
  int count = EEPROM.read(0);               // Membaca bit pertama EEPROM
  for ( int i = 1; i <= count; i++ ) {      // Loop once for each EEPROM entry
    readID(i);                              // Read an ID from EEPROM, it is stored in storedCard[4]
    if ( checkTwo( find, storedCard ) ) {   // Check to see if the storedCard read from EEPROM
      return true;
      break;                                // Berhenti mencari, kita sudah menemukannya
    }
    else {                                  // Jika tidak, kembali ke 'false'
    }
  }
  return false;
}

///////////////////////////////////////// Sukses menulis ke EEPROM   ///////////////////////////////////
void successWrite() {
  digitalWrite(blueLed, LOW);               // LED akan berkedip setiap 50ms
  delay(50);
  digitalWrite(blueLed, HIGH);
  delay(50);
  digitalWrite(blueLed, LOW);
  delay(50);
  digitalWrite(blueLed, HIGH);
  delay(50);
  digitalWrite(blueLed, LOW);
  delay(50);
  digitalWrite(blueLed, HIGH);
  delay(50);
}

///////////////////////////////////////// Gagal Menulis ke EEPROM   ///////////////////////////////////
void failedWrite() {
  digitalWrite(blueLed, LOW);               // LED akan Berkedip setiap 1 detik
  delay(1000);
  digitalWrite(blueLed, HIGH);
  delay(1000);
  digitalWrite(blueLed, LOW);
  delay(1000);
  digitalWrite(blueLed, HIGH);
  delay(1000);
}

///////////////////////////////////////// Sukses Menghapus UID dari EEPROM  ///////////////////////////////////
void successDelete() {
  digitalWrite(blueLed, LOW);               // LED Berkedip setiap 200ms
  delay(200);
  digitalWrite(blueLed, HIGH);
  delay(200);
  digitalWrite(blueLed, LOW);
  delay(200);
  digitalWrite(blueLed, HIGH);
  delay(200);
  digitalWrite(blueLed, LOW);
  delay(200);
  digitalWrite(blueLed, HIGH);
  delay(200);
}

////////////////////// Cek readCard JIKA masterCard   ///////////////////////////////////
                                           // Memeriksa jika ID memang Master Card
boolean isMaster( byte test[] ) {
  if ( checkTwo( test, masterCard ) )
    return true;
  else
    return false;
}

