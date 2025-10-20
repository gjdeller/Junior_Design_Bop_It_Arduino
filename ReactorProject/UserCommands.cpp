#include "UserCommands.h"
#include "Pins.h"
#include "ReactorPhysics.h"
#include <Arduino.h>

void initUserCommands(){
  // Configures all pins and stuff for the commands
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
}

// Helper: 0 = none, 1 = only BTN1, 2 = only BTN2, 3 = only BTN3, -1 = multiple
static int readExclusiveButton() {
  bool b1 = (digitalRead(BTN1) == LOW);
  bool b2 = (digitalRead(BTN2) == LOW);
  bool b3 = (digitalRead(BTN3) == LOW);
  int count = (b1 ? 1 : 0) + (b2 ? 1 : 0) + (b3 ? 1 : 0);
  if (count == 0) return 0;
  if (count > 1)  return -1;
  if (b1) return 1;
  if (b2) return 2;
  if (b3) return 3;
  return 0;
}

// Command 1 Method: Returns 1 if success, else 0
float getCommand1() {
  int command = random(1, 4);                // 1..3 inclusive
  const unsigned long timeout = 7000;        // 7 seconds
  const unsigned long debounceMs = 30;
  unsigned long startTime = millis();
  bool success = false;

  if (command == 1) Serial.println("Command 1: Start Reactor 1 (press BTN1 only)");
  else if (command == 2) Serial.println("Command 1: Start Reactor 2 (press BTN2 only)");
  else Serial.println("Command 1: Start Reactor 3 (press BTN3 only)");

  while (millis() - startTime < timeout) {
    int state = readExclusiveButton();

    if (state == 0) {
      // No button yet -> keep waiting
    } else if (state == -1) {
      // Multiple pressed -> immediate fail
      Serial.println("Failed: multiple buttons pressed. Shutting down grid.");
      success = false;
      break;
    } else {
      // Exactly one pressed
      if (state == command) {
        // Debounce: confirm still the same after a short delay
        delay(debounceMs);
        if (readExclusiveButton() == command) {
          Serial.print("Now Controlling Reactor ");
          Serial.println(command);
          success = true;
          break;
        }
      } else {
        Serial.print("Failed: wrong button (BTN");
        Serial.print(state);
        Serial.println("). Shutting down grid.");
        success = false;
        break;
      }
    }

    delay(10);  // light poll delay
  }

  if (!success && millis() - startTime >= timeout) {
    Serial.println("Time expired! No reactor selected.");
  }

  return success ? 1.0f : 0.0f;
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

