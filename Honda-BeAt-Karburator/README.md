# RFID_Sepeda_Motor

Ini adalah Source code Akses kunci sepeda motor tanpa kunci manual/dengan menggunakan kartu RFID.
Menghidupkan Sepeda Motor anda dengan RFID. video bisa dilihat di (https://youtu.be/wx30pKiNoxE)

## Alat-alat & Bahan yang Dibutuhkan

1. Arduino UNO
2. RFID RC522
3. Relay 3 Channel
4. Power Regulator 12V to 5-9 Volt.
5. Kabel, isolasi dll

## Koneksi antara Arduino, Relay, RFID dan Power Supply

1. Koneksi antara RFID ke arduino UNO,

(RC522)     (UNO)
MOSI ------- Pin 11
MISO ------- Pin 12
SCK -------- Pin 13
SDA/SS ----- Pin 10
RST -------- Pin 9
3.3V ------- 3.3V
GND -------- GND
  
2. Koneksi Relay dengan Arduino UNO

(Relay)     (UNO)
in1 -------- 8
in2 -------- 7
in3 -------- 6
VCC -------- 5V
GND -------- GND
  
3. Koneksi Power Supply 6~12 volt

Dianjurkan menggunakan power regulator untuk menurunkan tegangan dari aki motor 12v ke 6~9volt. bisa membuat sediri menggunakan chip IC LM7805 tutoorial bisa di lihat di (https://youtu.be/nkLXVcNdjxg)

Accu (+) ---> ke Saklar master ---> Arduino Input(+)
Accu (-) ---> Arduino (+)

anda bisa mengganti saklar master menggunakan saklar pada standard kaki samping dengan memotong kabel.
kabel yang dari standard kaki kiri disambungkan ke (+) aki dan satunya langsung ke input arduino.
kabel yang terpotong menuju cdi motor bisa di jumper (tidak merusak pengapian) karena motor hidup harus dalam posisi (jumper)

  
## Koneksi Antara Controller ke Mekanik Motor

1. Relay1 ----- Kunci Motor
2. Relay2 ----- Rem Belakang (letaknya di rem kiri berwarna kuning berfungsi seperti saklar NO-Normally Open)
3. Relay3 ------Tombol Starter
