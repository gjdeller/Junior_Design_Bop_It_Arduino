// ReactorProject.ino

#include "Potentiometer.h"
#include "ReactorPhysics.h"
#include "UserCommands.h"
#include "PhysicsConstants.h"
#include "TaskChecker.h"   // checkTaskCompletion(...)
#include "Pins.h"          // LED pins, button pins, etc.
#include "DisplayGauge.h"
#include "DisplayPowerGauge.h"
#include "LCDScreen.h"

// -------- Global Objects --------
ReactorPhysics reactor;  // single global instance

// -------- Task State --------
TaskRequirements currentTask;
unsigned long taskStartTime = 0;
unsigned long taskDeadline  = 0;   // fixed end time for each task
bool taskActive = false;
float currentMaintainTarget = 0.5f; // for "Maintain" task

// Hold requirement (prevents instant pass). Tune as desired.
const unsigned long HOLD_MS = 2000;  // 2 seconds required in-correct-state

// Track when condition first became true
static bool conditionMet = false;
static unsigned long conditionMetSince = 0;

// LED blink (for confirmation that user is at the correct value)

// this is how many times it should tick to make sure you're correct
static unsigned long ledTick = 0;

// LEd bool
static bool ledOn = false;

// This is for blinking the green led
const unsigned long GREEN_BLINK_PERIOD_MS = 250; // 2 Hz blink

// bool to tell the system that command 1 has finished. 
bool ranCommand1 = false;
float totalScore = 0.0f;

// ================= Setup =================
void setup() {
  Serial.begin(9600);

  // Initialize the Potentiometer and User Commands
  initPotentiometer();
  initUserCommands();

  // Green LED Pin and Red LED Pin initialization for the users correct action / reactor status.
  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);
  
  // Initialize the LED Outputs to Low
  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Initialize the Fission Rate Gauge
  Gauge_Init();

  // Initialize the Power Output Gauge
  ControlRodGauge_Init();
  LCD_init();

  Gauge_SetValue(0);
  ControlRodGauge_SetValue(0);

  Serial.println("All systems initialized. Reactor Simulation running...");
}

