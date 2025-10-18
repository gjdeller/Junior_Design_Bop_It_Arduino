#include "UserCommands.h"
#include "Pins.h"
#include "ReactorPhysics.h"
#include <Arduino.h>

void initUserCommands(){
  // Configures all pins and stuff for the commands
  pinMode(BTN1, INPUT_PULLUP);
}
// Command 1 Method: Returns 1 if the player succeeds (pressed the button in time or 0.0 if they fail)
float getCommand1(){

  unsigned long startTime = millis();

  const unsigned long timeout = 7000; // 7 seconds

  // iNITIALIZES SUCCESS AS FALSE (NO SUCCESS YET)
  bool success = false;

  // PRINTS OUT THE FIRST COMMAND
  Serial.println("Command 1: Select a reactor (press button within 7 seconds)");

  // STARTS A LOOP THAT RUNS UNITL 7 SECONDS HAVE PASSED (ESSENTIALLY A WHILE LOOP)
  while (millis() - startTime < timeout){
    // CHECKS IF BUTTON IS PRESSED
    if (digitalRead(BTN1) == LOW){
      // PRINTS OUT THAT REACTOR 1 WAS SELECTED
      Serial.println("Reactor 1 selected");
      // SETS SUCCESS BOOL TO TRUE
      success = true;
      break;
    }
    delay(10); // small delay to prevent issues
  }

  // IF SUCCESS IS FALSE PRINT OUT NO REACTOR
  if(!success){
    Serial.println("Time expired! No reactor selected.");
  }

  // RETURNS THE SUCCESS BOOL 
  return success ? 1.0 : 0.0;
}


TaskRequirements getCommand2(){
  // success bool
  int command = random(1, 6);
  TaskRequirements task;

  Serial.print("TASK: "); // This will say what task
  
  switch(command){
    case 1:
      Serial.println("Maintain current reactor status");
      task.requiredK = K_ANY;
      break;

    case 2:
      Serial.println("Increase Power, Remove the Control Rod to 25%");
      task.requiredK = K_SUPERCRITICAL; // K > 1
      task.requiredRodInsertion = 0.25f;
      break;

    case 3:
      Serial.println("Decrease Power, Insert the Control Rod to 75%");
      task.requiredK = K_SUBCRITICAL; // K < 1
      task.requiredRodInsertion = 0.75f;
      break;

    case 4:
      Serial.println("Return reactor to steady-state Conditions: Control Rod at 50%");
      task.requiredK = K_CRITICAL;
      task.requiredRodInsertion = 0.50f;
      break;
    case 5:
      Serial.println("MELTDOWN!!! INSERT RODS FULLY AND EMERGENCY STOP!");
      task.requiredRodInsertion = 1.0f; // insert rods the full way
      task.requiredK = K_SUBCRITICAL; // final state should be K < 1 since fission slows down bc rods are fully inserted
      break;
  }

  delay (3000);
  return task;
}

