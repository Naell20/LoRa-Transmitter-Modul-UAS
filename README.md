# LoRa-Transmitter-Modul-UAS

| Konstanta | Fungsi |
|---|---|
| VREF = 5.0f | Tegangan referensi ADC yang diasumsikan 5 V, sesuai Arduino Nano. |
| ADC_MAX = 1023 | Nilai maksimum pembacaan ADC 10-bit (rentang 0–1023). |
| R1 = 33.0f, R2 = 8.2f | Nilai resistor pembagi tegangan untuk pengukuran generator. |
| DIVIDER_RATIO | Rasio untuk mengubah tegangan yang sudah diturunkan oleh pembagi resistor menjadi perkiraan tegangan sebelum pembagi. |
| VOLTAGE_CALIBRATION = 0.7371f | Faktor koreksi untuk pembacaan tegangan generator setelah dikalikan rasio pembagi. |
| ZMPT101B_CALIBRATION_FACTOR = 532.04f | Faktor kalibrasi tegangan RMS ZMPT101B berdasarkan pembacaan referensi 237,1 V dan pembacaan awal 218,81 V. |

### Pembacaan arus

| Konstanta | Fungsi |
|---|---|
| MV_PER_A = 185.0f | Sensitivitas sensor ACS712, dalam mV per ampere. Nilai ini harus cocok dengan jenis modul ACS712 yang dipakai. |
| CURRENT_SCALE = 0.11f / 0.055f | Faktor skala tambahan untuk arus generator; hasilnya adalah 2. |
| DC_SAMPLES = 20 | Jumlah pembacaan yang digunakan ACS712 untuk menghitung arus DC. |
| NOISE_DEADBAND_A = 0.05f | Pembacaan arus DC di bawah 0,05 A dianggap noise dan ditampilkan sebagai nol. Karena itu nilai 0,03 A yang Anda lihat sebelumnya akan menjadi 0 A. |
| AC_SAMPLE_WINDOW = 400 | Banyaknya sampel RMS untuk ZMPT101B dan ZHT103. |
| ZHT103_CURRENT_CALIBRATION = 1.0f | Faktor kalibrasi arus RMS sensor ZHT103. |
| AC_VOLTAGE_NOISE_THRESHOLD = 1.0f | Pembacaan tegangan AC di bawah 1 V dianggap nol. |
| AC_CURRENT_NOISE_THRESHOLD = 0.02f | Pembacaan arus AC di bawah 0,02 A dianggap nol. |

### Perataan pembacaan

| Variabel | Fungsi |
|---|---|
| NUM_SAMPLES = 10 | Banyak nilai historis yang dirata-ratakan untuk mengurangi perubahan angka yang cepat. |
| dcCurrentHistory[] | Menyimpan riwayat pembacaan arus DC. |
| voltageHistory[] | Menyimpan riwayat pembacaan tegangan generator. |
| currentHistory[] | Menyimpan riwayat pembacaan arus AC RMS. |
| ...HistoryIndex | Menunjukkan posisi penyimpanan berikutnya pada array. |
| ...HistoryFull | Menandakan bahwa array riwayat sudah terisi penuh. |


BY : WAYLEN 
