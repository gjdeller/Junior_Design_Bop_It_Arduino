#ifndef USERCOMMANDS_H
#define USERCOMMANDS_H
#include <Arduino.h>

// ReactorPhysics.h defines the ReactorPhysics class.
enum K_Condition{
  K_ANY,          // No K check required (just position or timer)
  K_CRITICAL,     // K = 1 (approximately) --> Steady State
  K_SUBCRITICAL,  // K < 1.0 (Decrease Power) --> Subcritical
  K_SUPERCRITICAL, // K > 1.0 (Increase Power) --> Supercritical
  E_STOP,
  CALL
};

struct TaskRequirements{
  float requiredRodInsertion; // this is the target rod insertion (0.00 to 1.00)
  K_Condition requiredK; // This is the required K value or state
  //bool requiredEStop = false; // bool for emergency stop, must be set to true when metldown 
};

void initUserCommands();
float getCommand1(int score, float time);
TaskRequirements getCommand2(int score, float time);

#endif