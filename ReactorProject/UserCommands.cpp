#include "UserCommands.h"
#include "Pins.h"
#include "ReactorPhysics.h"
#include <Arduino.h>
#include "LCDScreen.h"

void initUserCommands(){
  // Configures all pins and stuff for the commands
  pinMode(BTN1, INPUT_PULLUP);
  pinMode(BTN2, INPUT_PULLUP);
  pinMode(BTN3, INPUT_PULLUP);
  pinMode(BTN4, INPUT_PULLUP);

  // Reactor LEDs:
  pinMode(R1_LED_PIN, OUTPUT);
  pinMode(R2_LED_PIN, OUTPUT);
  pinMode(R3_LED_PIN, OUTPUT);

  // Start all LEDs off
  digitalWrite(R1_LED_PIN, LOW);
  digitalWrite(R2_LED_PIN, LOW);
  digitalWrite(R3_LED_PIN, LOW);

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
float getCommand1(int score, float time) {
  int command = random(1, 4);                // 1..3 inclusive
  const unsigned long timeout = 7000;        // 7 seconds
  const unsigned long debounceMs = 30;
  unsigned long startTime = millis();
  bool success = false;

  if (command == 1){
    //Serial.println("Command 1: Start Reactor 1 (press BTN1 only)");
    LCD_Display("Start Reactor 1", score, time);
  }
  else if (command == 2){
    //Serial.println("Command 1: Start Reactor 2 (press BTN2 only)");
    LCD_Display("Start Reactor 2", score, time);
  }
  else{
    //Serial.println("Command 1: Start Reactor 3 (press BTN3 only)");
    LCD_Display("Start Reactor 3", score, time);
  }
  while (millis() - startTime < timeout) {
    int state = readExclusiveButton();

    if (state == 0) {
      // No button yet -> keep waiting
    } else if (state == -1) {
      // Multiple pressed -> immediate fail
      //Serial.println("Failed: multiple buttons pressed. Shutting down grid.");
      LCD_Display("Failed: No Reactor Selected", score, time);
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

          // Turn on the pin corresponding the the commands:
          digitalWrite(R1_LED_PIN, (command == 1) ? HIGH : LOW);
          digitalWrite(R2_LED_PIN, (command == 2) ? HIGH : LOW);
          digitalWrite(R3_LED_PIN, (command == 3) ? HIGH : LOW);

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
    //LCD_Display("Time Expired!", score);
  }

  return success ? 1.0f : 0.0f;
}


TaskRequirements getCommand2(int score, float time){
  // success bool
  //int command = random(1, 8);
  int command = 7;
  TaskRequirements task;

  //Serial.print("TASK: "); // This will say what task
  
  switch(command){
    case 1:
      Serial.println("Maintain Power");
      task.requiredK = K_ANY;
      LCD_Display("Maintain Power", score, time);
      break;

    case 2:
      Serial.println("Increase Power, Insert to 25%");
      task.requiredK = K_SUPERCRITICAL; // K > 1
      task.requiredRodInsertion = 0.25f;
      LCD_Display("Insert Rod 25%", score, time);
      break;

    case 3:
      Serial.println("Decrease Power, Insert to 75%");
      task.requiredK = K_SUBCRITICAL; // K < 1
      task.requiredRodInsertion = 0.75f;
      LCD_Display("Insert Rod 75%", score, time);
      break;

    case 4:
      Serial.println("Return reactor to steady-state Conditions: Control Rod at 50%");
      task.requiredK = K_CRITICAL;
      task.requiredRodInsertion = 0.50f;
      LCD_Display("Insert Rod 50%", score, time);
      break;
    case 5:
      Serial.println("Produce Zero Power, Insert to 100%");
      task.requiredRodInsertion = 1.0f; // insert rods the full way
      task.requiredK = K_SUBCRITICAL; // final state should be K < 1 since fission slows down bc rods are fully inserted
      //task.requiredEStop = true;
      LCD_Display("Insert Rod 100%", score, time);
      break;
    case 6:
      Serial.println("MELTDOWN and EMERGENCY STOP: INSRT RODS FULLY + ESTOP");
      task.requiredRodInsertion = 1.0f;
      task.requiredK = E_STOP;
      LCD_Display("E-Stop & Ins 100%", score, time);
      break;

    case 7:
      Serial.println("CALL YOUR MANAGER");
      task.requiredK = CALL;
      LCD_Display("Call Manager", score, time);
  }

  delay (1500);
  return task;
}

