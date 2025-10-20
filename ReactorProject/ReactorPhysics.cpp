#include "ReactorPhysics.h"
#include "Potentiometer.h"
#include "PhysicsConstants.h"
#include <Arduino.h>

ReactorPhysics::ReactorPhysics() {
    macro = 0.0f;
    reactionRate = 0.0f;
    k = 0.0f;
}

void ReactorPhysics::update() {
    float rodInsertion = readRodInsertion();  // 0–1 range

    // Macroscopic fission cross section
    macro = 2 * N * SIGMA_MICRO * (1 - rodInsertion);

    // Fission rate = neutron flux × macroscopic cross section
    reactionRate = NEUTRON_FLUX * macro;

    // Thermal utilization factor
    float base_f = 0.8f;
    float min_f  = 0.4f;
    float f = base_f - (base_f - min_f) * (rodInsertion);

    // Four-factor formula
    const float eta = 2.05f;
    const float p = 0.85f;
    const float epsilon = 1.03f;

    k = eta * f * p * epsilon;
}
