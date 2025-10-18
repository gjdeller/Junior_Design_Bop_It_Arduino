// TaskChecker.h
#ifndef TASK_CHECKER_H
#define TASK_CHECKER_H

#include <Arduino.h>
#include "UserCommands.h"      // Needed for TaskRequirements and K_Condition struct/enum
#include "PhysicsConstants.h"  // Needed for ROD_TOLERANCE and K_TOLERANCE

// Function prototype:
// Checks if the current reactor state meets the required task criteria.
bool checkTaskCompletion(
    TaskRequirements currentTask,
    float currentRod, 
    float currentK,
    float currentMaintainTarget // Required for the "Maintain" task
);

#endif