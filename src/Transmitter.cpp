#include <Arduino.h>
#include <ACS712.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <math.h>

const int csPin = 10;
const int resetPin = 9;
const int DIO0 = 8;

const int pinArusDC = A0;
const int pinTeganganDC = A1;
const int pinArusAC = A2;
const int pinTeganganAC = A3;

// setinggan sensor 
const float VREF = 5.0f;
const int ADC_MAX = 1023;
const float OUTPUT_VOLTAGE_CALIBRATION = 1.992f;
const float R1 = 33.0f; // INI Resistor ygy
const float R2 = 8.2f; 

const float DIVIDER_RATIO = (R1 + R2) / R2;
// V input
float VOLTAGE_CALIBRATION = 0.7000f;
const int OVERSAMPLE_COUNT = 20;
// Set arus
const float MV_PER_A = 185.0f;
const float CURRENT_SCALE = 0.11f / 0.043f;

const uint16_t DC_SAMPLES = 20;
const float NOISE_DEADBAND_A = 0.05f;
const int SAMPLE_WINDOW = 1000;
float CALIBRATION_FACTOR = 1.0f;
const float NOISE_THRESHOLD = 0.02f;
const int NUM_SAMPLES = 10;
// Arus Batre
ACS712 ACS(pinArusDC, VREF, ADC_MAX, MV_PER_A);
float dcCurrentHistory[NUM_SAMPLES];
int dcCurrentHistoryIndex = 0;
bool dcCurrentHistoryFull = false;
float voltageHistory[NUM_SAMPLES];
int voltageHistoryIndex = 0;
bool voltageHistoryFull = false;
float currentHistory[NUM_SAMPLES];
int currentHistoryIndex = 0;
bool currentHistoryFull = false;

// time out LoRa
unsigned long lastTimeSend = 0;
const unsigned long intervalKirim = 1000;
const unsigned long batasTunggu = 500;

// set Sensor smua
float readInstantVoltage() {
  (void)analogRead(pinTeganganDC);

  long sumAdc = 0;
  for (int i = 0; i < OVERSAMPLE_COUNT; i++) {
    sumAdc += analogRead(pinTeganganDC);
  }
  float avgAdc = (float)sumAdc / OVERSAMPLE_COUNT;

  float vOut = (avgAdc / ADC_MAX) * VREF;
  return vOut * DIVIDER_RATIO * VOLTAGE_CALIBRATION;
}

float getSmoothedVoltage() {
  float currentV = readInstantVoltage();
  voltageHistory[voltageHistoryIndex] = currentV;
  voltageHistoryIndex = (voltageHistoryIndex + 1) % NUM_SAMPLES;
  if (voltageHistoryIndex == 0) {
    voltageHistoryFull = true;
  }

  int count = voltageHistoryFull ? NUM_SAMPLES : voltageHistoryIndex;
  if (count == 0) {
    return currentV;
  }

  float total = 0.0f;
  for (int i = 0; i < count; i++) {
    total += voltageHistory[i];
  }
  return total / count;
}
// DC I
float getSmoothedDcCurrentAmps() {
  float currentAmps = (ACS.mA_DC(DC_SAMPLES) / 1000.0f) * CURRENT_SCALE;
  float magnitudeAmps = fabs(currentAmps);

  if (magnitudeAmps < NOISE_DEADBAND_A) {
    magnitudeAmps = 0.0f;
  }

  dcCurrentHistory[dcCurrentHistoryIndex] = magnitudeAmps;
  dcCurrentHistoryIndex = (dcCurrentHistoryIndex + 1) % NUM_SAMPLES;
  if (dcCurrentHistoryIndex == 0) {
    dcCurrentHistoryFull = true;
  }

  int count = dcCurrentHistoryFull ? NUM_SAMPLES : dcCurrentHistoryIndex;
  if (count == 0) {
    return magnitudeAmps;
  }

  float sum = 0.0f;
  for (int i = 0; i < count; i++) {
    sum += dcCurrentHistory[i];
  }
  return sum / count;
}

