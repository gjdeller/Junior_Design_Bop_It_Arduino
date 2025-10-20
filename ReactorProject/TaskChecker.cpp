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
    if (currentTask.requiredK == K_ANY) {
        kOK = true;
    }
    else if (currentTask.requiredK == K_CRITICAL) {
        kOK = (fabsf(currentK - 1.0f) <= K_TOLERANCE);
    }
    else if (currentTask.requiredK == K_SUBCRITICAL) {
        kOK = (currentK < 1.0f);
    }
    else if (currentTask.requiredK == K_SUPERCRITICAL) {
        kOK = (currentK > 1.0f);
    }

    // Task complete only if both rod and k conditions are met
    return (rodOK && kOK);
}
