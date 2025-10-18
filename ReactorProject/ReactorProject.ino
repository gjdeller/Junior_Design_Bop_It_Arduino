#include "Potentiometer.h"
#include "ReactorPhysics.h"
#include "UserCommands.h"
#include "PhysicsConstants.h"
#include "TaskChecker.h" // Includes the checkTaskCompletion function
#include "Pins.h"         // Define your LED pins here

// --- Global Constants and Objects ---

// Global reactor instance (instantiated once)
ReactorPhysics reactor;  

// Global state variables for task management
TaskRequirements currentTask;
unsigned long taskStartTime = 0;
bool taskActive = false;
float currentMaintainTarget = 0.5f; // Stores target for "Maintain" task

// Command 1 state
bool ranCommand1 = false; 

// Score Value
float totalScore = 0.0f; 

// --- Setup ---

void setup() {
    Serial.begin(9600);
    
    // Initialize hardware modules
    initPotentiometer();
    initUserCommands();

    // Initialize LED pins
    pinMode(GREEN_LED_PIN, OUTPUT);
    pinMode(RED_LED_PIN, OUTPUT);

    Serial.println("All systems initialized. Reactor Simulation running...");
}


// --- Loop ---

void loop() {
    // 1. COMMAND 1: Blocking execution (runs once at start)
    if (!ranCommand1) {
        float points = getCommand1();
        totalScore += points;
        ranCommand1 = true;
        
        Serial.print("\nInitial Score after Command 1: ");
        Serial.println(totalScore, 1);
        delay(1000); 
    }
    
    // 2. CORE PHYSICS UPDATE: Must run every loop iteration to get current Keff!
    reactor.update();
    
    // Get the latest values for task checking
    // NOTE: Assuming readRodInsertion() returns 0.0 (rod out) to 1.0 (rod in).
    float currentRodInsertion = readRodInsertion(); 
    float currentK = reactor.k;
    float currentFissionRate = reactor.reactionRate;                     
    
    // 3. COMMAND 2: Task Management (Non-Blocking)
    
    if (taskActive) {
        
        // A. Check for Completion using the external function
        if (checkTaskCompletion(currentTask, currentRodInsertion, currentK, currentMaintainTarget)) {
            Serial.println("TASK SUCCESSFUL! Score +1.");
            digitalWrite(GREEN_LED_PIN, HIGH);
            digitalWrite(RED_LED_PIN, LOW);
            totalScore += 1.0f;
            taskActive = false; 
            // Pause briefly to register success
            delay(1000); 
        } 
        
        // B. Check for Time Expiration
        else if (millis() - taskStartTime > TASK_TIME_MS) {
            Serial.println("TASK FAILED! Time expired. Score reset.");
            digitalWrite(RED_LED_PIN, HIGH);
            digitalWrite(GREEN_LED_PIN, LOW);
            totalScore = 0.0f; // Penalty for failure
            taskActive = false; 
            delay(2000); 
        }
        
        // C. If task is active and incomplete
        else {
             // Red LED is ON to signal that a task is active and incomplete
             digitalWrite(GREEN_LED_PIN, LOW);
             digitalWrite(RED_LED_PIN, HIGH); 
        }
    }

    // 4. Initiate a new command 2 task
    if (!taskActive) {
        delay(1000); // Wait 1 second before new task
        
        Serial.println("\n--- NEW TASK INITIATED ---");
        currentTask = getCommand2();
        
        // Handle the special "Maintain" case
        if (currentTask.requiredRodInsertion < 0) {
            // Set the target to the current rod position
            currentMaintainTarget = currentRodInsertion; 
            Serial.print("Target: Hold rod at ");
            Serial.print(currentMaintainTarget * 100.0f, 1);
            Serial.println("%");
        }
        
        // Reset timer and activate task state
        taskStartTime = millis();
        taskActive = true;
        
        // Reset LEDs for new active task
        digitalWrite(RED_LED_PIN, HIGH); 
        digitalWrite(GREEN_LED_PIN, LOW);
    }

    // 5. SERIAL OUTPUT: Continuous status update
    
    Serial.print("Rod (%): ");
    Serial.print(currentRodInsertion * 100.0f, 1);
    
    Serial.print(" | k_eff: ");
    Serial.print(currentK, 4);
    
    Serial.print(" | Score: ");
    Serial.print(totalScore, 1);

    // Print time remaining
    if (taskActive) {
      long timeRemaining = TASK_TIME_MS - (millis() - taskStartTime);
      Serial.print(" | Time: ");
      // Only print if time is positive
      if (timeRemaining > 0) Serial.print(timeRemaining / 1000.0f, 1);
      else Serial.print(0.0f, 1);
      Serial.print("s");
    }
    Serial.println();

    delay(100); 
}