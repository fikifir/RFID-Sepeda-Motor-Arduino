# **RFID Sepeda Motor**

> Ini adalah Source code Akses kunci sepeda motor tanpa kunci manual/ dengan menggunakan kartu RFID. Menghidupkan Sepeda Motor anda dengan RFID card.

Project ini di buat untuk mempermudah pemilik sepeda motor untuk menghidupkan dan mematikan sepeda motor dengan mudah. Serta memberi perlindungan anti-maling.

Video dibawah ini adalah contoh dan aplikasi sistem RFID Arduino.

[![Watch the video](https://img.youtube.com/vi/wx30pKiNoxE/maxresdefault.jpg)](https://youtu.be/wx30pKiNoxE)

Jika anda mengalami kesulitan dan memerlukan bantuan silahkan hubungi saya di alamar email ini, saya akan berusaha menjawab semua pertanyaan.

---

## **Index**
1. [Alat dan Bahan](#alat)
	- [1.1 Alat](#alat)
	- [1.2 Bahan](#bahan)
2. [Prinsip Kerja Sistem](#principle)
3. [Diagram](#diagram)
	- [2.1 Block Diagram](#block)
	- [2.2 Schematic Diagram](#schematic)
4. [Build Instructions](#build)
	- [3.1 Arduino IDE](#arduinoide)
		- [3.1.1 Download & Instalasi Software Arduino](#install)
		- [3.1.2 Instalasi Library module RFID RC522](#rc522install)
5. [Kesimpulan](#kesimpulan)
6. [Referensi](#referensi)

---

## <a name="alat">1. Alat dan Bahan</a>
### 1.1 Alat

| No. | Deskripsi | Qty
|:--|:--|:--
| 1 | Obeng (+/-) untuk membuka body motor | 1
| 2 | Solder & Timah | 1
| 3 | Tang potong/ Tang kombinasi| 1
| 4 | Cutter | 1

### 1.2 Bahan

| No. | Deskripsi | Qty
|:--|:--|:--
| 1 | Arduino UNO/NANO | 1
| 2 | RFID RC522 + Keytag/Kartu RFID | 1
| 3 | Module Relay Arduino 5v (3-channel) | 1
| 4 | Buck Converter DC-DC Stepdown | 1
| 5 | LED | 2
| 6 | Resistor 1K | 2
| 6 | Kabel / wire jumper | secukupnya

---

## <a name="principle">2. Prinsip Kerja Sistem</a>

Prinsip kerja RFID sepeda motor ini sama pengan sistem Door Lock RFID yang di buat oleh Miguel[^miguel orang keren [keren sekali](www.youtube.com)]

---

## <a name="diagram">3. Diagram</a>

Diagram kerja

### <a name="wiring">3.1 Block Diagram</a>
### <a name="schematic">3.2 Schematics Diagram</a>

---

## <a name="build">4. Build Instructions</a>
### <a name="install">4.1 Instalasi arduino IDE</a>
### Kunci RFID Manual
untuk Kunci kartu RFID dengan menggunakan 1 Relay.
Relay 1 = digunakan untuk kontak master
lalu starter tombol/ kickstarter secara manual

### Kunci RFID Otomatis
Untuk Kunci kartu RFID dengan menggunakan 2 Relay.
Relay 1 = digunakan untuk kontak master
Relay 2 = digunakan untuk tombol starter selama 1.5 detik

---

## <a name="kesimpulan">5. Kesimpulan</a>
Kode "library" asli ditulis oleh miguel

---

## <a name="referensi">6. Reference</a>
Full credit untuk Michael baobla


I know i don't usually do this
or ask for help. Anyway
i feel like i need to
music, tv, skateboarding 
and surfing
usually bring me happiness
Instead,
I'm feeling empty and fear
for what tomorrow will
bring and shit
plus, I'm not
f*cking sleeping 
as good as i usually do
why do i feel this way?
why do i feel worthlessness
and helplessness
will never go away
is there a way out?
or do i need to see
a f*ckin doctor
let me know, your boy. Fiki