// Bca Arus AC
float readCurrentRms() {
  long sumRaw = 0;
  for (int i = 0; i < SAMPLE_WINDOW; i++) {
    sumRaw += analogRead(pinArusAC);
  }
  float offsetAdc = (float)sumRaw / SAMPLE_WINDOW;

  double sumSq = 0.0;
  for (int i = 0; i < SAMPLE_WINDOW; i++) {
    float diff = (float)analogRead(pinArusAC) - offsetAdc;
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
// DC V
float getSmoothedCurrent() {
  float currentVal = readCurrentRms();
  currentHistory[currentHistoryIndex] = currentVal;
  currentHistoryIndex = (currentHistoryIndex + 1) % NUM_SAMPLES;
  if (currentHistoryIndex == 0) {
    currentHistoryFull = true;
  }

  int count = currentHistoryFull ? NUM_SAMPLES : currentHistoryIndex;
  if (count == 0) {
    return currentVal;
  }

  float sum = 0.0f;
  for (int i = 0; i < count; i++) {
    sum += currentHistory[i];
  }
  return sum / count;
}

// set LoRa
bool initializeLoRa() {
  LoRa.setPins(csPin, resetPin, DIO0);
  if (!LoRa.begin(433E6)) {
    return false;
  }

  LoRa.setSyncWord(0x11F3);
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setCodingRate4(5);
  LoRa.setPreambleLength(8);
  LoRa.setTxPower(17);
  LoRa.enableCrc();
  return true;
}

bool recoverLoRa() {
  LoRa.end();
  delay(10);
  return initializeLoRa();
}

bool waitForAck() {
  LoRa.receive();
  const unsigned long waktuMulai = millis();

  while (millis() - waktuMulai < batasTunggu) {
    int packetSize = LoRa.parsePacket();
    if (packetSize > 0) {
      String ack = "";
      while (LoRa.available()) {
        ack += (char)LoRa.read();
      }

      if (ack == "ACK_GEN1") {
        return true;
      }
    }

    delay(2);
  }

  return false;
}

void setup() {
  Serial.begin(115200);
  while (!Serial)
    ;
  delay(1000);

  for (int i = 0; i < NUM_SAMPLES; i++) {
    currentHistory[i] = 0.0f;
    dcCurrentHistory[i] = 0.0f;
    voltageHistory[i] = 0.0f;
  }

  ACS.autoMidPoint();

  Serial.println("LoRa Transmitter Module Setup...");

  while (!initializeLoRa()) {
    Serial.println("LoRa gagal diinisialisasi, mencoba kembali dalam 1 detik...");
    delay(1000);
  }

  Serial.println("LoRa Transmitter Ready");
}

void loop() {
  if (millis() - lastTimeSend >= intervalKirim) {
    lastTimeSend = millis();

    float iGenerator = getSmoothedDcCurrentAmps();

    float vGenerator = getSmoothedVoltage();

    float iOutput = getSmoothedCurrent();

    int rawTeganganAC = analogRead(pinTeganganAC);
    
    float vOutput = ((float)rawTeganganAC / ADC_MAX) * VREF * OUTPUT_VOLTAGE_CALIBRATION;

    String dataTosend = "GEN_TX1,GEN_RX1," + 
                        String(vGenerator, 2) + "," +
                        String(iGenerator, 2) + "," +
                        String(vOutput, 2) + "," +
                        String(iOutput, 2);

    Serial.println("Send Data Ke Receiver");
    Serial.println(dataTosend);

    int statusKirim = LoRa.beginPacket();
    if (statusKirim) {
      LoRa.print(dataTosend);
      int statusSelesai = LoRa.endPacket();
      if (statusSelesai && waitForAck()) {
        Serial.println("Data diterima receiver\n");
      } else {
        Serial.println("Tidak ada feedback dari receiver, memulai recovery...");
        if (recoverLoRa()) {
          Serial.println("Recovery LoRa berhasil; mencoba mengirim ulang");
        } else {
          Serial.println("Recovery LoRa gagal; mencoba kembali");
        }
      }
    } else {
      Serial.println("Gagal memulai paket, memulai recovery...");
      if (recoverLoRa()) {
        Serial.println("Recovery LoRa berhasil; mencoba mengirim ulang");
      } else {
        Serial.println("Recovery LoRa gagal; mencoba kembali");
      }
    }
  }
}