// TaskChecker.cpp
#include "TaskChecker.h"

// Note: ROD_TOLERANCE and K_TOLERANCE are accessible here via PhysicsConstants.h

bool checkTaskCompletion(TaskRequirements currentTask, float currentRod, float currentK, float currentMaintainTarget
) {
    bool rodOK = false;
    float targetRod = currentTask.requiredRodInsertion;

    // 1. Check Rod Position
    if (targetRod < 0) {
        // Case 1: Maintain Current Position. Use the stored 'currentMaintainTarget'.
        rodOK = (abs(currentRod - currentMaintainTarget) <= ROD_TOLERANCE);
    } else {
        // Fixed target position.
        rodOK = (abs(currentRod - targetRod) <= ROD_TOLERANCE);
    }
    
    // 2. Check K Condition
    bool kOK = false;
    switch (currentTask.requiredK) {
        case K_ANY:
            kOK = true; 
            break;
        case K_CRITICAL:
            kOK = (abs(currentK - 1.0f) <= K_TOLERANCE);
            break;
        case K_SUBCRITICAL:
            kOK = (currentK < 1.0f);
            break;
        case K_SUPERCRITICAL:
            kOK = (currentK > 1.0f);
            break;
    }

    return rodOK && kOK;
}