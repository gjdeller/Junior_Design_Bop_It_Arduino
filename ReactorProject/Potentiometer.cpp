#include "Potentiometer.h"
#include "Pins.h"
#include "ReactorPhysics.h"
#include "PhysicsConstants.h"
#include <Arduino.h>

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
