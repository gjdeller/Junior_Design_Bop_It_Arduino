#include "Potentiometer.h"
#include <Arduino.h>

// Set constants for the file
const int POT_PIN = 4; // analog input pin for the potentiometer

// Microscopic absorption cross section for U-235 (in cm^2)
const float SIGMA_MICRO = 680e-24; // 680 barns = 680 * 10^-24 cm^2

// Enrichment: 4.5% of uranium atoms are U-235
const float ENRICHMENT = 0.045;

// Total atomic density (atoms/cm^3) for uranium dioxide
const float N_TOTAL = 2.5e22; // atoms/cm^3

// Effective number density of U-235
const float N_U235 = N_TOTAL * ENRICHMENT;

// --- Function Definitions ---

// Utility function: linear map
float floatmap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void initPotentiometer() {
  analogSetAttenuation(ADC_11db); // for full 0–3.3V range on ESP32
}

float readRodInsertion() {
  int analogValue = analogRead(POT_PIN); // should be between 0 and 4095
  float voltage = floatmap(analogValue, 0, 4095, 0.0, 3.3);
  float rodInsertion = voltage / 3.3; // normalize: 0.0 (withdrawn) to 1.0 (inserted)
  return rodInsertion;
}

float readMac() {
  float rodInsertion = readRodInsertion();
  float macro = SIGMA_MICRO * N_U235 * rodInsertion;
  return macro; // Macroscopic cross section in cm^-1
}
