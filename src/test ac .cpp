const int CURRENT_PIN = A2;       // Gunakan A1 jika A0 dipakai ZMPT101B
const float VREF = 5.0f;          // Tegangan referensi ADC Arduino Nano
const int ADC_MAX = 1023;        // Re solusi ADC 10-bit
const int SAMPLE_WINDOW = 1000;  // Jumlah sampel per perhitungan RMS

// Ganti nilai ini setelah kalibrasi dengan arus aktual (Ampere)
// Rumus cepat: CALIBRATION_FACTOR = I_aktual / I_terbaca_awal
float CALIBRATION_FACTOR = 1.0f;

// Opsional: nolkan noise kecil saat arus sangat kecil / tidak ada beban
const float NOISE_THRESHOLD = 0.02f;

const int NUM_SAMPLES = 10;
float history[NUM_SAMPLES];
int idx = 0;
bool full = false;

float readCurrentRms() {
  long sumRaw = 0;
  for (int i = 0; i < SAMPLE_WINDOW; i++) {
    sumRaw += analogRead(CURRENT_PIN);
  }
  float offsetAdc = (float)sumRaw / SAMPLE_WINDOW;

  double sumSq = 0.0;
  for (int i = 0; i < SAMPLE_WINDOW; i++) {
    float diff = (float)analogRead(CURRENT_PIN) - offsetAdc;
    sumSq += (double)diff * (double)diff;
  }

  float rmsAdc = sqrt(sumSq / SAMPLE_WINDOW);
  float rmsVoltage = (rmsAdc / ADC_MAX) * VREF;

  float currentAmp = rmsVoltage * CALIBRATION_FACTOR;
  if (currentAmp < NOISE_THRESHOLD) {
    currentAmp = 0.0f;
  }
  return currentAmp;
}

float getSmoothedCurrent() {
  float currentVal = readCurrentRms();
  history[idx] = currentVal;
  idx = (idx + 1) % NUM_SAMPLES;
  if (idx == 0) {
    full = true;
  }

  int count = full ? NUM_SAMPLES : idx;
  if (count == 0) return currentVal;

float CALIBRATION_FACTOR = 1.0f;

// Opsional: nolkan noise kecil saat arus sangat kecil / tidak ada beban
const float NOISE_THRESHOLD = 0.02f;

const int NUM_SAMPLES = 10;
float history[NUM_SAMPLES];
int idx = 0;
bool full = false;

float readCurrentRms() {
	long sumRaw = 0;
	for (int i = 0; i < SAMPLE_WINDOW; i++) {
		sumRaw += analogRead(CURRENT_PIN);
	}
	float offsetAdc = (float)sumRaw / SAMPLE_WINDOW;

	double sumSq = 0.0;
	for (int i = 0; i < SAMPLE_WINDOW; i++) {
		float diff = (float)analogRead(CURRENT_PIN) - offsetAdc;
		sumSq += (double)diff * (double)diff;
	}

	float rmsAdc = sqrt(sumSq / SAMPLE_WINDOW);
	float rmsVoltage = (rmsAdc / ADC_MAX) * VREF;

	float currentAmp = rmsVoltage * CALIBRATION_FACTOR;
	if (currentAmp < NOISE_THRESHOLD) {
		currentAmp = 0.0f;
	}
	return currentAmp;
}

float getSmoothedCurrent() {
	float currentVal = readCurrentRms();
	history[idx] = currentVal;
	idx = (idx + 1) % NUM_SAMPLES;
	if (idx == 0) {
		full = true;
	}

	int count = full ? NUM_SAMPLES : idx;
	if (count == 0) return currentVal;

	float sum = 0.0f;
	for (int i = 0; i < count; i++) {
		sum += history[i];
	}
	return sum / count;
}

void setup() {
	Serial.begin(115200);
	for (int i = 0; i < NUM_SAMPLES; i++) {
		history[i] = 0.0f;
	}
}

void loop() {
	float arusStabil = getSmoothedCurrent();

	Serial.print("Arus (A): ");
	Serial.println(arusStabil, 2);

	delay(500);
}