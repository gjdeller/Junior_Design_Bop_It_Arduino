// ReactorProject.ino

#include "Potentiometer.h"
#include "ReactorPhysics.h"
#include "UserCommands.h"
#include "PhysicsConstants.h"
#include "TaskChecker.h"   // checkTaskCompletion(...)
#include "Pins.h"          // LED pins, button pins, etc.
#include "DisplayGauge.h"
#include "DisplayPowerGauge.h"
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

// LED blink (for "in spec, holding") — non-blocking
static unsigned long ledTick = 0;
static bool ledOn = false;
const unsigned long GREEN_BLINK_PERIOD_MS = 250; // 2 Hz blink

// -------- Game State --------
bool ranCommand1 = false;
float totalScore = 0.0f;

// ================= Setup =================
void setup() {
  Serial.begin(9600);

  initPotentiometer();
  initUserCommands();

  pinMode(GREEN_LED_PIN, OUTPUT);
  pinMode(RED_LED_PIN, OUTPUT);

  digitalWrite(GREEN_LED_PIN, LOW);
  digitalWrite(RED_LED_PIN, LOW);
  
  // Initialize the Fission Rate Gauge
  Gauge_Init();
  PowerGauge_Init();

  Gauge_SetValue(0);
  PowerGauge_SetValue(0);

  Serial.println("All systems initialized. Reactor Simulation running...");
}

// ================= Loop =================
void loop() {
  // 1) Command 1 (runs once at start)
  if (!ranCommand1) {
    float points = getCommand1();
    totalScore += points;
    ranCommand1 = true;

    Serial.print("\nInitial Score after Command 1: ");
    Serial.println(totalScore, 1);
    delay(500);
  }

  // 2) Physics update & read sensors
  reactor.update();
  float currentRodInsertion = readRodInsertion(); // 0.0..1.0
  float currentK            = reactor.k;
  float powerOutput         = reactor.power; 
  float currentFissionRate = reactor.reactionRate; // available if needed
  //PowerGauge_SetValue(currentRodInsertion);
  //Gauge_SetValue(currentFissionRate);

  // 3) Task management (fixed-duration with hold requirement)
  if (taskActive) {
    unsigned long now = millis();

    // Is the task requirement currently satisfied?
    bool okNow = checkTaskCompletion(currentTask, currentRodInsertion, currentK, currentMaintainTarget);

    // Track continuous time in-correct-state
    if (okNow) {
      if (!conditionMet) {
        conditionMet = true;
        conditionMetSince = now;
      }
    } else {
      conditionMet = false; // reset if user drifts out of tolerance
    }

    // Success condition if we ended now (must hold for HOLD_MS)
    bool successIfEndedNow = (conditionMet && (now - conditionMetSince >= HOLD_MS));

    // ----- LIVE LED FEEDBACK (non-blocking) -----
    if (!okNow) {
      // OUT OF SPEC -> solid RED
      digitalWrite(RED_LED_PIN, HIGH);
      digitalWrite(GREEN_LED_PIN, LOW);
    } else if (!successIfEndedNow) {
      // IN SPEC, but still accumulating HOLD_MS -> blink GREEN
      if (now - ledTick >= GREEN_BLINK_PERIOD_MS) {
        ledTick = now;
        ledOn = !ledOn;
      }
      digitalWrite(GREEN_LED_PIN, ledOn ? HIGH : LOW);
      digitalWrite(RED_LED_PIN, LOW);
    } else {
      // IN SPEC and HOLD MET -> solid GREEN
      digitalWrite(GREEN_LED_PIN, HIGH);
      digitalWrite(RED_LED_PIN, LOW);
    }
    // -------------------------------------------

    // Rollover-safe: check if now >= deadline
    if ((long)(now - taskDeadline) >= 0) {
      if (successIfEndedNow) {
        Serial.println("TASK SUCCESSFUL! Score +1.");
        digitalWrite(GREEN_LED_PIN, HIGH);
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

  // 4) Start a new Command 2 task when idle
  if (!taskActive) {
    delay(500); // small breather between tasks
    Serial.println("\n--- NEW TASK INITIATED ---");
    currentTask = getCommand2();

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

  PowerGauge_SetValue(currentRodInsertion * 100.0f);

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