// ================= Loop =================
void loop() {
  // This is an if statement that checks if command1 has completed.
  if (!ranCommand1) {
    // Get Command 1, returns a 1 or 0, depending on whether or not you get it correct
    // Therefore, it increements by 1 point to the total score.
    float points = getCommand1(totalScore);
    totalScore += points;

    // Sets ran command to true after command 1
    ranCommand1 = true;

    // Print out the initial score after the command
    Serial.print("\nInitial Score after Command 1: ");

    // Prints out score
    Serial.println(totalScore, 1);

    // delay
    delay(500);
  }

  // 2) Physics update & read sensors
  reactor.update();

  // Receives the rod insertion, k value, fission rate for outputs and amth
  float currentRodInsertion = readRodInsertion(); // 0.0..1.0
  float currentK            = reactor.k;
  //float powerOutput         = reactor.power; 
  float currentFissionRate = reactor.reactionRate; // available if needed
  //PowerGauge_SetValue(currentRodInsertion);
  //Gauge_SetValue(currentFissionRate);

  // 3) Task management (fixed-duration with hold requirement)
  if (taskActive) {
    unsigned long now = millis();

    // this bool checks if the task has been completed 
    // so it calls the checkTaskCompletion method in the Task Checker file, which 
    // takes in the current task (K_subcritical, K_supercritical, K_subcritical, etc) as well the current K value,
    // rod insertion, and the target rod insertion and returns whetehr or not you got it correct
    bool okNow = checkTaskCompletion(currentTask, currentRodInsertion, currentK, currentMaintainTarget);

    // if the task is compeleted,
    if (okNow) {
      if (!conditionMet) {
        conditionMet = true;
        conditionMetSince = now;
      }
    } else {
      conditionMet = false; // reset if user drifts out of tolerance
    }

    // Success condition if we ended now (must hold for HOLD_MS)
    // If the condition is completed 
    bool successIfEndedNow = (conditionMet && (now - conditionMetSince >= HOLD_MS));

    // Checks if successful operation.
    // If not correct, RED LED turns on.
    if (!okNow) {
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
    } 
    // If successful, GREEN LED will blink for 3 blinks.
    else if (!successIfEndedNow) {
      // Blinks green to check if it remains green for the blink period.
      if (now - ledTick >= GREEN_BLINK_PERIOD_MS) {
        ledTick = now;
        ledOn = !ledOn;
      }
      // turns solid green if stayed in range for correct. 
      digitalWrite(GREEN_LED_PIN, ledOn ? HIGH : LOW);
      digitalWrite(RED_LED_PIN, LOW);
    } else {
      // IN SPEC and HOLD MET -> solid GREEN
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }
    // -------------------------------------------

    // If task is completed during the countdown, increase score by 1
    if ((long)(now - taskDeadline) >= 0) {
      if (successIfEndedNow) {
        Serial.println("TASK SUCCESSFUL! Score +1.");

        // set the Green LED Pin to High to show that the user successfully operated the reactor
        digitalWrite(GREEN_LED_PIN, HIGH);

        // Set the Red LED pIn to low to signal that it was a correct operation
        digitalWrite(RED_LED_PIN, LOW);
        totalScore += 1.0f;
      } else {
        Serial.println("TASK FAILED! Time expired. Score reset.");
        digitalWrite(RED_LED_PIN, HIGH);
        digitalWrite(GREEN_LED_PIN, LOW);
        totalScore = 0.0f;
      }

      taskActive = false;
      delay(500); // small pause before next task
    }
  }

  // Start Command 2
  if (!taskActive) {
    // Delay
    delay(500); 
    Serial.println("\n--- NEW TASK INITIATED ---");

    // Get the second command
    // Command2: 
    //    Increase Power (Control Rod at 25%), 
    //    Decrease Power (Control Rod at 75%), 
    //    Set Reactor to Critical State (Control Rod at 50%)
    //    MELTDOWN       (Control Rods at 100%)
    //    Maintain Power (Keep Rod at Current Position)
    currentTask = getCommand2(totalScore);
    // Maintain: lock current rod position as target
    if (currentTask.requiredRodInsertion < 0.0f) {
      currentMaintainTarget = currentRodInsertion;
      Serial.print("Target: Hold rod at ");
      Serial.print(currentMaintainTarget * 100.0f, 1);
      Serial.println("%");
    }

    // Fixed-duration task window
    taskStartTime = millis();
    taskDeadline  = taskStartTime + TASK_TIME_MS;
    conditionMet = false;
    conditionMetSince = 0;

    // Reset blink state
    ledTick = taskStartTime;
    ledOn = false;

    taskActive = true;

    // Start with RED on (actively working)
    digitalWrite(RED_LED_PIN, HIGH);
    digitalWrite(GREEN_LED_PIN, LOW);
  }

  // 5) Status output (with target rod & time remaining)
  //Serial.print("Rod (%): ");
  //Serial.print(currentRodInsertion * 100.0f, 1);

  Serial.print(" | k_eff: ");
  Serial.print(currentK, 4);

  Serial.print(" | Score: ");
  Serial.print(totalScore, 1);

  // Print out the Fission Cross Section:
  Serial.print(" | Macroscopic Cross Section: ");
  Serial.print(reactor.macro);

  // Sets the current Fission Rate to the Gauge
  //Serial.print(" | Fission Rate: ");
  //Serial.print(currentFissionRate / 1e6, 2);
  // Divide the value by 1e6 to make it smaller and easier to display
  Gauge_SetValue(currentFissionRate / 1e6);

  //Serial.print(" | Power Output [J/s]: ");
  //Serial.print(reactor.power);
  

  // Target rod display (maintain vs fixed)
  float targetInsertion = (currentTask.requiredRodInsertion < 0.0f)
                          ? currentMaintainTarget
                          : currentTask.requiredRodInsertion;

  ControlRodGauge_SetValue(currentRodInsertion * 100.0f);

  Serial.print(" | Target Rod: ");
  Serial.print(targetInsertion * 100.0f, 1);
  Serial.print("%");

  // Time remaining based on fixed deadline
  if (taskActive) {
    long msLeft = (long)(taskDeadline - millis());
    if (msLeft < 0) msLeft = 0;
    Serial.print(" | Time: ");
    Serial.print(msLeft / 1000.0f, 1);
    Serial.print("s");
  }

  Serial.println();

  

  delay(100);
}
