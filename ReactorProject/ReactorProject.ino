#include "Potentiometer.h"

void setup() {
  Serial.begin(9600);
  initPotentiometer();
}

void loop() {
  float rod = readRodInsertion();
  float sigma = readMac();

  Serial.print("Rod Insertion (%): ");
  Serial.print((1 - rod) * 100, 1);
  Serial.print(" | Σ (Macroscopic): ");
  Serial.print(sigma, 6); // show small cm^-1 values
  Serial.println(" cm^-1");

  delay(1000);
}
