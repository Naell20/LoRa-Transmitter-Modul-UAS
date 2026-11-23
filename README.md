# LoRa-Transmitter-Modul-UAS

| Konstanta | Fungsi |
|---|---|
| VREF = 5.0f | Tegangan referensi ADC yang diasumsikan 5 V, sesuai Arduino Nano. |
| ADC_MAX = 1023 | Nilai maksimum pembacaan ADC 10-bit (rentang 0–1023). |
| R1 = 33.0f, R2 = 8.2f | Nilai resistor pembagi tegangan untuk pengukuran generator. |
| DIVIDER_RATIO | Rasio untuk mengubah tegangan yang sudah diturunkan oleh pembagi resistor menjadi perkiraan tegangan sebelum pembagi. |
| VOLTAGE_CALIBRATION = 0.7371f | Faktor koreksi untuk pembacaan tegangan generator setelah dikalikan rasio pembagi. |
| OUTPUT_VOLTAGE_CALIBRATION = 1.992f | Faktor koreksi terpisah untuk kanal V output. Nilainya disetel dari hasil pengukuran Anda: pembacaan sekitar 2,50 V dikoreksi menjadi 4,98 V. |

### Pembacaan arus

| Konstanta | Fungsi |
|---|---|
| MV_PER_A = 185.0f | Sensitivitas sensor ACS712, dalam mV per ampere. Nilai ini harus cocok dengan jenis modul ACS712 yang dipakai. |
| CURRENT_SCALE = 0.11f / 0.055f | Faktor skala tambahan untuk arus generator; hasilnya adalah 2. |
| DC_SAMPLES = 20 | Jumlah pembacaan yang digunakan ACS712 untuk menghitung arus DC. |
| NOISE_DEADBAND_A = 0.05f | Pembacaan arus DC di bawah 0,05 A dianggap noise dan ditampilkan sebagai nol. Karena itu nilai 0,03 A yang Anda lihat sebelumnya akan menjadi 0 A. |
| SAMPLE_WINDOW = 1000 | Banyaknya sampel yang dipakai untuk menghitung arus RMS AC. |
| CALIBRATION_FACTOR = 1.0f | Faktor kalibrasi hasil pembacaan arus AC RMS. |
| NOISE_THRESHOLD = 0.02f | Pembacaan arus AC di bawah 0,02 A dianggap nol. Ini berbeda dari ambang arus DC di atas. |

### Perataan pembacaan

| Variabel | Fungsi |
|---|---|
| NUM_SAMPLES = 10 | Banyak nilai historis yang dirata-ratakan untuk mengurangi perubahan angka yang cepat. |
| dcCurrentHistory[] | Menyimpan riwayat pembacaan arus DC. |
| voltageHistory[] | Menyimpan riwayat pembacaan tegangan generator. |
| currentHistory[] | Menyimpan riwayat pembacaan arus AC RMS. |
| ...HistoryIndex | Menunjukkan posisi penyimpanan berikutnya pada array. |
| ...HistoryFull | Menandakan bahwa array riwayat sudah terisi penuh. |
