#include "Potentiometer.h"
#include "ReactorPhysics.h"

ReactorPhysics reactor;  // global reactor instance

void setup() {
  Serial.begin(9600);
  initPotentiometer();   // initialize potentiometer hardware
}

void loop() {
  reactor.update();      // run all physics updates

  float rod = 1.0f - readRodInsertion();  // invert if you want “insertion” %
  Serial.print("Rod Insertion (%): ");
  Serial.print(rod * 100.0f, 1);

  Serial.print(" | Σ (Macroscopic): ");
  Serial.print(reactor.macro, 6);
  Serial.print(" cm^-1");

  Serial.print(" | Reaction Rate: ");
  Serial.printf("%e", reactor.reactionRate);

  Serial.print(" | k_eff: ");
  Serial.println(reactor.k, 4);

  delay(1000);
}
