# RFID Sepeda Motor

> Ini adalah Source code Akses kunci sepeda motor tanpa kunci manual/ dengan menggunakan kartu RFID. Menghidupkan Sepeda Motor anda dengan RFID card.

## Index
1. [Alat dan Bahan](#alat)
2. [Diagram](#diagram)
	- [Schematic Diagram](#schematic)
	- [Wiring Diagram](#wiring)
3. [Build Instructions](#wiring)
4. [Finished Product](#finished)
5. [Reference](#reference)
6. [Conclusion](#conclusion)
7. [Licensing](#licensing)
8. [Gallery](#gallery)

## <a name="alat">Alat & Bahan</a>

| No. | Alat | Qty
|:--|:--|:--
| 1 | Arduino Nano/ Arduino NANO | 1
| 2 | RFID RC522 + Keytag/Kartu RFID | 1
| 3 | Module Relay 5v (3channel untuk motor garburator dan 4-channel untuk motor injeksi) | 1
| 4 | LED indikator | 2
| 5 | Resistor 1K | 2
| 6 | Transistor PNP | 2


### Instalasi arduino IDE
## <a name="diagram">Diagram</a>
### Kunci RFID Manual
untuk Kunci kartu RFID dengan menggunakan 1 Relay.
Relay 1 = digunakan untuk kontak master
lalu starter tombol/ kickstarter secara manual

### Kunci RFID Otomatis
Untuk Kunci kartu RFID dengan menggunakan 2 Relay.
Relay 1 = digunakan untuk kontak master
Relay 2 = digunakan untuk tombol starter selama 1.5 detik

## Referensi
Kode "library" asli ditulis oleh miguel

## Lisensi
Mohon sertakan 