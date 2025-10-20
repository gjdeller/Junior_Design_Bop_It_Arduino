#include "TaskChecker.h"
#include "PhysicsConstants.h"
#include <cmath> // for fabsf()

bool checkTaskCompletion(TaskRequirements currentTask,
                         float currentRod,
                         float currentK,
                         float currentMaintainTarget) 
{
    // Normalize rod tolerance (5.0f = ±5%)
    //const float rodTolerance = ROD_TOLERANCE / 100.0f;

    // --- 1. Check rod position ---
    float targetRod = (currentTask.requiredRodInsertion < 0.0f)
                      ? currentMaintainTarget   // "Maintain" task
                      : currentTask.requiredRodInsertion; // Normal task

    bool rodOK = (fabsf(currentRod - targetRod) <= ROD_TOLERANCE);

    // --- 2. Check reactor k_eff condition ---
    bool kOK = false;

    // Case maintain current status: any K value is fine
    if (currentTask.requiredK == K_ANY) {
        kOK = true;
    }
    // Check if K is K_Critical this means that it is approximately = 1, and the reactor is under steady-state conditions.
    else if (currentTask.requiredK == K_CRITICAL) {
        kOK = (fabsf(currentK - 1.0f) <= K_TOLERANCE);
    }
    // Check if K is subcritical. this means K < 1, and that the reactor is decreasing energy production. 
    else if (currentTask.requiredK == K_SUBCRITICAL) {
        kOK = (currentK < 1.0f);
    }

      // Check if K = supercritical, K > 1, this means that energy is increasing
    else if (currentTask.requiredK == K_SUPERCRITICAL) {
        kOK = (currentK > 1.0f);
    }

    // Task complete only if both rod and k conditions are met
    return (rodOK && kOK);
}
